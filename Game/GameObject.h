#pragma once
#include <iostream>
#include "Ogre.h"
#include "GameComponent.h"

using namespace Ogre;
// forward declare game class
class GameComponent;

class GameObject
{

   // name and integer identifier of objects (you'll need em later!)
   std::string name;
   std::string uniquename;
   unsigned int id;
   static unsigned int sId; // static value for current id

   // does a "gameobject" imply position/orientation in every case? I think 
   // it doesnt, but we can live with the overhead of a few bytes
   Quaternion mRot;
   Vector3 mPos;

   // COMPONENTS (this is how the object functions)
   std::vector<GameComponent *>mComponents;

public:
   GameObject(Vector3 pos,Quaternion rot, const char *name);
   ~GameObject(void);

   void setPosition(Vector3 pos) { mPos = pos; };
   Vector3 getPosition() { return mPos; };
   void setOrientation(Quaternion rot) { mRot = rot; };
   Quaternion getOrientation() { return mRot; };
   const char *getName() { return name.c_str(); };
   const char *getUniqueName() { return uniquename.c_str(); };
   unsigned int getId() { return id; };


   virtual void Update(float fTime,unsigned int iTime);
   virtual bool AddComponent(GameComponent *pComponent);
   virtual bool GetComponentByType(GameComponent::ComponentType type,GameComponent **ppCom);
	virtual void RemoveComponent(GameComponent *pCom);
   //virtual void RemoveComponent();
   
};

