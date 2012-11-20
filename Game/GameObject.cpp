#include ".\gameobject.h"
#include ".\gamecomponent.h"
#include <stdio.h>
unsigned int GameObject::sId = 0;

GameObject::GameObject(Vector3 pos, Quaternion rot,const char *name)
{
	mComponents.clear();
	mPos = pos; 
	mRot = rot;
	id = ++sId; // unique id
	name = name;
	std::ostringstream st;
	st << id;
	std::string strId = st.str();
	uniquename = name;
	uniquename += strId;
}

GameObject::~GameObject(void)
{
	// TODO: iterate over any game components and delete them from the list
	// then clear the list

   /* PC: wanted to test a couple of alternative delete methods
	while (!mComponents.empty())
	{
      GameComponent *pCom = (*mComponents.back());
      mComponents.pop_back();
      delete pCom;
	}
   */
	std::vector<GameComponent *>::iterator it;
	for (it = mComponents.begin(); it != mComponents.end(); it++)
	{
		GameComponent *pCom = (*it);
		delete pCom;
	}
   // finally, remove the contents
   mComponents.clear();

}

void GameObject::RemoveComponent(GameComponent *pComponent)
{
	std::vector<GameComponent *>::iterator it;
	for (it = mComponents.begin(); it != mComponents.end(); it++)
	{
		GameComponent *pCom = (*it);
		if(pCom == pComponent)
		{
			delete pCom;
			mComponents.erase(it);
			return;
		}
	}
	
};

// game object updates basically are just iterations over component updates
void GameObject::Update(float fTime,unsigned int iTime)
{
	std::vector<GameComponent *>::iterator it;
	for (it = mComponents.begin(); it != mComponents.end(); it++)
	{
		//(*it)->Update(fTime,iTime);
		GameComponent *pCom = (*it);
		if(pCom)
		{
			pCom->Update(fTime,iTime);
		}

	}
};

bool GameObject::GetComponentByType(GameComponent::ComponentType type, GameComponent **ppCom)
{
	std::vector<GameComponent *>::iterator it;
	for (it = mComponents.begin(); it != mComponents.end(); it++)
	{
		GameComponent *pCom = (*it);
		if(pCom)
		{
			if(pCom->getType() == type)
			{
				// return the pointer to our component
				*ppCom = pCom;
				return true;
			}
		}

	}
	return false;
}

// adds a "component" to the list of this game objects components
// TODO: this should check to see if the component can actually be added 
// for instance, what if another component is already in the list??
bool GameObject::AddComponent(GameComponent *pComponent)
{
	// TODO: add asserts here.. for now we do the risky thing!!
	mComponents.push_back(pComponent);
	// component should really have a pointer to us.. TODO: fix something better here
	pComponent->setParent(this);

	return true;
}