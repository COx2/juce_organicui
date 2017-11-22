#include "InspectableContentComponent.h"
/*
  ==============================================================================

    InspectableContentComponent.cpp
    Created: 9 May 2016 6:51:16pm
    Author:  bkupe

  ==============================================================================
*/

InspectableContentComponent::InspectableContentComponent(Inspectable * inspectable) :
	InspectableContent(inspectable),
repaintOnSelectionChanged(true),
rounderCornerSize(4),
	autoDrawHighlightWhenSelected(true),
	highlightColor(HIGHLIGHT_COLOR),
autoSelectWithChildRespect(true),
bringToFrontOnSelect(true)
{
}

InspectableContentComponent::~InspectableContentComponent()
{
}


void InspectableContentComponent::mouseEnter(const MouseEvent & e)
{
	HelpBox::getInstance()->setOverData(inspectable->helpID);
}

void InspectableContentComponent::mouseExit(const MouseEvent & e)
{
	HelpBox::getInstance()->clearOverData(inspectable->helpID);
}

void InspectableContentComponent::mouseDown(const MouseEvent & e)
{
	if (inspectable.wasObjectDeleted())
	{
		DBG("Object deleted on inspectable content, should never be there !");
		return;
	}

	if (!inspectable->isSelectable) return;

	if (autoSelectWithChildRespect)
	{
		Component * c = e.eventComponent;

		bool foundAChildComponent = false;
		while (c != this)
		{
			InspectableContentComponent * ie = dynamic_cast<InspectableContentComponent *>(c);
			if (ie != nullptr)
			{
				foundAChildComponent = true;
				break;
			}
			c = c->getParentComponent();
		}

		if (!foundAChildComponent)
		{
			inspectable->selectThis();
		}
	}
}

void InspectableContentComponent::paintOverChildren(Graphics & g)
{
	if (inspectable.wasObjectDeleted()) return;
	if (autoDrawHighlightWhenSelected && inspectable->isSelected)
	{
		g.setColour(highlightColor);
		g.drawRoundedRectangle(getLocalBounds().toFloat(), rounderCornerSize, 2);
	}
}

void InspectableContentComponent::inspectableSelectionChanged(Inspectable *)
{
	if (bringToFrontOnSelect) toFront(true);
	if (repaintOnSelectionChanged) repaint();
}
