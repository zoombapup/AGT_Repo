#pragma once

// Ogre stuff..
#include "Ogre.h"
#include "Message.h"
#include "Types.h"
using namespace Ogre;


class GameObject;

class GameComponent : public MessageListener
{
protected:
	GameObject *pParentGob;
public:

	// enumeration of all the components available to decorate an object with
	enum ComponentType
	{
		eCom_Transform,
		eCom_Render3D,
		eCom_Render2D,
		eCom_Physics,
		eCom_AI,
		eCom_Network,
		eCom_InputListener,
		eCom_MessageHandler,
		eCom_Camera,
		eCom_SimpleMover,
		eCom_SimpleAnimation,
      eCom_SimpleMouseCursor,
      eCom_SimpleSelection,
		eCom_PathCameraController,
	};
	ComponentType type;
	ComponentType getType() { return type; };
	void setType(ComponentType myType) { type = myType; };

   GameComponent(void);
   ~GameComponent(void);
   void setParent(GameObject *pParent) { pParentGob = pParent; };
   virtual void Update(float fTime,unsigned int iTime){};
	void PrintParentGobInfo(const char *text);
};



// ogre based render component, stores entity/scenenode/animation data for rendering an object
class OgreRenderInstanceComponent : public GameComponent
{
   // OGRE/RENDERER stuff
   SceneNode *pNode;
   Entity *pEnt;
   
public:
	OgreRenderInstanceComponent();
	~OgreRenderInstanceComponent();

	Entity *getEntity() { return pEnt; };
	SceneNode *getSceneNode() { return pNode; };
	bool Create(const char *mesh,GameObject *pGob, bool bShadow = false);
	void Update(float fTime,unsigned int iTime);
	void SetCollision(bool bCollide) { pEnt->setQueryFlags(0); };
};



class CameraComponent : public GameComponent
{
	Vector3 mTranslateVector;
	float mMoveScale;
	Degree mRotScale;
	Radian mRotX, mRotY;
public:
	CameraComponent();
	virtual void Update(float fTime,unsigned int iTime);
};

class PathCameraComponent : public GameComponent
{
public:
	PathCameraComponent();
	virtual void Init();
	virtual void Update(float fTime,unsigned int iTime);
};

class MoverComponent : public GameComponent
{
	Vector3 lastPos;
	Quaternion lastRot;
	Quaternion targetRot;
	Vector3 targetPos;
	float wanderdistance;	// how far we randomly wander from the last target
	float targetdistance;	// the minimum distance we need to achieve
	float fSpeed;
	float fRotSpeed;
	bool m_bWander;			// do we wander around or not?
public:
	MoverComponent();
	void Init(float wanderdistance, float targetdistance,float fSpeed,Vector3 startpos,float fRSpeed,bool bWander = false);
	virtual void Update(float fTime,unsigned int iTime);
	void MoveToLocationHandler(Message *pMsg);
};

enum 
{
	eANIM_WALK,
	eMAX_ANIMS
};


class AnimationComponent : public GameComponent
{
	Vector3 lastPos;
	AnimationStateSet *pAnimStates;
	float fStartTime;
	float fAnimSpeed;
public:
	void Init(float fAnimSpeed,float fTimeOffset = 0.0f);
	AnimationComponent();
	virtual void Update(float fTime,unsigned int iTime);
	void StartAnimation(Message *pMsg);
	void StopAnimation(Message *pMsg);
};

// shows some "selection" value by rendering a selection differently (i.e. a selection circle)
class SelectionComponent : public GameComponent
{
	bool bIsSelected;
	bool bIsHighlighted;
	// selection
	SceneNode *pNode;
	Entity *pEnt;
	// highlight clones the normal material, which we save so we can put it back
	MaterialPtr NormalMaterial;
	MaterialPtr HighlightMaterial;

public:
	SelectionComponent(void);
	void Init(void);
	void UpdateSelector(void);
	void HighlightChanged(Message *pMsg);
	void SelectionListChanged(Message *pMsg);
	void SelectionHandleMoveToCommand(Message *pMsg);
};


// used by mouse selection class!
class SelectionRectangle : public ManualObject
{
public:
    SelectionRectangle(const String &name):ManualObject(name)
    {
		setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
		setUseIdentityProjection(true);
		setUseIdentityView(true);
		setQueryFlags(0);
		setCastShadows(false);
    }

	// range -1..1 in X and Y.. bah!
    void draw(const float left, const float top, const float right, const float bottom)
    {
		clear();
		AxisAlignedBox box;
		box.setInfinite();
		setBoundingBox(box);

		begin("", RenderOperation::OT_LINE_STRIP);
		   position(left, top, -1);
		   position(right, top, -1);
		   position(right, bottom, -1);
		   position(left, bottom, -1);
		   position(left, top, -1);
		end();
    }
};




// Simple mouse selection cursor component
class MouseSelectionComponent : public GameComponent
{
	bool bIsSelecting;
	Entity *pEntity;
	SceneNode *pSceneNode;
	RaySceneQuery *mRaySceneQuery;
	Ray mouseRay;
	SelectionRectangle *pRect;
	Vector2 startPos,endPos;
	SceneQueryResult result;
	SceneNode *pCurrentNode;
	Vector3 mLastCollision;
public:
	MouseSelectionComponent();
	~MouseSelectionComponent();
	void UpdateSelection();
	virtual void Update(float fTime,unsigned int iTime);
	void performRectSelect(float left,float top, float right,float bottom,Viewport *pView);
	void performRaySelect(Viewport *pView,Vector2 mousePos,bool bHighlightOnly = false);

	void MouseMoved(Message *pMsg);
	void MousePressed(Message *pMsg);
	void MouseReleased(Message *pMsg);
};

// this class basically acts as a bounds box collision check. Anything
// with one of these checks for collisions against things.
// TODO: Once the basics work, need to use a "collision manager" to
// increase performance.. 
class BoundedCollisionComponent : public GameComponent
{
	
public:
	virtual void Update(float fTime,unsigned int iTime);
};

/*
	Handles all of the AI for a GOB

*/
class AIComponent : public GameComponent
{
public:
	virtual void Update(float fTime,unsigned int iTime);

};
