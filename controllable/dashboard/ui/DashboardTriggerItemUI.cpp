#include "DashboardTriggerItemUI.h"

DashboardTriggerItemUI::DashboardTriggerItemUI(DashboardTriggerItem* triggerItem) :
	DashboardControllableItemUI(triggerItem),
	triggerItem(triggerItem)
{
	rebuildUI();
	if (itemUI != nullptr) setSize(50,50);
}


DashboardTriggerItemUI::~DashboardTriggerItemUI()
{
}

ControllableUI* DashboardTriggerItemUI::createControllableUI()
{
	File f = triggerItem->customImagePath->getFile();
	if (f.existsAsFile())
	{
		Image img = ImageCache::getFromFile(f);
		return triggerItem->trigger->createImageUI(img);
	}

	return DashboardControllableItemUI::createControllableUI();
}

void DashboardTriggerItemUI::controllableFeedbackUpdateInternal(Controllable* c)
{
	if (c == triggerItem->customImagePath) rebuildUI();
}
