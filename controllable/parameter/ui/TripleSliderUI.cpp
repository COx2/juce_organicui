/*
  ==============================================================================

	TripleSliderUI.cpp
	Created: 25 Oct 2016 11:46:46am
	Author:  bkupe

  ==============================================================================
*/

TripleSliderUI::TripleSliderUI(Point3DParameter* parameter) :
	ParameterUI(parameter),
	p3d(parameter),
	xParam("X", "xParam", parameter->x, parameter->minimumValue[0], parameter->maximumValue[0]),
	yParam("Y", "yParam", parameter->y, parameter->minimumValue[1], parameter->maximumValue[1]),
	zParam("Z", "zParam", parameter->z, parameter->minimumValue[2], parameter->maximumValue[2]),
	isUpdatingFromParam(false)
{

	showEditWindowOnDoubleClick = false;

	xParam.canHaveRange = parameter->canHaveRange;
	yParam.canHaveRange = parameter->canHaveRange;
	zParam.canHaveRange = parameter->canHaveRange;
	
	xParam.isCustomizableByUser = parameter->isCustomizableByUser;
	yParam.isCustomizableByUser = parameter->isCustomizableByUser;
	zParam.isCustomizableByUser = parameter->isCustomizableByUser;
	
	xParam.defaultValue = 0;
	yParam.defaultValue = 0;
	zParam.defaultValue = 0;
	
	xParam.defaultUI = parameter->defaultUI;
	yParam.defaultUI = parameter->defaultUI;
	zParam.defaultUI = parameter->defaultUI;


	xParam.addAsyncParameterListener(this);
	yParam.addAsyncParameterListener(this);
	zParam.addAsyncParameterListener(this);

	xSlider.reset((ParameterUI*)xParam.createDefaultUI());
	ySlider.reset((ParameterUI*)yParam.createDefaultUI());
	zSlider.reset((ParameterUI*)yParam.createDefaultUI());

	xSlider->showLabel = false;
	ySlider->showLabel = false;
	zSlider->showLabel = false;
	
	xSlider->showMenuOnRightClick = false;
	ySlider->showMenuOnRightClick = false;
	zSlider->showMenuOnRightClick = false;

	xSlider->setUndoableValueOnMouseUp = false;
	ySlider->setUndoableValueOnMouseUp = false;
	zSlider->setUndoableValueOnMouseUp = false;

	xSlider->addMouseListener(this, true);
	ySlider->addMouseListener(this, true);
	zSlider->addMouseListener(this, true);

	addAndMakeVisible(xSlider.get());
	addAndMakeVisible(ySlider.get());
	addAndMakeVisible(zSlider.get());

	setSize(200, GlobalSettings::getInstance()->fontSize->floatValue() + 4);//default size

	updateUIParams(); //force update
}

TripleSliderUI::~TripleSliderUI()
{
	xParam.removeAsyncParameterListener(this);
	yParam.removeAsyncParameterListener(this);
	zParam.removeAsyncParameterListener(this);
}

void TripleSliderUI::mouseDownInternal(const MouseEvent&)
{
	mouseDownValue = parameter->getValue();
}

void TripleSliderUI::mouseUpInternal(const MouseEvent&)
{
	if (setUndoableValueOnMouseUp)
	{
		if ((float)mouseDownValue[0] != xParam.floatValue()
			|| (float)mouseDownValue[1] != yParam.floatValue()
			|| (float)mouseDownValue[2] != zParam.floatValue())
		{

			p3d->setUndoableVector((float)mouseDownValue[0], (float)mouseDownValue[1], (float)mouseDownValue[2], xParam.floatValue(), yParam.floatValue(), zParam.floatValue());
		}
	}
}

void TripleSliderUI::paint(Graphics& g)
{
	ParameterUI::paint(g);

	if (showLabel)
	{
		Rectangle<int> r = getLocalBounds();
		g.setFont(jlimit(12, 40, jmin(r.getHeight(), r.getWidth()) - 16));
		r = r.removeFromLeft(jmin(g.getCurrentFont().getStringWidth(customLabel.isNotEmpty() ? customLabel : parameter->niceName) + 10, r.getWidth() - 60));
		g.setColour(useCustomTextColor ? customTextColor : TEXT_COLOR);
		g.drawFittedText(customLabel.isNotEmpty() ? customLabel : parameter->niceName, r, Justification::centred, 1);
	}
}

void TripleSliderUI::resized()
{
	juce::Rectangle<int> r = getLocalBounds();

	if (showLabel)
	{
		Font font(jlimit(12, 40, jmin(r.getHeight(), r.getWidth()) - 16));
		r.removeFromLeft(jmin(font.getStringWidth(customLabel.isNotEmpty() ? customLabel : parameter->niceName) + 10, r.getWidth() - 60));
		r.removeFromLeft(2);
	}

	xSlider->setBounds(r.removeFromLeft(r.getWidth() / 3 - 15));
	ySlider->setBounds(r.removeFromRight(r.getWidth() / 2 - 10));
	zSlider->setBounds(r.removeFromRight(r.getWidth() - 5));
}

void TripleSliderUI::showEditWindowInternal()
{
	AlertWindow nameWindow("Change point 2D params", "Set new values and bounds for this parameter", AlertWindow::AlertIconType::NoIcon, this);

	const String coordNames[3]{ "X","Y","Z" };

	for (int i = 0; i < 3; ++i) nameWindow.addTextEditor("val" + String(i), String((float)p3d->value[i]), "Value " + coordNames[i]);

	nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
	nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	int result = nameWindow.runModalLoop();

	if (result)
	{
		float newVals[3];
		for (int i = 0; i < 3; ++i) newVals[i] = nameWindow.getTextEditorContents("val" + String(i)).getFloatValue();
		p3d->setUndoableVector(p3d->x, p3d->y, p3d->z, newVals[0], newVals[1], newVals[2]);
	}
}

void TripleSliderUI::showEditRangeWindowInternal()
{
	if (!parameter->isCustomizableByUser) return;

	AlertWindow nameWindow("Change point 2D params", "Set new values and bounds for this parameter", AlertWindow::AlertIconType::NoIcon, this);

	const String coordNames[3]{ "X","Y","Z" };

	for (int i = 0; i < 3; ++i)
	{
		nameWindow.addTextEditor("minVal" + String(i), String((float)p3d->minimumValue[i]), "Minimum " + coordNames[i]);
		nameWindow.addTextEditor("maxVal" + String(i), String((float)p3d->maximumValue[i]), "Maximum " + coordNames[i]);
	}

	nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
	nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	int result = nameWindow.runModalLoop();

	if (result)
	{
		float newMins[3];
		float newMaxs[3];
		for (int i = 0; i < 3; ++i)
		{
			newMins[i] = nameWindow.getTextEditorContents("minVal" + String(i)).getFloatValue();
			newMaxs[i] = nameWindow.getTextEditorContents("maxVal" + String(i)).getFloatValue();
		}
		p3d->setBounds(newMins[0], newMins[1], newMins[2], jmax(newMins[0], newMaxs[0]), jmax(newMins[1], newMaxs[1]), jmax(newMins[2], newMaxs[2]));
	}
}


void TripleSliderUI::rangeChanged(Parameter* p)
{
	if (p != parameter) return;
	isUpdatingFromParam = true;
	xParam.setRange(parameter->minimumValue[0], parameter->maximumValue[0]);
	yParam.setRange(parameter->minimumValue[1], parameter->maximumValue[1]);
	zParam.setRange(parameter->minimumValue[2], parameter->maximumValue[2]);
	isUpdatingFromParam = false;
}

void TripleSliderUI::updateUIParamsInternal()
{
	xParam.setControllableFeedbackOnly(parameter->isControllableFeedbackOnly);
	yParam.setControllableFeedbackOnly(parameter->isControllableFeedbackOnly);
	zParam.setControllableFeedbackOnly(parameter->isControllableFeedbackOnly);
}

void TripleSliderUI::newMessage(const Parameter::ParameterEvent& e)
{
	ParameterUI::newMessage(e);

	if (e.parameter == parameter)
	{
		isUpdatingFromParam = true;
		xParam.setValue(((Point3DParameter*)e.parameter)->x);
		yParam.setValue(((Point3DParameter*)e.parameter)->y);
		zParam.setValue(((Point3DParameter*)e.parameter)->z);
		isUpdatingFromParam = false;
	}
	else if (isInteractable() && !isUpdatingFromParam)
	{
		if (e.parameter == &xParam || e.parameter == &yParam)
		{
			if (xParam.floatValue() != p3d->x || yParam.floatValue() != p3d->y || zParam.floatValue() != p3d->z)
			{
				if (!isMouseButtonDown(true) && !UndoMaster::getInstance()->isPerformingUndoRedo()) p3d->setUndoableVector(p3d->x, p3d->y, p3d->z, xParam.floatValue(), yParam.floatValue(), zParam.floatValue());
				else p3d->setVector(xParam.floatValue(), yParam.floatValue(), zParam.floatValue());
			}
		}
	}
}
