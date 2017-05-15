/*
 ==============================================================================

 Parameter.h
 Created: 8 Mar 2016 1:08:19pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED


class Parameter : 
	public Controllable,
	public ScriptExpression::Listener
{
public:
	enum ControlMode {
		MANUAL,
		EXPRESSION,
		REFERENCE
	};

    Parameter(const Type &type, const String & niceName, const String &description, var initialValue, var minValue, var maxValue, bool enabled = true);
	virtual ~Parameter();

    var defaultValue;
    var value;
    var lastValue;
    var minimumValue;
    var maximumValue;

	var defaultMinValue;//for autoAdaptRange
	var defaultMaxValue;
   
	//Control Mode
	bool lockManualControlMode;
	ControlMode controlMode;
	String controlExpression;
	ScopedPointer<ScriptExpression> expression;
	WeakReference<Controllable> controlReference;

	bool isComplex();
	virtual StringArray getValuesNames();

	virtual void setRange(var,var, bool setDefaultRange = true);

	bool isPresettable;
    bool isOverriden;
	bool autoAdaptRange;

	void setControlMode(ControlMode _mode);
	void setControlExpression(const String &);

	virtual var getValue(); //may be useful, or testing expression or references (for now, forward update from expression timer)

    void resetValue(bool silentSet = false);
    virtual void setValue(var _value, bool silentSet = false, bool force = false);
    virtual void setValueInternal(var & _value);

	virtual bool checkValueIsTheSame(var newValue, var oldValue); //can be overriden to modify check behavior

    //For Number type parameters
    void setNormalizedValue(const float &normalizedValue, bool silentSet = false, bool force = false);
    float getNormalizedValue();

    //helpers for fast typing
    virtual float floatValue() { return (float)getValue(); }
	virtual double doubleValue(){return (double)getValue();}
	virtual int intValue() { return (int)getValue(); }
	virtual bool boolValue() { return (bool)getValue(); }
	virtual String stringValue() { return getValue().toString(); }

    void notifyValueChanged();

	//From Script Expression
	virtual void expressionValueChanged(ScriptExpression *) override;
	virtual void expressionStateChanged(ScriptExpression *) override;
	
	InspectableEditor * getEditor(bool isRoot) override;

	virtual var getJSONDataInternal() override;
	virtual void loadJSONDataInternal(var data) override;
	
	static var getValueFromScript(const juce::var::NativeFunctionArgs &a);


    //Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void parameterValueChanged(Parameter * ) {};
		virtual void parameterRangeChanged(Parameter * ) {};
		virtual void parameterControlModeChanged(Parameter *) {}
    };

    ListenerList<Listener> listeners;
    void addParameterListener(Listener* newListener) { listeners.add(newListener); }
    void removeParameterListener(Listener* listener) { listeners.remove(listener); }
	


    // ASYNC
    class  ParameterEvent
	{
    public:
		enum Type { VALUE_CHANGED, BOUNDS_CHANGED, CONTROLMODE_CHANGED, EXPRESSION_STATE_CHANGED };

		ParameterEvent(Type t,Parameter * p,var v = var()) :
			type(t),parameter(p),value(v)
		{
		}

		Type type;
        Parameter * parameter;
        var value;
    };

    QueuedNotifier<ParameterEvent> queuedNotifier;
    typedef QueuedNotifier<ParameterEvent>::Listener AsyncListener;


    void addAsyncParameterListener(AsyncListener* newListener) { queuedNotifier.addListener(newListener); }
    void addAsyncCoalescedListener(AsyncListener* newListener) { queuedNotifier.addAsyncCoalescedListener(newListener); }
    void removeAsyncParameterListener(AsyncListener* listener) { queuedNotifier.removeListener(listener); }


	
private:



    bool checkVarIsConsistentWithType();

    WeakReference<Parameter>::Master masterReference;
    friend class WeakReference<Parameter>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)

};


#endif  // PARAMETER_H_INCLUDED
