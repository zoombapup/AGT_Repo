#include "input.h"
#include "Game.h"
#include "Renderer.h"

using namespace OIS;

Input::Input(void)
{
	bQuit = false;
}

Input::~Input(void)
{
}


void Input::Release()
{
   if(mKeyboard)
   {
      mKeyboard->setEventCallback(NULL);
      mInputManager->destroyInputObject(mKeyboard);
      mKeyboard = NULL;
   }

   if (mMouse)
   {
      mMouse->setEventCallback(NULL);
      mInputManager->destroyInputObject(mMouse);
      mMouse = NULL;
   }

   if (mInputManager)
	OIS::InputManager::destroyInputSystem(mInputManager);
   mInputManager = NULL;
}

// TODO: make this a singleton
void Input::Init(Game *pGame)
{
   // setup the Input manager
   OIS::ParamList pl;
   size_t windowHnd = 0;
   std::ostringstream windowHndStr;
   bool bufferedKeys = false;
   bool bufferedMouse = true;
   bool bufferedJoy = false;


   windowHnd = pGame->getRenderer()->getWindowHandle();
   windowHndStr << windowHnd;
   pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
   mInputManager = InputManager::createInputSystem( pl );
   // TODO: add an assert that we actually created the input manager

   // and the various handlers/callbacks
   //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
   mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, bufferedKeys ));
   mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, bufferedMouse ));
   try {
	   //mJoy = static_cast<JoyStick*>(mInputManager->createInputObject( OISJoyStick, bufferedJoy ));
   }
   catch(...) {
	   mJoy = 0;
   }
   
   mMouse->setEventCallback(this);
   mKeyboard->setEventCallback(this);
   mKeyboard->setBuffered(true);
}

bool Input::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	MessageManager::GetInstance().SendMessage(0,eMSG_MOUSEPRESSED,id,NULL);
	return true; 
};
bool Input::mouseMoved(const OIS::MouseEvent &arg)
{ 
	MessageManager::GetInstance().SendMessage(0,eMSG_MOUSEMOVED,0,NULL);
	return true; 
};
bool Input::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{ 
	MessageManager::GetInstance().SendMessage(0,eMSG_MOUSERELEASED,id,NULL);
	return true; 
};

