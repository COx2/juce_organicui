/*  ==============================================================================

	AutomationUI.cpp
	Created: 11 Dec 2016 1:22:02pm
	Author:  Ben

  ==============================================================================
*/


AutomationUI::AutomationUI(Automation * _automation, Colour c) :
	BaseManagerUI("Automation", _automation, false),
	firstROIKey(0),
	lastROIKey(0),
	autoResetViewRangeOnLengthUpdate(false),
	currentPosition(0),
	color(c),
	currentUI(nullptr),
	transformer(nullptr)
{
	manager->selectionManager->addSelectionListener(this);

	setShowAddButton(false);
	animateItemOnAdd = false;

	transparentBG = true;
	setViewRange(0, manager->length->floatValue());
	manager->addAsyncContainerListener(this);

	resizeOnChildBoundsChanged = false;
	addExistingItems();

	noItemText = "Add keys by double-clicking here";

	setSize(100, 100);

	updateROI();
}

AutomationUI::~AutomationUI()
{
	if (!inspectable.wasObjectDeleted())
	{
		manager->selectionManager->removeSelectionListener(this);
		manager->removeAsyncContainerListener(this);
	}

}

void AutomationUI::setCurrentPosition(const float &pos)
{
	currentPosition = pos;
	currentUI = getClosestKeyUIForPos(currentPosition);

}

void AutomationUI::setCurrentValue(const float &val)
{
	currentValue = val;
	repaint(); //to specify ?d
}

void AutomationUI::setViewRange(float start, float end)
{
	viewStartPos = start;
	viewEndPos = end;
	updateROI();
	resized();
}

void AutomationUI::updateROI()
{
	if (itemsUI.size() == 0) return;

	int len = itemsUI.size() - 1;
	for (int i = 0; i < len; i++)
	{
		itemsUI[i]->setVisible(false);
		if (isInView(itemsUI[i]))
		{
			firstROIKey = jmax<int>(i - 1, 0);
			break;
		}
	}

	for (int i = len; i >= 0; i--)
	{
		itemsUI[i]->setVisible(false);
		if (isInView(itemsUI[i]))
		{
			lastROIKey = jmin<int>(i + 1, len);
			break;
		}
	}

	for (int i = firstROIKey; i <= lastROIKey; i++)
	{
		itemsUI[i]->setVisible(true);
	}
	resized();
}

void AutomationUI::paint(Graphics & g)
{
	BaseManagerUI::paint(g);


	//int count = 0;
	if (itemsUI.size() >= 2)
	{
		
		int ty = getYForValue(currentValue);
		Rectangle<int> vr = getLocalBounds().withTop(ty);
		g.setColour(color.withAlpha(.1f));
		if(vr.getHeight() > 0) g.fillRect(vr);
		

		//pos-value feedback
		g.setColour(Colours::orange);
		g.drawEllipse(Rectangle<int>(0, 0, 3, 3).withCentre(Point<int>(getXForPos(currentPosition), ty)).toFloat(), 1);
	}

	//recorder
	if (manager->recorder != nullptr)
	{
		if (manager->recorder->isRecording->boolValue())
		{
			int numRKeys = manager->recorder->keys.size();
			if (numRKeys > 0)
			{
				g.setColour(Colours::red.withAlpha(.3f));
				g.fillRect(getLocalBounds().withLeft(getXForPos(manager->recorder->keys[0].x)).withRight(getXForPos(currentPosition)));

				if (numRKeys >= 2)
				{
					Path p;
					Point<float> k = manager->recorder->keys[0];
					p.startNewSubPath(getXForPos(k.x), getYForValue(k.x));
					for (int i = 1; i < numRKeys; i++)
					{
						k = manager->recorder->keys[i];
						p.lineTo(getXForPos(k.x), getYForValue(k.y));
					}
					//p.closeSubPath();
					g.setColour(Colours::orangered);
					g.strokePath(p, PathStrokeType(2));
				}
			}
		}
	}
}


void AutomationUI::resized()
{

	MessageManagerLock mm;

	if (getParentComponent() == nullptr) return;
	if (getWidth() == 0 || getHeight() == 0) return;
	if (itemsUI.size() == 0) return;

	for (int i = lastROIKey; i >= firstROIKey; i--)
	{
		placeKeyUI(itemsUI[i], true);
		itemsUI[i]->toBack(); // place each ui in front of its right
	}

	if (transformer != nullptr) transformer->updateBoundsFromKeys();

	repaint(); //overkill? needed to have proper value feedback when creating ui and resizing for the first time
}

void AutomationUI::placeKeyUI(AutomationKeyUI * kui, bool placePrevKUI)
{

	int index = itemsUI.indexOf(kui);
	if (kui == nullptr) return;

	int tx = getXForPos(kui->item->position->floatValue());
	int ty = getYForValue(kui->item->value->floatValue());
	Rectangle<int> kr;

	if (index < itemsUI.size() - 1)
	{
		AutomationKeyUI * nextKey = itemsUI[index + 1];
		int tx2 = getXForPos(nextKey->item->position->floatValue());
		int ty2 = getYForValue(nextKey->item->value->floatValue());

		//Rectangle<int> kr2 = Rectangle<int>(0, 0, AutomationKeyUI::handleClickZone, AutomationKeyUI::handleClickZone).withCentre(Point<int>(tx2, ty2));
		kr = Rectangle<int>(tx, 0, tx2 - tx, getHeight()).expanded(AutomationKeyUI::handleClickZone / 2, 0);
		kui->setKeyPositions(ty, ty2);
	} else
	{
		kr = Rectangle<int>(0, 0, AutomationKeyUI::handleClickZone, getHeight()).withPosition(tx - AutomationKeyUI::handleClickZone / 2, 0);
		kui->setKeyPositions(ty, 0);
	}

	kui->setBounds(kr);

	if (placePrevKUI && index > 0)
	{
		placeKeyUI(itemsUI[index - 1], false);
	}
}

int AutomationUI::getXForPos(float time)
{
	if (viewStartPos == viewEndPos) return 0;
	return (int)jmap<float>(time, viewStartPos, viewEndPos, 0, (float)getWidth());
}

float AutomationUI::getPosForX(int tx, bool offsetStart)
{
	float viewRange = viewEndPos - viewStartPos;
	float mapStart = offsetStart ? viewStartPos : 0;
	return jmap<float>((float)tx, 0, (float)getWidth(), mapStart, mapStart + viewRange);
}

int AutomationUI::getYForValue(float value)
{
	return (int)((1 - value)*getHeight());
}

float AutomationUI::getValueForY(int ty)
{
	return (1 - ty * 1.f / getHeight());
}

bool AutomationUI::isInView(AutomationKeyUI * kui)
{
	return kui->item->position->floatValue() >= viewStartPos && kui->item->position->floatValue() <= viewEndPos;
}

AutomationKeyUI * AutomationUI::getClosestKeyUIForPos(float pos, int start, int end)
{
	if (itemsUI.size() == 0) return nullptr;

	if (start == -1) start = 0;
	if (end == -1) end = itemsUI.size() - 1;


	if (pos < itemsUI[0]->item->position->floatValue()) return itemsUI[0];
	if (pos > itemsUI[itemsUI.size() - 1]->item->position->floatValue()) return itemsUI[itemsUI.size() - 1];


	if (end - start <= 1) return itemsUI[start];

	int midIndex = (int)floor((start + end) / 2);
	float medPos = itemsUI[midIndex]->item->position->floatValue();

	if (pos == medPos) return itemsUI[midIndex];

	else if (pos > medPos)
	{
		return getClosestKeyUIForPos(pos, midIndex, end);
	} else
	{
		return getClosestKeyUIForPos(pos, start, midIndex);
	}
}

void AutomationUI::itemAddedAsync(AutomationKey * k)
{
	BaseManagerUI::itemAddedAsync(k);
	updateROI();

}

void AutomationUI::itemsReorderedAsync()
{
	BaseManagerUI::itemsReorderedAsync();
	updateROI();
}

AutomationKeyUI * AutomationUI::createUIForItem(AutomationKey * item)
{
	return new AutomationKeyUI(item, color);
}

void AutomationUI::addItemUIInternal(AutomationKeyUI * kui)
{
	kui->handle.addMouseListener(this, false);
}

void AutomationUI::removeItemUIInternal(AutomationKeyUI * kui)
{
	if (transformer != nullptr)
	{
		removeChildComponent(transformer);
		transformer = nullptr;
	}

	kui->handle.removeMouseListener(this);
	updateROI();
}

void AutomationUI::mouseDown(const MouseEvent & e)
{
	BaseManagerUI::mouseDown(e);

	if (e.eventComponent == this)
	{
		if (e.mods.isLeftButtonDown() && e.mods.isAltDown())
		{
			manager->addItem(getPosForX(e.getPosition().x), getValueForY(e.getPosition().y));
		} else
		{
			Array<Component *> selectables;
			Array<Inspectable *> inspectables;
			for (auto &i : itemsUI) if (i->isVisible())
			{
				selectables.add(&i->handle);
				inspectables.add(i->inspectable);
			}

			if (transformer != nullptr)
			{
				removeChildComponent(transformer);
				transformer = nullptr;
			}

			if (InspectableSelector::getInstance()) InspectableSelector::getInstance()->startSelection(this, selectables, inspectables, manager->selectionManager, !e.mods.isCommandDown() && !e.mods.isShiftDown());
		}
	} else
	{
		if (e.mods.isShiftDown())
		{
			AutomationKeyUI::Handle * kHandle = dynamic_cast<AutomationKeyUI::Handle *>(e.eventComponent);
			if (kHandle != nullptr)
			{
				if (manager->selectionManager->currentInspectables.size() > 0)
				{
					AutomationKey * lastSelectedKey = (AutomationKey *)manager->selectionManager->currentInspectables[manager->selectionManager->currentInspectables.size() - 1];
					AutomationKey * sKey = ((AutomationKeyUI *)kHandle->getParentComponent())->item;

					int i1 = manager->items.indexOf(lastSelectedKey);
					int i2 = manager->items.indexOf(sKey);

					int index1 = jmin(i1, i2) + 1;
					int index2 = jmax(i1, i2) - 1;

					for (int i = index1; i <= index2; i++)
					{
						manager->items[i]->selectThis(true);
					}
				}
			}
		}

	}

}

void AutomationUI::mouseDoubleClick(const MouseEvent & e)
{
	if (e.eventComponent == this) manager->addItem(getPosForX(e.getPosition().x), getValueForY(e.getPosition().y));
}

void AutomationUI::mouseDrag(const MouseEvent & e)
{
	if (e.originalComponent == this)
	{

	} else
	{
		AutomationKeyUI::Handle * h = dynamic_cast<AutomationKeyUI::Handle *>(e.eventComponent);

		if (h != nullptr)
		{
			AutomationKeyUI * kui = static_cast<AutomationKeyUI *>(h->getParentComponent());
			if (e.mods.isLeftButtonDown())
			{
				Point<int> mp = e.getEventRelativeTo(this).getPosition();
				float pos = getPosForX(mp.x);
				float val = getValueForY(mp.y);

				MouseInputSource source = Desktop::getInstance().getMainMouseSource();

				if (e.mods.isShiftDown())
				{
					float initX = getXForPos(kui->posAtMouseDown);
					float initY = getYForValue(kui->valueAtMouseDown);

					if (fabsf(mp.x - initX) > fabsf(mp.y - initY))
					{
						kui->handle.setMouseCursor(MouseCursor::LeftRightResizeCursor);
						val = kui->valueAtMouseDown;
					} else
					{
						kui->handle.setMouseCursor(MouseCursor::UpDownResizeCursor);
						pos = kui->posAtMouseDown;
					}
				} else
				{
					kui->handle.setMouseCursor(MouseCursor::NormalCursor);
				}

				kui->item->position->setValue(pos);
				kui->item->value->setValue(val);

			}
		}
	}
}

void AutomationUI::mouseUp(const MouseEvent & e)
{
	if (e.originalComponent == this)
	{

	} else
	{
		AutomationKeyUI::Handle * h = dynamic_cast<AutomationKeyUI::Handle *>(e.eventComponent);
		if (h != nullptr)
		{
			AutomationKeyUI * kui = static_cast<AutomationKeyUI *>(h->getParentComponent());
			if (e.mods.isLeftButtonDown())
			{

				Array<UndoableAction *> actions;
				actions.add(kui->item->position->setUndoableValue(kui->posAtMouseDown, kui->item->position->floatValue(), true));
				actions.add(kui->item->value->setUndoableValue(kui->valueAtMouseDown, kui->item->value->floatValue(), true));
				UndoMaster::getInstance()->performActions("Move automation key", actions);

			}
		}
	}
}

bool AutomationUI::keyPressed(const KeyPress & e)
{
	BaseManagerUI::keyPressed(e);
	return false;
}

void AutomationUI::newMessage(const ContainerAsyncEvent & e)
{
	if (e.type == ContainerAsyncEvent::EventType::ControllableFeedbackUpdate)
	{
		if (e.targetControllable == manager->position)
		{
			setCurrentPosition(manager->position->floatValue());
			repaint();
		} else if (e.targetControllable == manager->value)
		{
			setCurrentValue(manager->value->floatValue());
			repaint();
		} else if (e.targetControllable == manager->length)
		{
			if (autoResetViewRangeOnLengthUpdate) setViewRange(0, manager->length->floatValue());
		}else if (e.targetControllable != nullptr)
		{
			AutomationKey * k = dynamic_cast<AutomationKey *>(e.targetControllable->parentContainer);
			if (k != nullptr)
			{
				if (e.targetControllable == k->easingType)
				{
					//repaint();
				} else if (e.targetControllable == k->position || e.targetControllable == k->value)
				{
					placeKeyUI(getUIForItem(k));
					//repaint();
				}
			}
		}

	}

}

void AutomationUI::inspectablesSelectionChanged()
{
	if (transformer != nullptr)
	{
		removeChildComponent(transformer);
		transformer = nullptr;
	}

	Array<AutomationKeyUI *> uiSelection;
	if (manager->selectionManager->currentInspectables.size() >= 2)
	{

	}
	for (auto &i : manager->selectionManager->currentInspectables)
	{
		AutomationKey * k = static_cast<AutomationKey *>(i);
		if (k == nullptr) continue;
		AutomationKeyUI * kui = getUIForItem(k);
		if (kui == nullptr) return;

		uiSelection.add(kui);
	}

	if (uiSelection.size() >= 2)
	{
		transformer = new AutomationMultiKeyTransformer(this, uiSelection);
		addAndMakeVisible(transformer);
		transformer->grabKeyboardFocus(); // so no specific key has the focus for deleting
	}
}