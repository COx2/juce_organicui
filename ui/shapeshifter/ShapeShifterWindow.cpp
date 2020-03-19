/*
  ==============================================================================

    ShapeShifterWindow.cpp
    Created: 2 May 2016 4:10:48pm
    Author:  bkupe

  ==============================================================================
*/

ShapeShifterWindow::ShapeShifterWindow(ShapeShifterPanel * _panel, juce::Rectangle<int> bounds) :
	ResizableWindow(_panel->currentContent->contentName, true),
	panel(_panel),
	checking(false),
	dragMode(PANEL)
{
	setTopLeftPosition(bounds.getTopLeft());
	_panel->setBounds(bounds);

	panel->setPreferredWidth(getWidth());
	panel->setPreferredHeight(getHeight());
    
    //DBG("window -> addShapeShifterListener " << panel->header.tabs[0]->content->contentName);
	panel->addShapeShifterPanelListener(this); //is it necessary ?

	setAlwaysOnTop(true);
	setContentNonOwned(_panel,true);

	setBackgroundColour(BG_COLOR.darker(.1f).withAlpha(.3f));

	setResizable(true, true);
	setDraggable(true);

	setVisible(true);
	toFront(true);


	addMouseListener(this,true);

}

ShapeShifterWindow::~ShapeShifterWindow()
{
	removeMouseListener(this);
	clear();

}

void ShapeShifterWindow::paintOverChildren(Graphics & g)
{
	g.setColour(BG_COLOR.brighter());
	g.drawRect(getLocalBounds());
}

void ShapeShifterWindow::resized()
{
	ResizableWindow::resized();

	if (panel == nullptr) return;

	panel->setPreferredWidth(getWidth());
	panel->setPreferredHeight(getHeight());
}

void ShapeShifterWindow::mouseDown(const MouseEvent & e)
{
	if (e.eventComponent == &panel->header || dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent) != nullptr)
	{
		dragMode = e.eventComponent == &panel->header ? PANEL : TAB;
		dragger.startDraggingComponent(this, e);
	}else
	{
		dragMode = NONE;
	}

}

void ShapeShifterWindow::mouseDrag(const MouseEvent & e)
{
	if (dragMode == NONE) return;
	panel->setTransparentBackground(true);
	ShapeShifterManager::getInstance()->checkCandidateTargetForPanel(panel);
	dragger.dragComponent(this, e, 0);
}

void ShapeShifterWindow::mouseUp(const MouseEvent &)
{
	panel->setTransparentBackground(false);
    
    checking = true;
	bool found = ShapeShifterManager::getInstance()->checkDropOnCandidateTarget(panel);
    checking = false;

    if(found)
    {
		clear();
        ShapeShifterManager::getInstance()->closePanelWindow(this, false);   
    }
}


void ShapeShifterWindow::clear()
{
	if (panel != nullptr)
	{
		panel->removeShapeShifterPanelListener(this);
		panel = nullptr;
	}
}

void ShapeShifterWindow::userTriedToCloseWindow()
{
	ShapeShifterManager::getInstance()->closePanelWindow(this,true);
}

var ShapeShifterWindow::getCurrentLayout()
{
	var data(new DynamicObject());
	data.getDynamicObject()->setProperty("panel", panel->getCurrentLayout());
	data.getDynamicObject()->setProperty("x", getBounds().getPosition().x);
	data.getDynamicObject()->setProperty("y", getBounds().getPosition().y);
	data.getDynamicObject()->setProperty("width", getWidth());
	data.getDynamicObject()->setProperty("height", getHeight());
	return data;
}

void ShapeShifterWindow::panelEmptied(ShapeShifterPanel *)
{
	if(!checking) ShapeShifterManager::getInstance()->closePanelWindow(this, true);
}
