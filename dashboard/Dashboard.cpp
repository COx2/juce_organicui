/*
  ==============================================================================

	Dashboard.cpp
	Created: 19 Apr 2017 10:57:04pm
	Author:  Ben

  ==============================================================================
*/


Dashboard::Dashboard() :
	BaseItem("Dashboard", false)
{
	addChildControllableContainer(&itemManager);
}

Dashboard::~Dashboard()
{
}

var Dashboard::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("itemManager", itemManager.getJSONData());
	return data;
}

void Dashboard::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	itemManager.loadJSONData(data.getProperty("itemManager", var()));
}
