#pragma once

class Game;
#include "Ogre.h"
using namespace Ogre;


class UserInterface
{
   // these become protected so that we enforce the singleton
   UserInterface(void);
   ~UserInterface(void);

   Vector2 mousePos;

public:
   static UserInterface * GetInstance() { static UserInterface theUserInterface; return &theUserInterface; };

   void Init(Game *pGame);
   // the UI has a number of GUI screens, set them up here..
   void SetupUserInterface();
   void ShowMouse(bool bShow);
   void Update(Game *pGame);
   Vector2 getMousePosition() { return mousePos; };

};

