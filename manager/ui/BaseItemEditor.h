/*
  ==============================================================================

    BaseItemEditor.h
    Created: 18 Jan 2017 2:23:31pm
    Author:  Ben

  ==============================================================================
*/

#ifndef BASEITEMEDITOR_H_INCLUDED
#define BASEITEMEDITOR_H_INCLUDED


class BaseItemEditor :
	public InspectableEditor,
	public ControllableContainer::ContainerAsyncListener,
	public ButtonListener
{
public:
	BaseItemEditor(BaseItem * bi, bool isRoot);
	~BaseItemEditor();

	BaseItem * item;

	ScopedPointer<BoolImageToggleUI> enabledUI;
	ScopedPointer<StringParameterUI> nameUI;
	ScopedPointer<ImageButton> removeBT;

	ScopedPointer<InspectableEditor> scriptManagerUI;
	ScopedPointer<InspectableEditor> customParamsUI;

	int headerHeight;
	bool transparentBG;
	bool paintHeaderOnly;

	virtual void paint(Graphics &g) override;
	virtual void resized() override;
	virtual void resizedInternalHeader(Rectangle<int> &) {}
	virtual void resizedInternalContent(Rectangle<int> &) {}
	virtual void resizedInternalFooter(Rectangle<int> &) {}

	void newMessage(const ContainerAsyncEvent &e) override;
	virtual void controllableFeedbackAsyncUpdate(Controllable *);

	virtual void controllableAddedAsync(Controllable *) {}
	virtual void controllableRemovedAsync(Controllable *) {}

	virtual void childBoundsChanged(Component *) override;

	virtual void buttonClicked(Button *b) override;

};




#endif  // BASEITEMEDITOR_H_INCLUDED
