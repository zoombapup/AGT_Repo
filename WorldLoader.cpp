#include "WorldLoader.h"
#include "Game.h"
#include "Renderer.h"
#include ".\game\GameObject.h"
#include ".\game\GameComponent.h"

WorldLoader::WorldLoader(void)
{
	// subscribe world loader messages
	MessageManager::GetInstance().SubscribeMessage(eMSG_CREATETESTSCENE)->bind(this,&WorldLoader::HandleCreateSceneMessage);
}

WorldLoader::~WorldLoader(void)
{
}

void WorldLoader::HandleCreateSceneMessage(Message *pMsg)
{
	switch(pMsg->type)
	{
	case eMSG_CREATETESTSCENE:
		CreateTestWorld(&Game::GetInstance());
	default:	break;

	}
}


bool WorldLoader::CreateWorldFromFile(const char *szWorldName, Game *pGame)
{
	
	return false;
};

void CreateRandomHouse(Game *pGame)
{
	Vector3 pos = pGame->getRandomVector(10000.0f);
	pos.y += 550.0f;
	GameObject *pHouse = pGame->CreateObject(pos,Quaternion(pos.x,0,0,0),"testent",eStaticMeshObject,false);
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("tudorhouse.mesh",pHouse);
	pHouse->AddComponent(pORC);
	pGame->AddGob(pHouse);
}

void CreateArena(Game *pGame)
{
	Vector3 pos;
	pos.x = 0; pos.y = -8; pos.z = 0;
	GameObject *pArena = pGame->CreateObject(pos,Quaternion(pos.x,0,0,0),"testent",eStaticMeshObject,false);
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("Arena.mesh",pArena);
	pORC->SetCollision(false);
	pArena->AddComponent(pORC);
	pGame->AddGob(pArena);
}

void CreateRandomBarrel(Game *pGame)
{
	Vector3 pos = pGame->getRandomVector(4000.0f);
	GameObject *pHouse = pGame->CreateObject(pos,Quaternion(pos.x,0,0,0),"testent",eStaticMeshObject,false);
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("Barrel.mesh",pHouse);
	pHouse->AddComponent(pORC);
	pGame->AddGob(pHouse);
}

void CreateRandomBox(Game *pGame)
{
	Vector3 pos = pGame->getRandomVector(4000.0f);
	GameObject *pHouse = pGame->CreateObject(pos,Quaternion(pos.x,0,0,0),"testent",eStaticMeshObject,false);
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("twentyfiveunitpyramid.mesh",pHouse,true);
	//pORC->getSceneNode()->setScale(3,3,3);
	pHouse->AddComponent(pORC);
	pGame->AddGob(pHouse);
}

void CreateCameraBox(Game *pGame)
{
	Vector3 pos;
	pos.x = 0; pos.y = 0; pos.z = 0;
	GameObject *pObj = pGame->CreateObject(pos,Quaternion(pos.x,0,0,0),"testent",eStaticMeshObject,false);
	// render mesh component
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("Box01.mesh",pObj,true);
	pObj->AddComponent(pORC);
	// animation handler component
	AnimationComponent *pAnim = dynamic_cast<AnimationComponent *>(pGame->CreateComponent(GameComponent::eCom_SimpleAnimation));
	pAnim->setParent(pObj);
	pAnim->Init(0.001f);
	pObj->AddComponent(pAnim);

	pGame->AddGob(pObj);

	// path controller component
	PathCameraComponent *pPath = dynamic_cast<PathCameraComponent *>(pGame->CreateComponent(GameComponent::eCom_PathCameraController));
	if(pPath)
	{
		pObj->AddComponent(pPath);
		pPath->Init();
	}

}

void CreateRandomRobot(Game *pGame)
{
	Vector3 pos = pGame->getRandomVector(2000.0f);
	
	GameObject *pRobot = pGame->CreateObject(pos,Quaternion(1,0,0,0),"testent",eStaticMeshObject,false);
	OgreRenderInstanceComponent *pORC = dynamic_cast<OgreRenderInstanceComponent *>(pGame->CreateComponent(GameComponent::eCom_Render3D));
	if (pORC) pORC->Create("robot.mesh",pRobot,true);
	pRobot->AddComponent(pORC);
	MoverComponent *pMover = dynamic_cast<MoverComponent *>(pGame->CreateComponent(GameComponent::eCom_SimpleMover));
	pMover->setParent(pRobot);
	pMover->Init(1000.0f,70.0f,40.0f,pos,1.5f,true);
	pRobot->AddComponent(pMover);
	AnimationComponent *pAnim = dynamic_cast<AnimationComponent *>(pGame->CreateComponent(GameComponent::eCom_SimpleAnimation));
	pAnim->setParent(pRobot);
	pAnim->Init(0.001f);
	pRobot->AddComponent(pAnim);

	
	// a selector component, this allows a gameobject to know wether its selected or not
   SelectionComponent *pSelect = dynamic_cast<SelectionComponent *>(pGame->CreateComponent(GameComponent::eCom_SimpleSelection));
   pSelect->setParent(pRobot);
   pSelect->Init();
   pRobot->AddComponent(pSelect);
	
	pGame->AddGob(pRobot);
}

bool WorldLoader::CreateTestWorld(Game *pGame)
{
	SceneManager * pSceneMgr = pGame->getRenderer()->getSceneManager();
 // PC: Note, ogre likes the shadow setup to happen first, because it affects the model loading?? weird.. check the manual
      // setup the shadow technique to use.. stencil does nice self-shadowing but its a bit slow.
      pSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
      //mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
      //pSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
      //mSceneMgr->setShadowTextureSelfShadow(true);
      //pSceneMgr->setShadowTextureSize(1024);
      //mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
      /*
      LiSPSMShadowCameraSetup* lispsm = new LiSPSMShadowCameraSetup();
		lispsm->setOptimalAdjustFactor(0.3);
		mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(lispsm));
		mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
      */
	  srand(1027);
#ifdef _DEBUG
	  for(int i = 0; i < 20; i++)
#else
	  for(int i = 0; i < 2000; i++)
#endif
	  {
		CreateRandomRobot(pGame);
	  }
	  for(int i = 0; i < 4; i++)
	  {
		//CreateRandomBox(pGame);
	  }
		//CreateArena(pGame);
		//CreateCameraBox(pGame);
/*
		for(int i = 0; i < 40; i++)
	  {
		CreateRandomHouse(pGame);
	  }

		for(int i = 0; i < 40; i++)
	  {
		CreateRandomBarrel(pGame);
	  }
*/

#ifndef _TERRAIN_
      

      Plane plane(Vector3::UNIT_Y,0);
      MeshManager::getSingleton().createPlane("ground",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
         plane,150000,150000,20,20,true,1,500,500,Vector3::UNIT_Z);

      Entity *ent = pSceneMgr->createEntity("GroundEntity", "ground");
      SceneNode* planeNode = pSceneMgr->getRootSceneNode()->createChildSceneNode();
      planeNode->attachObject(ent);
      //ent->setMaterialName("Examples/GrassFloor");
		ent->setMaterialName("PAC/Floor");
	  //ent->setMaterialName("PAC/Floor");
      ent->setCastShadows(false);
     
#else
      pSceneMgr->setWorldGeometry("terrain.cfg");
#endif


	  Viewport * pView  = pGame->getRenderer()->getRenderWindow()->getViewport(0);
      pView->setBackgroundColour(ColourValue(0.3f,0.3f,0.3f,1.0f));

      //pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
      //pSceneMgr->setSkyBox(true,"Examples/CloudyNoonSkyBox",50);
      



      // Set ambient light
      pSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

      // Create a light
      Light* l = pSceneMgr->createLight("MainLight");
      l->setType(Light::LightTypes::LT_DIRECTIONAL);
      Vector3 lightpos(20,140,50);
      Vector3 lightspot(0,0,0);
      Vector3 lightdir = lightspot - lightpos;
      l->setDirection(lightdir);
      l->setPosition(lightpos);

	return true;
};
