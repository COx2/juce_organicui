/*
  ==============================================================================

    AssetManager.h
    Created: 23 Oct 2016 2:22:04pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef ASSETMANAGER_H_INCLUDED
#define ASSETMANAGER_H_INCLUDED


class AssetManager
{
public:
	juce_DeclareSingleton(AssetManager, true);
	AssetManager();
	virtual ~AssetManager();

	ImageButton * getRemoveBT();
	ImageButton * getConfigBT();
	ImageButton * getTargetBT();
	ImageButton * getPowerBT();  
	ImageButton * getAddBT();
	ImageButton * getFileBT();
	ImageButton * getEditBT();
	ImageButton * getRightArrowBT();
	ImageButton * getDownArrowImageBT();

	Image getInImage();
	Image getOutImage();
	Image getReloadImage();
	Image getPlayImage();
	Image getStopImage();
	Image getPrevCueImage();
	Image getNextCueImage();
	Image getTriggerImage();

	ImageButton * getSetupBTImage(const Image & image);
	ImageButton * getToggleBTImage(const Image &image);
};



#endif  // ASSETMANAGER_H_INCLUDED
