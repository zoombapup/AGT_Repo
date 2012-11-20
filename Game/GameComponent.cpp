#include "gamecomponent.h"
#include "gameobject.h"

// TODO: this looks nasty, fix!!!
#include "Game.h"
#include "Renderer.h"
#include "Input.h"
#include "Message.h"
#include "UserInterface.h"

using namespace Ogre;
using namespace InputMap;



GameComponent::GameComponent(void)
{
	pParentGob = NULL;
}

GameComponent::~GameComponent(void)
{
}


void GameComponent::PrintParentGobInfo(const char *text)
{
	String s(text);
	s += " Parent GOB: ";
	StringConverter conv;
	s += conv.toString(pParentGob);
	s += " id: ";
	s += conv.toString(pParentGob->getId());
	s += " name: ";
	s += pParentGob->getUniqueName();
	LogManager::getSingleton().logMessage(s);
}

CameraComponent::CameraComponent()
{
	mTranslateVector.x = 0.0f;
	mTranslateVector.y = 0.0f;
	mTranslateVector.z = 0.0f;
	mMoveScale = 4.0f;
}

void CameraComponent::Update(float fTime, unsigned int iTime)
{
	mTranslateVector = mTranslateVector.ZERO;
	mRotX = 0.0f;
	mRotY = 0.0f;

	// get the input and camera objects from out of the game object
	Camera *pCamera = Game::GetInstance().getRenderer()->getRenderWindow()->getViewport(0)->getCamera();
	Input *pInput = Game::GetInstance().getInput();
	float fMoveScale = mMoveScale;
	if(pInput->getShiftKeyPressed()) fMoveScale = (mMoveScale * 4.0f);

	if(pInput->getLeftKeyPressed())
	{
		mTranslateVector.x = -fMoveScale;	// Move camera left
	}

	if(pInput->getRightKeyPressed())
	{
		mTranslateVector.x = fMoveScale;	// Move camera left
	}

	if(pInput->getForwardKeyPressed())
	{
		mTranslateVector.z = -fMoveScale;	// Move camera forward
	}

	if(pInput->getBackKeyPressed())
	{
		mTranslateVector.z = fMoveScale;	// Move camera forward
	}

	if( pInput->getRightMousePressed())
	{
      mRotX = Degree(-pInput->getMouseRelativeX() * 0.13);
		mRotY = Degree(-pInput->getMouseRelativeY() * 0.13);
		//mTranslateVector.x += pInput->getMouseRelativeX() * 0.13;
		//mTranslateVector.y -= pInput->getMouseRelativeY() * 0.13;
	}
	else
	{
		//mRotX = Degree(-pInput->getMouseRelativeX() * 0.13);
		//mRotY = Degree(-pInput->getMouseRelativeY() * 0.13);
	}

	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
	//(e.g. airplane)
	pCamera->yaw(mRotX);
	pCamera->pitch(mRotY);
	pCamera->moveRelative(mTranslateVector);


}

PathCameraComponent::PathCameraComponent()
{
	pParentGob = NULL;
};

void PathCameraComponent::Init()
{
	// get the transform from the parent GOB and update the visual entities
	if(pParentGob)
	{
		SendMessage(pParentGob->getId(),eMSG_ANIMATIONSTART,true,"CameraTrack1");
	}
}

void PathCameraComponent::Update(float fTime,unsigned int iTime)
{
};

OgreRenderInstanceComponent::OgreRenderInstanceComponent()
{
	pParentGob = NULL;
};

void OgreRenderInstanceComponent::Update(float fTime,unsigned int iTime)
{
	// get the transform from the parent GOB and update the visual entities
	if(pParentGob)
	{
		pNode->setPosition(pParentGob->getPosition());
		pNode->setOrientation(pParentGob->getOrientation());
	}
};

bool OgreRenderInstanceComponent::Create(const char *mesh,GameObject *pGob,bool bShadow)
{
	// store the parent just in case
	setParent(pGob);

	// create the renderer objects 
	SceneManager *pSceneMgr = Game::GetInstance().getRenderer()->getSceneManager();
	pEnt = pSceneMgr->createEntity(pGob->getUniqueName(),mesh);
	pNode = pSceneMgr->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEnt);
	pEnt->setCastShadows(bShadow);

	// update the graphics pos using our update function..
	Update(0.0f,0);
	// TODO: check that we actually succeeded in creating the render data (Ent, Node etc)

	// register our RenderInstances SceneNode with the global SceneNode->Gob map.
	// register our RenderInstances Entity with the global Entity->Gob map.

	return true;
};

MoverComponent::MoverComponent()
{
	wanderdistance = 0.0f;
	targetdistance = 0.0f;
	targetPos = targetPos.ZERO;
	lastPos = lastPos.ZERO;
	m_bWander = false;
}

void MoverComponent::Init(float wdist, float tdist,float fspd, Vector3 startpos,float fRSpeed,bool bWander)
{
	wanderdistance = wdist;
	targetdistance = tdist;
	m_bWander = bWander;
	fSpeed = fspd;
	targetPos = startpos;
	// compute a new wander vector
	//targetPos += Game::GetInstance().getRandomVector(1000.0f);
	fRotSpeed = fRSpeed;
	MessageManager::GetInstance().SubscribeMessage(eMSG_MOVETOLOCATION)->bind(this,&MoverComponent::MoveToLocationHandler);
}

void MoverComponent::MoveToLocationHandler(Message *pMsg)
{
#ifdef _DEBUGMOVE
	MessageManager::GetInstance().PrintMessageInfo("Mover MOVETO: ",pMsg);
	PrintParentGobInfo("Mover: ");
#endif
	if(pMsg->idSender == pParentGob->getId())
	{
#ifdef _DEBUGMOVE
		LogManager::getSingleton().getDefaultLog()->logMessage("Actually Moving");
#endif
		// we got a message from ourselves, so lets perform the operation
		Vector3 *pVect = (Vector3 *)pMsg->pData;
		targetPos = *pVect;

		// lets determine if we need to move and if so, we need to start animating
		Vector3 pos = pParentGob->getPosition();
		Vector3 targetvect = targetPos - pos;
		if (targetvect.length() > targetdistance)
		{
			// need to start walking.. play animation
			SendMessage(pParentGob->getId(),eMSG_ANIMATIONSTART,true,"Walk");
		}
	}
}

void MoverComponent::Update(float fTime,unsigned int iTime)
{
	if(pParentGob)
	{
		Vector3 pos = pParentGob->getPosition();
		Vector3 targetvect = targetPos - pos;
		if (targetvect.length() < targetdistance)
		{
			if(m_bWander)
			{
				// compute a new wander vector
				targetPos += Game::GetInstance().getRandomVector(1000.0f);
				targetvect = targetPos - pos;
				targetvect.normalise();
				// need to start walking.. play animation
				SendMessage(pParentGob->getId(),eMSG_ANIMATIONSTART,true,"Walk");
			}
			else
			{
				SendMessage(pParentGob->getId(),eMSG_MOVECOMPLETE,0,0);
				SendMessage(pParentGob->getId(),eMSG_ANIMATIONSTOP,false,"Walk");
			}
		}
		else
		{
			targetvect.normalise();
			Quaternion old = pParentGob->getOrientation();
			Vector3 src = old * Vector3::UNIT_X;
			Quaternion quat = src.getRotationTo(targetvect) * old;
			pParentGob->setOrientation(Quaternion::Slerp(fRotSpeed * fTime,old,quat,true));
			pParentGob->setPosition(pos + ((targetvect * fSpeed) * fTime));
		}

	}
}
AnimationComponent::AnimationComponent()
{
	pAnimStates = NULL;	
}

void AnimationComponent::Init(float fSpeed, float fTimeOffset)
{
	MessageManager::GetInstance().SubscribeMessage(eMSG_ANIMATIONSTART)->bind(this,&AnimationComponent::StartAnimation);
	MessageManager::GetInstance().SubscribeMessage(eMSG_ANIMATIONSTOP)->bind(this,&AnimationComponent::StopAnimation);

	if(pParentGob)
	{
		GameComponent *pCom;
		pParentGob->GetComponentByType(GameComponent::eCom_Render3D,&pCom);
		OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pCom);
		pAnimStates = pORC->getEntity()->getAllAnimationStates();

	}
}
void AnimationComponent::StartAnimation(Message *pMsg)
{
	// for now, only the parent can send this a message to animate!
	if (pMsg->idSender != pParentGob->getId()) return;
	if(NULL == pAnimStates) return;

	AnimationState *pAnimState = pAnimStates->getAnimationState((const char *)pMsg->pData);
	if(pAnimState)
	{
		// play the walk anim
		pAnimState->setEnabled(true);
		pAnimState->setLoop(pMsg->iValue);
		pAnimState->setTimePosition(0);
	};
	return;




	if (pMsg->idSender == pParentGob->getId())
	{
		// got a message to start an animation.. 
		switch(pMsg->iValue)
		{
		case eANIM_WALK:
			/*
						AnimationState *pAnimState = pAnimStates->getAnimationState("Walk");
						if(pAnimState)
						{
							// play the walk anim
							pAnimState->setEnabled(true);
							pAnimState->setLoop(true);
							pAnimState->setTimePosition(0);
						};
						
						*/
			break;
		}
	}
}
void AnimationComponent::StopAnimation(Message *pMsg)
{
	if (pMsg->idSender != pParentGob->getId()) return;
	if(NULL == pAnimStates) return;

	AnimationState *pAnimState = pAnimStates->getAnimationState((const char *)pMsg->pData);
	if(pAnimState)
		pAnimState->setEnabled(false);
	return;

	if (pMsg->idSender == pParentGob->getId())
	{
		// got a message to start an animation.. 
		switch(pMsg->iValue)
		{
		case eANIM_WALK:
			/*
						// stop playing the walk anim
						AnimationState *pAnimState = pAnimStates->getAnimationState("Walk");
						if(pAnimState)
							pAnimState->setEnabled(false);
						
						*/
			break;
		}
	}
}


void AnimationComponent::Update(float fTime,unsigned int iTime)
{
	if(NULL == pAnimStates) return;

	if(pAnimStates->hasEnabledAnimationState())
	{
		AnimationStateIterator it = pAnimStates->getAnimationStateIterator();
		while(it.hasMoreElements())
		{
			AnimationState *pAnimState = it.getNext();
			pAnimState->addTime(fTime);
		}
	}
}


SelectionComponent::SelectionComponent(void)
{
	bIsSelected = false;
	bIsHighlighted = false;
	pNode = NULL;
   pEnt = NULL;
	NormalMaterial.setNull();
	HighlightMaterial.setNull();
}

void SelectionComponent::Init(void)
{
	// listen for any "selection" messages
   MessageManager::GetInstance().SubscribeMessage(eMSG_HIGHLIGHTCHANGED)->bind(this,&SelectionComponent::HighlightChanged);
	MessageManager::GetInstance().SubscribeMessage(eMSG_SELECTIONLISTCHANGED)->bind(this,&SelectionComponent::SelectionListChanged);
	MessageManager::GetInstance().SubscribeMessage(eMSG_SELECTEDMOVETOLOCATION)->bind(this,&SelectionComponent::SelectionHandleMoveToCommand);

   // display our selector based on the bIsSelected boolean
   GameComponent *pRI = NULL;
   // get the Render3D component
   pParentGob->GetComponentByType(GameComponent::eCom_Render3D,&pRI);
   OgreRenderInstanceComponent *pRenderInst = NULL;
   // and if we have one, cast it to a specific type for now
   if(pRI)
       pRenderInst = dynamic_cast<OgreRenderInstanceComponent *>(pRI);
   // and if that worked, lets update our selection status
   if(pRenderInst)
   {
      std::string highlightname("SEL/");
      highlightname += pParentGob->getUniqueName();


		NormalMaterial = pRenderInst->getEntity()->getSubEntity(0)->getMaterial();
		HighlightMaterial = NormalMaterial->clone(highlightname);
		HighlightMaterial->setAmbient(0.0f,0.0f,1.0f);
		HighlightMaterial->setDiffuse(Ogre::ColourValue(1.0f,0.0f,0.0f));
		HighlightMaterial->getTechnique(0)->getPass(0)->setAmbient(0.0f,0.0f,1.0f);
		HighlightMaterial->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(1.0f,0.0f,0.0f));
      

      Plane plane(Vector3::UNIT_Y,5);
      MeshManager::getSingleton().createPlane(highlightname,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
         plane,120,120,1,1,true,1,1,1,Vector3::UNIT_Z);

      pEnt = Game::GetInstance().getRenderer()->getSceneManager()->createEntity(highlightname, highlightname);
      pEnt->setMaterialName("PAC/Selected");

		pNode = pRenderInst->getSceneNode();
      pNode->attachObject(pEnt);
      pEnt->setCastShadows(false);
      pEnt->setVisible(bIsSelected);
   }

}

void SelectionComponent::HighlightChanged(Message *pMsg)
{
	  // display our selector based on the bIsSelected boolean
   GameComponent *pRI = NULL;
   // get the Render3D component
   pParentGob->GetComponentByType(GameComponent::eCom_Render3D,&pRI);
   OgreRenderInstanceComponent *pRenderInst = NULL;
   // and if we have one, cast it to a specific type for now
   if(pRI)
       pRenderInst = dynamic_cast<OgreRenderInstanceComponent *>(pRI);
   // and if that worked, lets update our selection status
   if(pRenderInst)
   {
		bIsHighlighted = false;
		if(pMsg->pData)
		{
			if(pNode == pMsg->pData)
			{
				bIsHighlighted = true;
				Entity *pEnt = pRenderInst->getEntity();
				String name = HighlightMaterial->getName();
				pEnt->setMaterialName("PAC/Selected");
				//pNode->showBoundingBox(true);
			
			}
			else
			{
				Entity *pEnt = pRenderInst->getEntity();
				String name = NormalMaterial->getName();
				pEnt->setMaterialName(name);
				//pNode->showBoundingBox(false);
			}
		}
	}
};
// MOVETO command sent from the ray/picking system and should be handled if this
// object is selected.. otherwise, ignore it
void SelectionComponent::SelectionHandleMoveToCommand(Message *pMsg)
{
#ifdef _DEBUGSELECTION
	PrintParentGobInfo("SelectionB");
#endif
	if(bIsSelected)
	{
#ifdef _DEBUGSELECTION
		PrintParentGobInfo("Selection");
#endif
		// simply send another message to say to perform the move..
		SendMessage(pParentGob->getId(),eMSG_MOVETOLOCATION,0,pMsg->pData);
	}
};
void SelectionComponent::SelectionListChanged(Message *pMsg)
{
#ifdef _DEBUGSELECTION
	PrintParentGobInfo("SelectionListChanged");
#endif
	SceneQueryResult *pResult = NULL;
	pResult = (SceneQueryResult *) pMsg->pData;
	if (pResult)
	{
		bIsSelected = false;
		// for now, lets highlight the selection manually
		SceneQueryResultMovableList::iterator itr;
		for (itr = pResult->movables.begin(); itr != pResult->movables.end(); ++itr)
		{
			if(pNode == (*itr)->getParentSceneNode()) 
				bIsSelected = true;
		}
	}
	UpdateSelector();

#ifdef _DEBUGSELECTION
	if(bIsSelected)
	{
		PrintParentGobInfo("Selected: ");
	}
	else
	{
		PrintParentGobInfo("UnSelected: ");
	}
#endif
};


void SelectionComponent::UpdateSelector()
{
   if(pEnt)
      pEnt->setVisible(bIsSelected);
}

MouseSelectionComponent::MouseSelectionComponent(void)
{
	bIsSelecting = false;
	pCurrentNode = NULL;
   mRaySceneQuery = Game::GetInstance().getRenderer()->getSceneManager()->createRayQuery(mouseRay);
   pRect = new SelectionRectangle("sel");
   Game::GetInstance().getRenderer()->getSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(pRect);
	MessageManager::GetInstance().SubscribeMessage(eMSG_MOUSEMOVED)->bind(this,&MouseSelectionComponent::MouseMoved);
	MessageManager::GetInstance().SubscribeMessage(eMSG_MOUSEPRESSED)->bind(this,&MouseSelectionComponent::MousePressed);
	MessageManager::GetInstance().SubscribeMessage(eMSG_MOUSERELEASED)->bind(this,&MouseSelectionComponent::MouseReleased);

}

MouseSelectionComponent::~MouseSelectionComponent(void)
{
  Game::GetInstance().getRenderer()->getSceneManager()->destroyQuery(mRaySceneQuery);
   mRaySceneQuery = NULL;
	mLastCollision.x = mLastCollision.y = mLastCollision.z = 0.0f;
	mLastCollision.ZERO;
   
	MessageManager::GetInstance().UnSubscribeMessage(eMSG_MOUSEMOVED,this);
	MessageManager::GetInstance().UnSubscribeMessage(eMSG_MOUSERELEASED,this);
	MessageManager::GetInstance().UnSubscribeMessage(eMSG_MOUSEPRESSED,this);

}

static void swap(float &x, float &y)
{
  float tmp = x;
  x = y;
  y = tmp;
}

void MouseSelectionComponent::MouseMoved(Message *pMsg)
{
	if(bIsSelecting)
	{
		// lets update the selection rectangle
		endPos = Game::GetInstance().getUI()->getMousePosition();
	}
}
void MouseSelectionComponent::MousePressed(Message *pMsg)
{
	if(pMsg->iValue == MouseButtonID::MB_Left)
	{
		bIsSelecting = true;
		startPos = Game::GetInstance().getUI()->getMousePosition();
		endPos = startPos;
	}
	
}

void MouseSelectionComponent::MouseReleased(Message *pMsg)
{
	if(pMsg->iValue == MouseButtonID::MB_Right)
	{
		if(pCurrentNode)
		{
			if(pCurrentNode->getAttachedObject(0)->getName() == "GroundEntity")
			{
				//assert(result.movables.size() > 1);
				SendMessage(pParentGob->getId(),eMSG_SELECTEDMOVETOLOCATION,0,&mLastCollision);
				
				// clear out any selection..
				result.movables.clear();
				// inform everyone they are no longer selected
				SendMessage(pParentGob->getId(),eMSG_SELECTIONLISTCHANGED,0,&result);
				
			}
		}
	}

	if(pMsg->iValue == MouseButtonID::MB_Left)
	{

		
		if(pCurrentNode)
		{
			if(pCurrentNode->getAttachedObject(0)->getName() == "GroundEntity")
			{/*
				SendMessage(pParentGob->getId(),eMSG_SELECTEDMOVETOLOCATION,0,&mLastCollision);
				
				// clear out any selection..
				result.movables.clear();
				// inform everyone they are no longer selected
				SendMessage(pParentGob->getId(),eMSG_SELECTIONLISTCHANGED,0,&result);
				*/
		
			}
			else
			{
				//make sure we absolutely have the correct click position
				endPos = Game::GetInstance().getUI()->getMousePosition();
				Viewport *pView = Game::GetInstance().getRenderer()->getRenderWindow()->getViewport(0);
				performRaySelect(pView,endPos);
			}
		}
		//pCurrentNode = NULL;
		bIsSelecting = false;
		pRect->setVisible(false);

	}
}


void MouseSelectionComponent::Update(float fTime,unsigned int iTime)
{
	// lets update the selection rectangle or selection highlight
	endPos = Game::GetInstance().getUI()->getMousePosition();
	Viewport *pView = Game::GetInstance().getRenderer()->getRenderWindow()->getViewport(0);

	// deselect all objects
	//SendMessage(pParentGob->getId(),eMSG_SELECTNONE,0,NULL);

	if(bIsSelecting)
	{

		float fWidth = pView->getActualWidth();
		float fHeight= pView->getActualHeight();
		// calculate the left/top/right/bottom extents of the selection rectangle
		float left = (startPos.x / fWidth - 0.5f) * 2.0f;
		float right = (endPos.x / fWidth - 0.5f) * 2.0f;
		float top = (1.0f - (startPos.y / fHeight) - 0.5f) * 2.0f;
		float bottom = (1.0f - (endPos.y /fHeight) - 0.5f) * 2.0f;

		// and finally, draw the sodding rectangle!
		pRect->setVisible(true);
		pRect->draw(left,top,right,bottom);

		// and update the selection results
		left = startPos.x / fWidth;
		right = endPos.x / fWidth;
		top = startPos.y / fHeight;
		bottom = endPos.y / fHeight;

		if (left > right)
		  swap(left, right);

		if (top > bottom)
		  swap(top, bottom);

		// if our selection rectangle is either a point or a tiny rectangle, then we use a raycast
		if ((right - left) * (bottom - top) > 0.0001)
		{
			performRectSelect(left,top,right,bottom,pView);
		}
	}
	else
	{
		performRaySelect(pView,endPos,true);
	}
	//}

}

void MouseSelectionComponent::performRaySelect(Viewport *pView,Vector2 mousePos,bool bHighlightOnly)
{
   
   // ok, so lets raycast!!!
   // Setup the ray scene query, use UI's mouse pos
   Camera *pCamera = pView->getCamera();
   float fWidth = pView->getActualWidth();
   float fHeight= pView->getActualHeight();
   mouseRay = pCamera->getCameraToViewportRay(mousePos.x/fWidth, mousePos.y/fHeight);
   mRaySceneQuery->setRay(mouseRay);
   mRaySceneQuery->setSortByDistance(true);

   // Execute query
   RaySceneQueryResult &sqresult = mRaySceneQuery->execute();
   RaySceneQueryResult::iterator itr = sqresult.begin( );

	// hack to do floor collision
	Plane floorPlane(Vector3::UNIT_Y,0);

   // Get results, create a node/entity on the position
   for ( itr = sqresult.begin(); itr != sqresult.end(); itr++ )
   {
      if (itr->movable)
      {
         SceneNode *pNode = itr->movable->getParentSceneNode();
			result.movables.clear();
			result.movables.push_back(pNode->getAttachedObject(0));
			

			if(bHighlightOnly && pCurrentNode != pNode)
			{
				SendMessage(pParentGob->getId(),eMSG_HIGHLIGHTCHANGED,0,pNode);
			}

			pCurrentNode = pNode;

			// if we hit anythign other than the ground.. we need to select it
         if(itr->movable->getName() != "GroundEntity" && !bHighlightOnly)
		   {
				SendMessage(pParentGob->getId(),eMSG_SELECTIONLISTCHANGED,0,&result);
				return;
			}
			else
			{
				// we hit the ground, so we dont need to select it
				Vector3 col = mouseRay.getPoint(itr->distance);
				mLastCollision = mouseRay.getPoint((Math::intersects(mouseRay,floorPlane)).second);
			}
			break;
      } // if
   }
}



void MouseSelectionComponent::performRectSelect(float left,float top, float right,float bottom,Viewport *pView)
{

		Camera *pCamera = pView->getCamera();
		Ray topLeft = pCamera->getCameraToViewportRay(left, top);
		Ray topRight = pCamera->getCameraToViewportRay(right, top);
		Ray bottomLeft = pCamera->getCameraToViewportRay(left, bottom);
		Ray bottomRight = pCamera->getCameraToViewportRay(right, bottom);

		PlaneBoundedVolume vol;
		vol.planes.push_back(Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));         // front plane
		vol.planes.push_back(Plane(topLeft.getOrigin(), topLeft.getPoint(100), topRight.getPoint(100)));         // top plane
		vol.planes.push_back(Plane(topLeft.getOrigin(), bottomLeft.getPoint(100), topLeft.getPoint(100)));       // left plane
		vol.planes.push_back(Plane(topLeft.getOrigin(), bottomRight.getPoint(100), bottomLeft.getPoint(100)));   // bottom plane
		vol.planes.push_back(Plane(topLeft.getOrigin(), topRight.getPoint(100), bottomRight.getPoint(100)));     // right plane

		PlaneBoundedVolumeList volList;
		volList.push_back(vol);

		PlaneBoundedVolumeListSceneQuery *mVolQuery;
		mVolQuery = Game::GetInstance().getRenderer()->getSceneManager()->createPlaneBoundedVolumeQuery(PlaneBoundedVolumeList());
		mVolQuery->setVolumes(volList);

		// actually get the rectangle selection
		result = mVolQuery->execute();
		// tell all to update thier sorry asses!
		SendMessage(pParentGob->getId(),eMSG_SELECTIONLISTCHANGED,0,&result);
}


