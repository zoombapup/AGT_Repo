#include "userinterface.h"
#include "game.h"
#include "renderer.h"
#include "input.h"

using namespace Ogre;

UserInterface::UserInterface(void)
{

}

UserInterface::~UserInterface(void)
{
}

void UserInterface::ShowMouse(bool bShow)
{
   if(bShow)
   {
   }
   else
   {
   }
}

void UserInterface::Init(Game *pGame)
{
   RenderWindow *pWindow = pGame->getRenderer()->getRenderWindow();
   SceneManager *pManager = pGame->getRenderer()->getSceneManager();
   // TODO: You need to call initialization on whatever GUI system you use here!!


}

void UserInterface::Update(Game *pGame)
{
   float mMouseX = pGame->getInput()->getMouseRelativeX();
   float mMouseY = pGame->getInput()->getMouseRelativeY();
}

