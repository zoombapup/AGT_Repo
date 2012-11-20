#pragma once

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "./res/resource.h"
#endif

#include "Ogre.h"

using namespace Ogre;

// this is a base class which implements a renderer. For now it implements OGRE3D as the renderer
class Renderer
{
	SceneManager *m_pManager;
	Root *m_pRoot;

    Camera* m_pCamera;
    RenderWindow* m_pWindow;
	Ogre::String mResourcePath;

	// this feels like a nasty way of doing things, but some platforms require a window handle! (win32)
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	HWND m_hWnd;
#endif

public:
   Renderer(void);
   ~Renderer(void);

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   // window handle is the underlying windows window handle (used for a few things, so need accessor)
   virtual size_t getWindowHandle() { return (size_t) m_hWnd; };
#endif

   SceneManager * getSceneManager() { return m_pManager; };
   Root * getRoot() { return m_pRoot; };
   RenderWindow * getRenderWindow() { return m_pWindow; };

   bool Initialize();
   bool RenderFrame();

   // OGRE framework methods we stole!
   void createViewports(void);
   void createDefaultCamera(void);
   bool configure(void);
   void loadResources(void);
   void setupResources(void);
};
