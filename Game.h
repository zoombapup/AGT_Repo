#pragma once

#include "Ogre.h"
#include "gamecomponent.h"
#include <list>
#include "WorldLoader.h"
#include "Types.h"

using namespace Ogre;

// forward declarations
class GameObject;
class GameComponent;
class Renderer;
class Input;
class UserInterface;



// enumeration of all the object types available to be created
enum ObjectType
{
   eStaticMeshObject,
   eAIObject,
   eDynamicObject,
   eCamera
};



class Game
{
   // linked list of game objects
   std::list<GameObject *>mGobs;
   
   // member "manager" instances
   Renderer *pRenderer;
   // this is a bit of a hack, use this to determine if the game has decided it wants to exit!
   bool m_bRunning;

   WorldLoader wld;

   Game(void);
   ~Game(void);
public:
	static Game& GetInstance() { static Game theGame; return theGame; };


   // class factory methods.. just creates the objects, does not initalize them, that
   // is up to the caller (note: There may be composite objects which ARE initialized)
   // we need to decide wether CreateObject can actually initialize things really??
   GameObject * CreateObject(Vector3 pos,Quaternion rot,const char *name,ObjectType type, bool bAddToGobList = true);
   GameComponent * CreateComponent(GameComponent::ComponentType type);
   bool AddGob(GameObject *pGob);


   bool Initialize();
   void CleanUp();
   void Go();
   void UpdateGobs(float fTime,unsigned int iTime);
   void MainLoop();

   void CreateTestScene();
   
   Vector3 getRandomVector(float scale);
   Renderer *getRenderer() { return pRenderer; };
   Input *getInput();
   UserInterface *getUI();

};
