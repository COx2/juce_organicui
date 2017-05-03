/*
  ==============================================================================

	Parameter.cpp
	Created: 8 Mar 2016 1:08:19pm
	Author:  bkupe

  ==============================================================================
*/


Parameter::Parameter(const Type &type, const String &niceName, const String &description, var initialValue, var minValue = var(), var maxValue = var(), bool enabled) :
	Controllable(type, niceName, description, enabled),
	isEditable(true),
	isPresettable(true),
	isOverriden(false),
autoAdaptRange(false),
queuedNotifier(100) 
{
	minimumValue = minValue;
	maximumValue = maxValue;
	defaultMinValue = minValue;
	defaultMaxValue = maxValue;

	defaultValue = initialValue;

	resetValue(true);

	scriptObject.setMethod("get", Parameter::getValueFromScript);
	scriptObject.setMethod("set", Controllable::setValueFromScript);
}

Parameter::~Parameter() { 
	Parameter::masterReference.clear(); 
}

void Parameter::resetValue(bool silentSet)
{
	isOverriden = false;
	setValue(defaultValue, silentSet, true);
}

void Parameter::setValue(var _value, bool silentSet, bool force)
{

	if (!force && checkValueIsTheSame(_value, value)) return;
	lastValue = var(value);
	setValueInternal(_value);

	if (_value != defaultValue) isOverriden = true;

	if (!silentSet) notifyValueChanged();
}


bool Parameter::isComplex()
{
	return value.isArray();
}

StringArray Parameter::getValuesNames()
{
	StringArray result;
	if (!isComplex()) result.add("Value");
	else
	{
		for (int i = 0; i < value.size(); i++) result.add("Value " + String(i));
	}
	return result;
}

void Parameter::setRange(var min, var max, bool setDefaultRange) {

	if (setDefaultRange)
	{
		defaultMinValue = min;
		defaultMaxValue = max;
	}

	minimumValue = min;
	maximumValue = max;


	listeners.call(&Listener::parameterRangeChanged, this);
	var arr;
	arr.append(minimumValue); arr.append(maximumValue);
	queuedNotifier.addMessage(new ParamWithValue(this, arr, ParamWithValue::RANGE));

	setValue(value); //if value is outside range, this will change the value
}

void Parameter::setValueInternal(var & _value) //to override by child classes
{
	value = _value;

#ifdef JUCE_DEBUG
	checkVarIsConsistentWithType();
#endif
}

bool Parameter::checkValueIsTheSame(var newValue, var oldValue)
{
	if (oldValue.isArray())
	{
		if (!newValue.isArray()) return false;
		if (newValue.size() != oldValue.size()) return false;
		for (int i = 0; i < oldValue.size(); i++) if (oldValue[i] != newValue[i]) return false;
		return true;
	}

	return newValue == oldValue;
}

void Parameter::checkVarIsConsistentWithType() {
	if (type == Type::STRING)  jassert(value.isString());
	else if (type == Type::INT)     jassert(value.isInt());
	else if (type == Type::BOOL)    jassert(value.isBool());
	else if (type == Type::FLOAT)   jassert(value.isDouble());
}

void Parameter::setNormalizedValue(const float & normalizedValue, bool silentSet, bool force)
{
	setValue(jmap<float>(normalizedValue, (float)minimumValue, (float)maximumValue), silentSet, force);
}

float Parameter::getNormalizedValue()
{
	if (type != FLOAT) return 0;
	if (minimumValue == maximumValue) {
		return 0.0;
	} else
		return jmap<float>((float)value, (float)minimumValue, (float)maximumValue, 0.f, 1.f);
}

void Parameter::notifyValueChanged() {
	listeners.call(&Listener::parameterValueChanged, this);
	queuedNotifier.addMessage(new ParamWithValue(this, value, ParamWithValue::VALUE));
}

var Parameter::getJSONDataInternal()
{
	var data = Controllable::getJSONDataInternal();
	data.getDynamicObject()->setProperty("value", value);
	if (saveValueOnly) return data;
	data.getDynamicObject()->setProperty("minValue", minimumValue);
	data.getDynamicObject()->setProperty("maxValue", maximumValue);
	return data;
}

void Parameter::loadJSONDataInternal(var data)
{
	Controllable::loadJSONDataInternal(data);

	if (!saveValueOnly)
	{
		setRange(data.getProperty("minValue", minimumValue), data.getProperty("maxValue", maximumValue));
	}

	if (data.getDynamicObject()->hasProperty("value"))
	{
		setValue(data.getProperty("value", 0));
	}
}

var Parameter::getValueFromScript(const juce::var::NativeFunctionArgs & a)
{
	Parameter * c = getObjectFromJS<Parameter>(a);
	if (c == nullptr) return var();
	return c->value;
}


//JS Helper
