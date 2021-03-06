#pragma once

#include "IApplicationEventReceiver.h"

class SampleLuckyDraw :
	public IApplicationEventReceiver,
	public IScrollerCallback
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;

	CGlyphFont *m_largeFont;
	CGlyphFont *m_smallFont;

	CGUIImage *m_backgroundImage;

	CSpriteAtlas *m_sprite;

	std::vector<CScroller*> m_scrollers;
	CScrollerController *m_controller;

	CButton* m_spin;
	CButton* m_stop;
	CButton* m_quit;

	int m_state;
public:
	SampleLuckyDraw();
	virtual ~SampleLuckyDraw();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();

protected:

	void onSpinClick();

	void onStopClick();

public:

	virtual CGUIElement* createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID);
};