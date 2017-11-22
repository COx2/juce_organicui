/*
  ==============================================================================

    Inspector.h
    Created: 9 May 2016 6:41:38pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INSPECTOR_H_INCLUDED
#define INSPECTOR_H_INCLUDED

#include "../../ui/shapeshifter/ShapeShifterContent.h"
#include "InspectableEditor.h"
#include "../InspectableSelectionManager.h"

class Inspector :
	public Component,
	public Inspectable::InspectableListener,
	public InspectableSelectionManager::Listener
{
public:
	Inspector(InspectableSelectionManager * selectionManager = nullptr);
	virtual ~Inspector();

	InspectableSelectionManager * selectionManager;
	WeakReference<Inspectable> currentInspectable;
	Viewport vp;
	ScopedPointer<InspectableEditor> currentEditor;

	void setSelectionManager(InspectableSelectionManager * newSM);
	void resized() override;
	void setCurrentInspectable(WeakReference<Inspectable> inspectable);
	void clear();

    void inspectableDestroyed(Inspectable * inspectable) override;

	//From InspectableSelectionManager
	void inspectablesSelectionChanged() override;

	class  InspectorListener
	{
	public:
		virtual ~InspectorListener() {}
		virtual void currentInspectableChanged(Inspector *) {};
	};

	ListenerList<InspectorListener> listeners;
	void addInspectorListener(InspectorListener* newListener) { listeners.add(newListener); }
	void removeInspectorListener(InspectorListener* listener) { listeners.remove(listener); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Inspector)
};


class InspectorUI :
	public ShapeShifterContentComponent
{
public:
	InspectorUI(const String &name, InspectableSelectionManager * selectionManager = nullptr);
	~InspectorUI();

	Inspector inspector;

	void resized() override;

	static InspectorUI * create(const String &contentName) { return new InspectorUI(contentName); }
};

#endif  // INSPECTOR_H_INCLUDED
