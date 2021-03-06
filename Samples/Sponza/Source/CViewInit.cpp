#include "pch.h"

#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "GridPlane/CGridPlane.h"
#include "SkyDome/CSkyDome.h"

#include "Lightmapper/Components/Probe/CLightProbe.h"
#include "Lightmapper/Components/Probe/CLightProbeRender.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_spriteArchive(NULL),
	m_downloaded(0)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char *name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	getApplication()->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager *shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");

	shaderMgr->loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SH.xml");

#if defined(USE_FREETYPE)
	CGlyphFreetype *freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");
#endif
}

void CViewInit::initScene()
{
	CBaseApp *app = getApplication();

	CScene *scene = CContext::getInstance()->initScene();
	CZone *zone = scene->createZone();

	// camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	CCamera *camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(2.0f, 1.0f, 2.0f));
	camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject *guiCameraObj = zone->createEmptyObject();
	guiCameraObj->addComponent<CCamera>();
	CCamera *guiCamera = guiCameraObj->getComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture *skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome *skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(-2.0f, -7.0f, -1.5f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	core::vector3df pointLightPosition[] = {
		{-5.595442f, 1.2f, 2.00912f},
		{-5.6f, 1.2f, -2.25},
		{6.018463f, 1.2f, 2.0211f},
		{6.007851f, 1.2f, -2.237712f},

		{3.09f, 0.8f, -1.10477f},
		{3.09f, 0.8f, 0.71455f},
		{-2.4447f, 0.8f, -1.0884f},
		{-2.4447f, 0.8f,  0.7141f},
	};

	for (int i = 0; i < 8; i++)
	{
		CGameObject *pointLightObj = zone->createEmptyObject();

		CPointLight *pointLight = pointLightObj->addComponent<CPointLight>();
		pointLight->setShadow(true);

		if (i >= 4)
			pointLight->setRadius(1.5f);
		else
			pointLight->setRadius(3.0f);

		CTransformEuler *pointLightTransform = pointLightObj->getTransformEuler();
		pointLightTransform->setPosition(pointLightPosition[i]);
	}

	// sponza
	CMeshManager *meshManager = CMeshManager::getInstance();
	CEntityPrefab *prefab = NULL;

	std::vector<std::string> textureFolders;
	textureFolders.push_back("Sponza/Textures");

	// load model
	prefab = meshManager->loadModel("Sponza/Sponza.dae", NULL, true);
	if (prefab != NULL)
	{
		// load material
		ArrayMaterial& materials = CMaterialManager::getInstance()->loadMaterial("Sponza/Sponza.xml", true, textureFolders);

		// create render mesh object
		CGameObject *sponza = zone->createEmptyObject();
		sponza->setStatic(true);

		// renderer
		CRenderMesh *renderer = sponza->addComponent<CRenderMesh>();
		renderer->initFromPrefab(prefab);
		renderer->initMaterial(materials);

		// indirect indirect lighting
		// CIndirectLighting *indirectLighting = sponza->addComponent<CIndirectLighting>();
		// indirectLighting->setIndirectLightingType(CIndirectLighting::VertexColor);
	}

#if defined(USE_FREETYPE)
	CGlyphFont *fontLarge = new CGlyphFont();
	fontLarge->setFont("Segoe UI Light", 50);

	CGlyphFont *fontSmall = new CGlyphFont();
	fontSmall->setFont("Segoe UI Light", 34);

	CGlyphFont *fontTiny = new CGlyphFont();
	fontTiny->setFont("Segoe UI Light", 16);

	// gui
	bool is3DGUI = true;

	CGameObject *guiObject = zone->createEmptyObject();
	CCanvas *canvas = guiObject->addComponent<CCanvas>();

	// Scale screen resolution to meter and flip 2D coord (Y down, X invert)
	CGUIElement *rootGUI = canvas->getRootElement();

	if (is3DGUI == true)
	{
		// 3D GUI transform
		rootGUI->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));
		rootGUI->setScale(core::vector3df(-0.002f, -0.002f, 0.002f));

		// Billboard or not?
		canvas->enable3DBillboard(true);
	}

	CGUIText *textLarge = canvas->createText(fontLarge);
	textLarge->setText("Skylicht Engine");
	textLarge->setPosition(core::vector3df(0.0f, 40.0f, 0.0f));

	CGUIText *textSmall = canvas->createText(fontSmall);
	textSmall->setText("This is demo for render of Truetype font");
	textSmall->setPosition(core::vector3df(0.0f, 100.0f, 0.0f));
#endif

	if (m_sprite != NULL)
	{
		SFrame* f = m_sprite->getFrame("icon_gunfire_sr_semi_auto.png");
		if (f != NULL)
		{
			CGUISprite *spriteGUI = canvas->createSprite(f);
			spriteGUI->setPosition(core::vector3df(0.0f, 150.0f, 0.0f));

			// test mask on sprite
			//CGUIMask *mask = canvas->createMask(core::rectf(20.0f, 20.0f, 400.0f, 200.0f));
			//spriteGUI->setMask(mask);
		}
	}

	// test mask on all gui
	// CGUIMask *mask = canvas->createMask(core::rectf(20.0f, 20.0f, 400.0f, 200.0f));
	// canvas->getRootElement()->setMask(mask);

	// save to context
	CContext *context = CContext::getInstance();
	context->initRenderPipeline(app->getWidth(), app->getHeight());
	context->setActiveZone(zone);
	context->setActiveCamera(camera);

	if (is3DGUI == true)
		context->setGUICamera(camera);
	else
		context->setGUICamera(guiCamera);

	context->setDirectionalLight(directionalLight);

	initProbes();
}

void CViewInit::initProbes()
{
	CContext *context = CContext::getInstance();
	CZone *zone = context->getActiveZone();

	std::vector<core::vector3df> probesPosition;

	for (int i = 0; i < 7; i++)
	{
		float x = i * 2.8f - 3.0f * 2.8f;

		// row 0
		probesPosition.push_back(core::vector3df(x, 1.0f, -0.2f));

		probesPosition.push_back(core::vector3df(x, 1.0f, 2.0f));

		probesPosition.push_back(core::vector3df(x, 1.0f, -2.3f));

		// row 1

		probesPosition.push_back(core::vector3df(x, 3.5f, -0.2f));

		probesPosition.push_back(core::vector3df(x, 3.5f, 2.0f));

		probesPosition.push_back(core::vector3df(x, 3.5f, -2.3f));

		// row 2

		probesPosition.push_back(core::vector3df(x, 8.0f, -0.2f));
	}

	std::vector<CLightProbe*> probes;

	for (u32 i = 0, n = (int)probesPosition.size(); i < n; i++)
	{
		// probe
		CGameObject *probeObj = zone->createEmptyObject();
		CLightProbe *probe = probeObj->addComponent<Lightmapper::CLightProbe>();

		CTransformEuler *probeTransform = probeObj->getTransformEuler();
		probeTransform->setPosition(probesPosition[i]);

		probes.push_back(probe);
	}

	CLightProbeRender::showProbe(true);

	context->setProbes(probes);
}

void CViewInit::onDestroy()
{

}

void CViewInit::onUpdate()
{
	CContext *context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("Common.Zip");
		listBundles.push_back("Sponza.Zip");

#ifdef __EMSCRIPTEN__
		std::vector<std::string> listFile;
		listFile.push_back("Sprite.zip");
		listFile.insert(listFile.end(), listBundles.begin(), listBundles.end());

		const char *filename = listFile[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				if (m_downloaded == 0)
				{
					// sprite.zip
					fileSystem->addFileArchive(filename, false, false, irr::io::EFAT_UNKNOWN, "", &m_spriteArchive);
				}
				else
				{
					// [bundles].zip
					fileSystem->addFileArchive(filename, false, false);
				}

				if (++m_downloaded >= listFile.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
	}
#else

#if defined(WINDOWS_STORE) || defined(MACOS)
		fileSystem->addFileArchive(getBuiltInPath("Sprite.zip"), false, false, irr::io::EFAT_UNKNOWN, "", &m_spriteArchive);
#else
		fileSystem->addFileArchive("Sprite.zip", false, false, irr::io::EFAT_UNKNOWN, "", &m_spriteArchive);
#endif

		for (std::string& bundle : listBundles)
		{
			const char *r = bundle.c_str();
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
		}

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		if (m_spriteArchive != NULL)
		{
			m_sprite = new CSpriteAtlas(video::ECF_A8R8G8B8, 2048, 2048);

			// get list sprite image
			std::vector<std::string> sprites;

			const io::IFileList *fileList = m_spriteArchive->getFileList();
			for (int i = 0, n = fileList->getFileCount(); i < n; i++)
			{
				const char *fullFileame = fileList->getFullFileName(i).c_str();
				const char *name = fileList->getFileName(i).c_str();

				m_sprite->addFrame(name, fullFileame);
			}

			m_sprite->updateTexture();
		}

		initScene();
		m_initState = CViewInit::Finished;
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene *scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
}
}

void CViewInit::onRender()
{

}
