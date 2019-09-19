/*
  ==============================================================================

    Dashboard.h
    Created: 19 Apr 2017 10:57:04pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class Dashboard :
	public BaseItem
{
public:
	Dashboard();
	~Dashboard();

	DashboardItemManager itemManager;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

};