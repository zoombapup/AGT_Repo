#include "renderer.h"

// abstract base..
Renderer::Renderer(void)
{
        m_pRoot = NULL;
		m_pManager = NULL;
		// Provide a nice cross platform solution for locating the configuration files
		// On windows files are searched for in the current working directory, on OS X however
		// you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
		mResourcePath = "";
#endif
}

Renderer::~Renderer(void)
{
	if(m_pRoot)
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
}

bool Renderer::RenderFrame()
{
	// TODO: assert that the m_pRoot isnt NULL!!!

	return m_pRoot->renderOneFrame();
};


void Renderer::createDefaultCamera(void)
{
	// Create the camera
	m_pCamera = m_pManager->createCamera("PlayerCam1");

	// Position it at 500 in Z direction
	m_pCamera->setPosition(Vector3(0,40,140));
	// Look back along -Z
	m_pCamera->lookAt(Vector3(0,0,-300));
	m_pCamera->setNearClipDistance(5);
}

// These internal methods package up the stages in the startup process
/** Sets up the application - returns false if the user chooses to abandon configuration. */
bool Renderer::Initialize(void)
{

	String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
	pluginsPath = mResourcePath + "plugins.cfg";
#endif
	
    m_pRoot = new Root(pluginsPath,mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

	// setup default resources from resource paths etc.
    setupResources();

	// calls the configuration dialog box if needed
    bool carryOn = configure();
    if (!carryOn) return false;

	// create the scene manager
    m_pManager = m_pRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	// setup a default camera
    createDefaultCamera();

	// and a viewport
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Load resources
	loadResources();

    return true;
}

bool Renderer::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
	if(/*m_pRoot->restoreConfig() || */m_pRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        m_pWindow = m_pRoot->initialise(true,"Renderer Framework Test");
		// Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		m_pWindow->getCustomAttribute("WINDOW", (void*)&m_hWnd);
		LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
		SetClassLong( m_hWnd, GCL_HICON, iconID );
#endif
        return true;
    }
    else
    {
        return false;
    }
}
void Renderer::createViewports(void)
{
    // Create one viewport, entire window
    Viewport* vp = m_pWindow->addViewport(m_pCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    m_pCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
};

/// Method which will define the source of resources (other than current folder)
void Renderer::setupResources(void)
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load(mResourcePath + "resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }
}

/// Optional override method where you can perform resource group loading
/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
void Renderer::loadResources(void)
{
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


