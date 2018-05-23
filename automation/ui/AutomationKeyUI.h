/*
  ==============================================================================

    AutomationKeyUI.h
    Created: 11 Dec 2016 1:22:27pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class AutomationKeyUI :
	public BaseItemMinimalUI<AutomationKey>
{
public:
	AutomationKeyUI(AutomationKey *, Colour c = Colours::white);
	virtual ~AutomationKeyUI();
    
	Colour color;

    int keyYPos1;
    int keyYPos2;

	ScopedPointer<EasingUI> easingUI;

	const static int handleSize = 6;
	const static int handleClickZone = 10;


	float posAtMouseDown;
	float valueAtMouseDown;


	class Handle :
		public Component
	{
	public:
		Handle(Colour c);
		bool highlight;
		Colour color;
		void paint(Graphics &g) override;
	};

    Handle handle;
    

	void paint(Graphics &) override {}; //avoid default item painting

	void setEasingUI(EasingUI * eui);

	void setKeyPositions(const int &k1, const int &k2);

	void showKeyEditorWindow();
	
	void resized() override;

	bool hitTest(int tx, int ty) override;

	void mouseDown(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;

	void controllableFeedbackUpdateInternal(Controllable * c) override;

	void inspectableSelectionChanged(Inspectable *) override;
	void inspectablePreselectionChanged(Inspectable *) override;
};
