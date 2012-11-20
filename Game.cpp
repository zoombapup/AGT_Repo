
// need this for timeGetTime
#include "windows.h"
// need this to remove horrid windows #define SendMessage corrupting our namespace
#undef SendMessage

#include "game.h"
#include "Input.h"
#include "Renderer.h"
#include "UserInterface.h"
#include ".\Game\GameObject.h"
#include "WorldLoader.h"
#include "crtdbg.h"

using namespace Ogre;

Game::Game(void)
{
}

Game::~Game(void)
{
}


bool Game::Initialize()
{
	// PC: Todo: Renderer needs to have its window handle accessible for input manager..
	// currently, this means we need to instantiate the renderer first, but we need
	// to somehow assert if this doesnt happen (so others know WHY we need it first)
	// create our renderer
	pRenderer = new Renderer();
	if(pRenderer)
	pRenderer->Initialize();

   // creates and then initialises the Input system
   Input::GetInstance()->Init(this);

   // creates and then initializes the UI (note: do input first!)
   UserInterface::GetInstance()->Init(this);



	return true;  
};

UserInterface * Game::getUI()
{
   return UserInterface::GetInstance();
}

void Game::Go()
{
/*
   // Get the current bits
   tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

   // Clear the upper 16 bits and OR in the desired freqency
   tmp = (tmp & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;
*/


	Initialize();
	m_bRunning = true;
	CreateTestScene();

   // Set the new bits
   //_CrtSetDbgFlag(tmp);

   try
   {
	   MainLoop();
      CleanUp(); 
   }
   catch( OIS::Exception &e)
   {
      std::cerr << "An exception has occured: " <<	e.eText << std::endl;
   }
	
};

// TODO: this should be removed, but is useful for test purposes
// it should be replaced by a nicer "LevelLoader" interface perhaps?
void Game::CreateTestScene()
{
	  //WorldLoader wld;
	  //wld.CreateTestWorld(this);

	  // create camera
	  Viewport * pView  = pRenderer->getRenderWindow()->getViewport(0);
	  Camera *pCam = pView->getCamera();

	/// This just sets up a default first person freelook camera for now.. 

	  // TODO: do we automatically add created objects to the list of objects in game??
	  // what if we DONT want it added?
	  GameObject *pObj = CreateObject(pCam->getPosition(),pCam->getOrientation(),"Camera",eCamera,false);
	  if(pObj)
	  {
			AddGob(pObj);
	  }
	  GameComponent *pCom = CreateComponent(GameComponent::eCom_Camera);
	  pObj->AddComponent(pCom);
      pCom = CreateComponent(GameComponent::eCom_SimpleMouseCursor);
	  pObj->AddComponent(pCom);

	  MessageManager::GetInstance().SendMessage(0,eMSG_CREATETESTSCENE,0,NULL);
};
bool Game::AddGob(GameObject *pGob)
{
	// TODO: assert the pointer is valid!!!
	mGobs.push_back(pGob);

	// TODO: can this fail? if so, we need to consider how and wether
	// we handle that with some return value etc.
	return true;
}

void Game::CleanUp()
{
	// TODO: iterate over the object list and release all objects
	// currently this leaks memory like an exxon oil tanker
   
	while (!mGobs.empty())
	{
      GameObject *pObj = mGobs.front();
      mGobs.pop_front();
		delete pObj;
	}

   // delete the renderer
   if(pRenderer) 
   {
      delete pRenderer;
      pRenderer = NULL;
   }
};

void Game::UpdateGobs(float fTime,unsigned int iTime)
{
	std::list<GameObject *>::iterator it;
	for (it = mGobs.begin(); it != mGobs.end(); it++)
	{
		(*it)->Update(fTime,iTime);
	}
};

GameObject * Game::CreateObject(Vector3 pos, Quaternion rot, const char *name, ObjectType type,bool bAddToGobList)
{
	GameObject *pGob = new GameObject(pos,rot,name);
	if(bAddToGobList)
	{
		AddGob(pGob);
	}
	return pGob;
}

// -1.0 to 1.0
float getRandomFloat()
{
	return ((float(rand())/float(RAND_MAX))*2.0f) - 1.0f;
}

Vector3 Game::getRandomVector(float scale)
{
	Vector3 ran;
    float f = 

	ran.x = scale * getRandomFloat();
	ran.y = scale * getRandomFloat();
	ran.y = 0.0f;
	ran.z = scale * getRandomFloat();
	return ran;
}

// component factory method.. creates "components" which are added to "gameobjects"
GameComponent * Game::CreateComponent(GameComponent::ComponentType type)
{
	GameComponent *pCom = NULL;
	switch(type)
	{
		case GameComponent::eCom_Camera:
			pCom = (GameComponent *) new CameraComponent();
			break;
		case GameComponent::eCom_Render3D:
			pCom = (GameComponent *) new OgreRenderInstanceComponent();
			break;
		case GameComponent::eCom_SimpleMover:
			pCom = (GameComponent *) new MoverComponent();
			break;
		case GameComponent::eCom_SimpleAnimation:
			pCom = (GameComponent *) new AnimationComponent();
			break;
		case GameComponent::eCom_SimpleMouseCursor:
			pCom = (GameComponent *) new MouseSelectionComponent();
			break;
		case GameComponent::eCom_SimpleSelection:
			pCom = (GameComponent *) new SelectionComponent();
			break;
		case GameComponent::eCom_PathCameraController:
			pCom = (GameComponent *) new PathCameraComponent();
			break;

			
	default:
			break;
	};
	pCom->setType(type);
	return pCom;
};

Input * Game::getInput()
{
   return Input::GetInstance();
}
// return from here to exit
void Game::MainLoop()
{
	timeBeginPeriod(1);
	static int iLastTime = timeGetTime();
	
	while(m_bRunning)
	{
		// call the windows message pump so our app can actually recieve events and whatnot..
		WindowEventUtilities::messagePump(); 

		// need to capture the input when using OIS apparently!
      Input::GetInstance()->capture();
		// handle input
      if(Input::GetInstance()->getQuitKeyPressed() == true)
      {
			m_bRunning = false;
         Input::GetInstance()->Release();
      }
      else
      {
		   int iNewTime = timeGetTime();
		   int iTime = iNewTime - iLastTime;
		   iLastTime = iNewTime;
	      float fTime = (float)iTime / 1000.0f;

         
		   // update objects
		   UpdateGobs(fTime,iTime);

		   // render
		   if(pRenderer)
		   {
			   bool bRet = pRenderer->RenderFrame();
			   // if anything causes the renderer to die, bail out now
			   if(bRet == false) 
				   m_bRunning = false;
		   }

         UserInterface::GetInstance()->Update(this);
      }

	}
};