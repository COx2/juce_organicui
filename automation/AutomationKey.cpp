/*
  ==============================================================================

	AutomationKey.cpp
	Created: 11 Dec 2016 1:22:20pm
	Author:  Ben

  ==============================================================================
*/

AutomationKey::AutomationKey(float minimumValue, float maximumValue) :
	BaseItem("Key",false)
{
	itemDataType = "AutomationKey";
	hideEditorHeader = true;

	position = addFloatParameter("Position", "Position of the key", 0);
	position->defaultUI = FloatParameter::TIME;
	value = addFloatParameter("Value", "Value of the key", 0, minimumValue, maximumValue);

	easingType = addEnumParameter("EasingType", "Type of transition to the next key");

	easingType->addOption("Linear", Easing::LINEAR);
	easingType->addOption("Bezier", Easing::BEZIER);
	easingType->addOption("Hold", Easing::HOLD);
	easingType->addOption("Sine", Easing::SINE);

	easingType->setValueWithData(Easing::LINEAR);
	easingType->hideInEditor = true;
	easingType->forceSaveValue = true;

	canInspectChildContainers = false;

	setEasing(Easing::LINEAR);

	helpID = "AutomationKey";
}


AutomationKey::~AutomationKey()
{
}

void AutomationKey::setEasing(Easing::Type t)
{

	if (easing != nullptr)
	{
		if (easing->type == t) return;
		removeChildControllableContainer(easing);
	}

	switch (t)
	{
	case Easing::LINEAR:
		easing = new LinearEasing();
		break;

	case Easing::HOLD:
		easing = new HoldEasing();
		break;

	case Easing::BEZIER:
		easing = new CubicEasing();
		break;

	case Easing::SINE:
		easing = new SineEasing();
		break;
	}

	if (easing != nullptr)
	{
        easing->setSelectionManager(selectionManager);
		addChildControllableContainer(easing);
	}
}

float AutomationKey::getValue(AutomationKey * nextKey, const float & _pos)
{

	float relPos = 0;
	if(position->floatValue() < nextKey->position->floatValue()) relPos = jmap<float>(_pos, position->floatValue(), nextKey->position->floatValue(), 0, 1); 

	if (relPos < 0) relPos = 0;
	if (relPos > 1) relPos = 1;

	return easing->getValue(value->floatValue(), nextKey->value->floatValue(), relPos);
}

void AutomationKey::setSelectionManager(InspectableSelectionManager * ism)
{
	BaseItem::setSelectionManager(ism);
	if (easing != nullptr) easing->setSelectionManager(ism);
}

void AutomationKey::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == easingType)
	{
		setEasing((Easing::Type)(int)easingType->getValueData());
	}
}

var AutomationKey::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (easing != nullptr) data.getDynamicObject()->setProperty("easing", easing->getJSONData());
	return data;
}

void AutomationKey::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	if (easing != nullptr) easing->loadJSONData(data.getProperty("easing", var()));
}
