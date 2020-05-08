#pragma once

#include "Utils/CGameSingleton.h"
#include "CViewLayer.h"

class CViewManager : public CGameSingleton<CViewManager>
{
protected:
	std::vector<CViewLayer*> m_viewLayers;

public:
	CViewManager();

	virtual ~CViewManager();

	void initViewLayer(int numLayer);

	void releaseAllLayer();

	inline int getLayerCount()
	{
		return (int)m_viewLayers.size();
	}

	inline CViewLayer* getLayer(int id)
	{
		return m_viewLayers[id];
	}

	void update();

	void render();

	void postRender();

	void onResume();

	void onPause();

	bool onBack();
};