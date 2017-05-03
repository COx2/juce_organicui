/*
  ==============================================================================

    ScriptEditor.h
    Created: 20 Feb 2017 5:04:20pm
    Author:  Ben

  ==============================================================================
*/

#ifndef SCRIPTEDITOR_H_INCLUDED
#define SCRIPTEDITOR_H_INCLUDED



class ScriptEditor :
	public BaseItemEditor,
	public Script::AsyncListener
{
public:
	ScriptEditor(Script * script, bool isRoot);
	~ScriptEditor();

	Script * script;
	bool editMode;

	ScopedPointer<ImageButton> fileBT;
	ScopedPointer<TriggerImageUI> reloadBT;
	ScopedPointer<ImageButton> editBT;
	ScopedPointer<BoolToggleUI> logUI;

	ScopedPointer<InspectableEditor> paramsEditor;

	Rectangle<int> statusBounds;

	void paint(Graphics &g) override;
	void resizedInternalHeader(Rectangle<int> &r) override;
	void resizedInternalContent(Rectangle<int> &r) override;

	void newMessage(const Script::ScriptEvent &e) override;

	void buttonClicked(Button * b) override;
};



#endif  // SCRIPTEDITOR_H_INCLUDED
