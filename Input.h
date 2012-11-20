#pragma once

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
#include "Types.h"

// forward declarations
class Game;

namespace InputMap
{
	enum MouseButtonID
	{
		MB_Left = 0, MB_Right, MB_Middle,
		MB_Button3, MB_Button4,	MB_Button5, MB_Button6,	MB_Button7
	};
}
// TODO: split this into its own .h and .cpp
class Input : public OIS::KeyListener, public OIS::MouseListener
{
protected:
	bool bQuit;
	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	OIS::JoyStick* mJoy;
	OIS::MouseState ms;
   Input(void);
   ~Input(void);

public:
   static Input * GetInstance() 
   { 
	   static Input theInput; 
	   return &theInput; 
   };
   virtual void Init(Game *pGame);
   virtual void Release();

   // OIS event handlers.. will get repackaged as "input" events..

   // TODO: remove this hideous hack job!!
   virtual bool keyPressed(const OIS::KeyEvent &arg)
   { 
	   if( arg.key == OIS::KC_Q || arg.key == OIS::KC_ESCAPE )
	   {
			bQuit = true;
	   }
	   return true; 
   };
   virtual void capture() { mKeyboard->capture(); mMouse->capture(); ms = mMouse->getMouseState(); };
   virtual bool keyReleased(const OIS::KeyEvent &arg){ return true; };
   virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
   virtual bool mouseMoved(const OIS::MouseEvent &arg);
   virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

   // TODO: remove this temporary hack job!
   virtual bool getQuitKeyPressed() 
   {
	   if(mKeyboard->isKeyDown(OIS::KC_Q)||mKeyboard->isKeyDown(OIS::KC_ESCAPE))
	   {
			return true;
	   }
	   return false;
   };

   virtual bool getLeftKeyPressed()		{ return mKeyboard->isKeyDown(OIS::KC_A); };
   virtual bool getRightKeyPressed()	{ return mKeyboard->isKeyDown(OIS::KC_D); };
   virtual bool getForwardKeyPressed()	{ return mKeyboard->isKeyDown(OIS::KC_W); };
   virtual bool getBackKeyPressed()		{ return mKeyboard->isKeyDown(OIS::KC_S); };
   virtual bool getSpaceBarPressed()	{ return mKeyboard->isKeyDown(OIS::KC_SPACE); };
   virtual bool getShiftKeyPressed()	{ return mKeyboard->isKeyDown(OIS::KC_LSHIFT); };

   virtual bool getLeftMousePressed()	{ return ms.buttonDown(OIS::MouseButtonID::MB_Left); };
   virtual bool getRightMousePressed()	{ return ms.buttonDown(OIS::MouseButtonID::MB_Right); };
   virtual int	getMouseRelativeX()		{ return ms.X.rel; };
   virtual int	getMouseRelativeY()		{ return ms.Y.rel; };

};
