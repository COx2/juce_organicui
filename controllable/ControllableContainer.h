/*
  ==============================================================================

    ControllableContainer.h
    Created: 8 Mar 2016 1:15:36pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLECONTAINER_H_INCLUDED
#define CONTROLLABLECONTAINER_H_INCLUDED


#include "preset/PresetManager.h"

class ControllableContainer :
	public Parameter::Listener,
	public Controllable::Listener,
	public Parameter::AsyncListener,
	public Trigger::Listener,
	public ControllableContainerListener,
	public Inspectable,
	public ScriptTarget

{
public:
	ControllableContainer(const String &niceName);
	virtual ~ControllableContainer();

	String niceName;
	String shortName;
	bool hasCustomShortName;

	bool canHavePresets;
	StringParameter * currentPresetName;
	Trigger * savePresetTrigger;
	PresetManager::Preset * currentPreset;

	bool skipControllableNameInAddress;
	void setNiceName(const String &_niceName);
	void setCustomShortName(const String &_shortName);
	void setAutoShortName();
	void setCanHavePresets(bool value);

	bool nameCanBeChangedByUser;
	bool isTargettable; //for controllableChooser

	//Editor
	bool hideInEditor;
	bool canInspectChildContainers;

	//save / load
	bool presetSavingIsRecursive;
	bool saveAndLoadRecursiveData;
	bool saveAndLoadName;

	//Script
	bool includeInScriptObject;

	static ControllableComparator comparator;

	Uuid uid;

	OwnedArray<Controllable, CriticalSection> controllables;
	Array<WeakReference<ControllableContainer>  > controllableContainers;
	WeakReference<ControllableContainer> parentContainer;

	void addControllable(Controllable * p);
	void addParameter(Parameter * p);
	FloatParameter * addFloatParameter(const String &niceName, const String &description, const float &initialValue, const float &minValue = 0, const float &maxValue = 1, const bool &enabled = true);
	IntParameter * addIntParameter(const String &niceName, const String &description, const int &initialValue, const int &minValue, const int &maxValue, const bool &enabled = true);
	BoolParameter * addBoolParameter(const String &niceName, const String &description, const bool &value, const bool &enabled = true);
	StringParameter * addStringParameter(const String &niceName, const String &description, const String &value, const bool &enabled = true);
	EnumParameter * addEnumParameter(const String &niceName, const String &description, const bool &enabled = true);
	Point2DParameter * addPoint2DParameter(const String &niceName, const String &description, const bool &enabled = true);
	Point3DParameter * addPoint3DParameter(const String &niceName, const String &description, const bool &enabled = true);
	TargetParameter * addTargetParameter(const String &niceName, const String &description, WeakReference<ControllableContainer> rootReference = nullptr, const bool &enabled = true);
	Trigger * addTrigger(const String &niceName, const String &description, const bool &enabled = true);

	void addTriggerInternal(Trigger * t);
	void addParameterInternal(Parameter * p);


	void removeControllable(Controllable * c);
	Controllable * getControllableByName(const String &name, bool searchNiceNameToo = false);

	void addChildControllableContainer(ControllableContainer * container);
	void removeChildControllableContainer(ControllableContainer *container);
	

	ControllableContainer * getControllableContainerByName(const String &name, bool searchNiceNameToo = false);
	ControllableContainer * getControllableContainerForAddress(const String &address, bool recursive = false, bool getNotExposed = false);
	ControllableContainer * getControllableContainerForAddress(StringArray  addressSplit, bool recursive = false, bool getNotExposed = false);

	void setParentContainer(ControllableContainer * container);
	void updateChildrenControlAddress();


	virtual Array<WeakReference<Controllable>> getAllControllables(bool recursive = false, bool getNotExposed = false);
	virtual Array<WeakReference<Parameter>> getAllParameters(bool recursive = false, bool getNotExposed = false);
	virtual Controllable * getControllableForAddress(const String &address, bool recursive = true, bool getNotExposed = false);
	virtual Controllable * getControllableForAddress(StringArray addressSplit, bool recursive = true, bool getNotExposed = false);
	bool containsControllable(Controllable * c, int maxSearchLevels = -1);
	String getControlAddress(ControllableContainer * relativeTo = nullptr);

	void orderControllablesAlphabetically();

	virtual bool loadPresetWithName(const String &name);
	virtual bool loadPreset(PresetManager::Preset * preset);
	virtual PresetManager::Preset* saveNewPreset(const String &name);
	virtual bool saveCurrentPreset();
	virtual int getNumPresets();

	virtual bool resetFromPreset();

	//    to be overriden
	virtual void loadPresetInternal(PresetManager::Preset *) {};
	virtual void savePresetInternal(PresetManager::Preset *) {};

	virtual String getPresetFilter();
	virtual var getPresetValueFor(Parameter * p);//Any parameter that is part of a this preset can use this function

	void dispatchFeedback(Controllable * c);

	// Inherited via Parameter::Listener
	virtual void parameterValueChanged(Parameter * p) override;
	// Inherited via Trigger::Listener
	virtual void triggerTriggered(Trigger * p) override;

	virtual void askForRemoveControllable(Controllable * c) override;


	virtual var getJSONData();
	virtual void loadJSONData(var data, bool createIfNotThere = false);
	virtual void loadJSONDataInternal(var /*data*/) { /* to be overriden by child classes */ }

	virtual void childStructureChanged(ControllableContainer *)override;

	String getUniqueNameInContainer(const String &sourceName, int suffix = 0);

	//SCRIPT
	virtual DynamicObject * createScriptObject(DynamicObject * parent = nullptr) override;

	static var getChildFromScript(const var::NativeFunctionArgs &a);



private:
	// internal callback that a controllableContainer can override to react to any of it's parameter change
	//@ ben this is to avoid either:
	//      adding controllableContainerListener for each implementation
	//      or overriding parameterValueChanged and needing to call ControllableContainer::parameterValueChanged in implementation (it should stay independent as a different mechanism)
	//      or using dispatch feedback that triggers only exposedParams

	virtual void onContainerNiceNameChanged() {};
	virtual void onContainerShortNameChanged() {};
	virtual void onContainerParameterChanged(Parameter *) {};
	virtual void onExternalParameterChanged(Parameter *) {}; //When listening to other child controllable then this container's children
	virtual void onContainerTriggerTriggered(Trigger *) {};
	virtual void onExternalTriggerTriggered(Trigger *) {}; //When listening to other child controllable then this container's children

    virtual void onContainerParameterChangedAsync(Parameter *,const var & /*value*/){};
    
    // anti feedback when loading preset ( see loadPresetWithName)
    bool isLoadingPreset = false;

public:


    ListenerList<ControllableContainerListener> controllableContainerListeners;
    void addControllableContainerListener(ControllableContainerListener* newListener) { controllableContainerListeners.add(newListener);}
    void removeControllableContainerListener(ControllableContainerListener* listener) { controllableContainerListeners.remove(listener);}
    
	QueuedNotifier<ContainerAsyncEvent> queuedNotifier;
	typedef QueuedNotifier<ContainerAsyncEvent>::Listener ContainerAsyncListener;

	void addAsyncContainerListener(ContainerAsyncListener* newListener) { queuedNotifier.addListener(newListener); }
	void addAsyncCoalescedContainerListener(ContainerAsyncListener * newListener) {queuedNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncContainerListener(ContainerAsyncListener* listener) { queuedNotifier.removeListener(listener); }
	
	void clear();

	virtual InspectableEditor * getEditor(bool /*isRootEditor*/) override;

protected:
    void notifyStructureChanged();
    void newMessage(const Parameter::ParameterEvent &e)override;

    WeakReference<ControllableContainer>::Master masterReference;
    friend class WeakReference<ControllableContainer>;

    friend class PresetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableContainer)


};


#endif  // CONTROLLABLECONTAINER_H_INCLUDED
