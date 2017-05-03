/*
  ==============================================================================

    DashboardView.h
    Created: 19 Apr 2017 10:57:12pm
    Author:  Ben

  ==============================================================================
*/

#ifndef DASHBOARDVIEW_H_INCLUDED
#define DASHBOARDVIEW_H_INCLUDED

class DashboardView :
	public BaseManagerViewUI<DashboardPanelManager,DashboardPanel,DashboardPanelUI>
{
public:
	DashboardView(DashboardPanelManager * manager);
	~DashboardView();
};



#endif  // DASHBOARDVIEW_H_INCLUDED
