/*
==============================================================================

Script.cpp
Created: 20 Feb 2017 5:01:11pm
Author:  Ben

==============================================================================
*/

Script::Script(ScriptTarget * _parentTarget, bool canBeDisabled) :
	BaseItem("Script", canBeDisabled, false),
	scriptParamsContainer("params"),
	parentTarget(_parentTarget),
	scriptAsyncNotifier(10)
{
	isSelectable = false;

	filePath = addStringParameter("File Path", "Path to the script file", "");
	logParam = addBoolParameter("Log", "Utility parameter to easily activate/deactivate logging from the script", false);
	logParam->setCustomShortName("enableLog");

	reload = addTrigger("Reload", "Reload the script");

	scriptObject.setMethod("log", Script::logFromScript);
	scriptObject.setMethod("addBoolParameter", Script::addBoolParameterFromScript);
	scriptObject.setMethod("addIntParameter", Script::addIntParameterFromScript);
	scriptObject.setMethod("addFloatParameter", Script::addFloatParameterFromScript);
	scriptObject.setMethod("addEnumParameter", Script::addEnumParameterFromScript);
	scriptObject.setMethod("addTargetParameter", Script::addTargetParameterFromScript);
	scriptObject.setMethod("addTrigger", Script::addTriggerFromScript);

	addChildControllableContainer(&scriptParamsContainer);
}

Script::~Script()
{

}

void Script::loadScript()
{
	String path = filePath->stringValue();
	if (path.isEmpty())
	{
		return;
	}

#if JUCE_WINDOWS
	if (path.startsWithChar('/')) //avoid jassertfalse
	{
		NLOG(niceName, "Path " + path + " is not a valid Windows-style path.");
		setState(SCRIPT_ERROR);
		return; //OSX / Linux file
	}
#endif


	File f = Engine::mainEngine->getFile().getParentDirectory().getChildFile(path);

	if (!f.exists())
	{
		//check local director
		NLOG("Script", "File not found : " + f.getFileName());
		setState(SCRIPT_EMPTY);
		return;
	}

	fileName = f.getFileName();
	nameParam->setValue(f.getFileNameWithoutExtension());

	String s = f.loadFileAsString();

	stopTimer();

	buildEnvironment();
	Result result = scriptEngine->execute(s);

	if (result.getErrorMessage().isEmpty())
	{
		NLOG("Script : " + niceName, "Script loaded succesfully");
		setState(SCRIPT_LOADED);
	} else
	{
		NLOG("Script : " + niceName, "Script error : " + result.getErrorMessage());
		setState(SCRIPT_ERROR);
		return;
	}

	const NamedValueSet props = scriptEngine->getRootObjectProperties();
	updateEnabled = props.contains(updateIdentifier);
	if (updateEnabled)
	{
		lastUpdateTime = (float)Time::getMillisecondCounter() / 1000.f;
		startTimerHz(30); //should be parametrable
	}


}

void Script::buildEnvironment()
{
	//clear phase
	setState(SCRIPT_CLEAR);

	scriptEngine = new JavascriptEngine();
	while (scriptParamsContainer.controllables.size() > 0) scriptParamsContainer.removeControllable(scriptParamsContainer.controllables[0]);
	scriptParamsContainer.clear();

	scriptEngine->registerNativeObject("script", createScriptObject()); //force "script" for this objet
	if (parentTarget != nullptr) scriptEngine->registerNativeObject("local", parentTarget->createScriptObject()); //force "local" for the related object
	if (Engine::mainEngine != nullptr) scriptEngine->registerNativeObject(Engine::mainEngine->scriptTargetName, Engine::mainEngine->createScriptObject());
	if (ScriptUtil::getInstanceWithoutCreating() != nullptr) scriptEngine->registerNativeObject(ScriptUtil::getInstance()->scriptTargetName, ScriptUtil::getInstance()->createScriptObject());
}

void Script::setState(ScriptState newState)
{
	state = newState;
	scriptAsyncNotifier.addMessage(new ScriptEvent(ScriptEvent::STATE_CHANGE));
}

bool Script::callFunction(const Identifier & function, const Array<var> args)
{
	if (canBeDisabled && !enabled->boolValue()) return false;

	if (scriptEngine == nullptr) return false;
	Result result = Result::ok();
	scriptEngine->callFunction(function, var::NativeFunctionArgs(var::undefined(), (const var *)args.begin(), args.size()), &result);
	if (result.getErrorMessage().isNotEmpty())
	{
		NLOG(niceName, "Script Error :\n" + result.getErrorMessage());
		return false;
	}

	return true;
}


void Script::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == filePath) loadScript();
}

void Script::onContainerTriggerTriggered(Trigger * t)
{
	if (t == reload)
	{
		loadScript();
	}
}

void Script::controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	if (cc == &scriptParamsContainer)
	{
		Array<var> args;
		args.add(c->createScriptObject());
		callFunction("scriptParamChanged", args);
	}
}

void Script::timerCallback()
{
	float curTime = (float)Time::getMillisecondCounter() / 1000.f;
	float deltaTime = curTime - lastUpdateTime;
	lastUpdateTime = curTime;

	Array<var> args;
	args.add(deltaTime);

	bool result = callFunction(updateIdentifier, args);
	if (!result) stopTimer();
}

var Script::logFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = ScriptTarget::getObjectFromJS<Script>(args);
	if (!s->logParam->boolValue()) return var();

	for (int i = 0; i < args.numArguments; i++)
	{
		if (i == 0) NLOG("Script : " + s->niceName, args.arguments[i].toString());
		else NLOG("", args.arguments[i].toString());
	}

	return var();
}

var Script::addBoolParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 3)) return var();
	return s->scriptParamsContainer.addBoolParameter(args.arguments[0], args.arguments[1], (bool)args.arguments[2])->createScriptObject();
}

var Script::addIntParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 5)) return var();
	return s->scriptParamsContainer.addIntParameter(args.arguments[0], args.arguments[1], (int)args.arguments[2], (int)args.arguments[3], (int)args.arguments[4])->createScriptObject();
}

var Script::addFloatParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 5)) return var();
	return s->scriptParamsContainer.addFloatParameter(args.arguments[0], args.arguments[1], (float)args.arguments[2], (float)args.arguments[3], (float)args.arguments[4])->createScriptObject();
}

var Script::addStringParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 3)) return var();
	return s->scriptParamsContainer.addStringParameter(args.arguments[0], args.arguments[1], args.arguments[2])->createScriptObject();
}

var Script::addEnumParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 2)) return var();
	return s->scriptParamsContainer.addEnumParameter(args.arguments[0], args.arguments[1])->createScriptObject();
}

var Script::addTargetParameterFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 2)) return var();
	return s->scriptParamsContainer.addTargetParameter(args.arguments[0], args.arguments[1])->createScriptObject();
}

var Script::addTriggerFromScript(const var::NativeFunctionArgs & args)
{
	Script * s = getObjectFromJS<Script>(args);
	if (!checkNumArgs(s->niceName, args, 2)) return var();
	return s->scriptParamsContainer.addTrigger(args.arguments[0], args.arguments[1])->createScriptObject();
}

bool Script::checkNumArgs(const String &logName, const var::NativeFunctionArgs & args, int expectedArgs)
{
	if (args.numArguments != expectedArgs)
	{
		NLOG(logName, "Error addTargetParameter takes " + String(expectedArgs) + " arguments, got " + String(args.numArguments));
		if (args.numArguments > 0) NLOG("", "When tying to add : " + args.arguments[0].toString());
		return false;
	}

	return true;
}
