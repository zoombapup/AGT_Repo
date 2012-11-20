#pragma once

#include "Message.h"

class Game;

class WorldLoader : public MessageListener
{
public:
	WorldLoader(void);
	~WorldLoader(void);

	virtual void HandleCreateSceneMessage(Message *pMsg);
	virtual bool CreateWorldFromFile(const char *szWorldName,Game *pGame);
	virtual bool CreateTestWorld(Game *pGame);
};
