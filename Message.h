#pragma once
#include <vector>
#include <map>
#include <queue>
#undef SendMessage
#include "FastDelegate.h"


using namespace fastdelegate;

enum MessageType
{
	eMSG_STARTGAME,
	eMSG_SELECTALL,
	eMSG_SELECTNONE,
	eMSG_SELECTIONCHANGED,
	eMSG_HIGHLIGHTCHANGED,
	eMSG_SELECTIONLISTCHANGED,
	eMSG_MOUSEPRESSED,
	eMSG_MOUSEMOVED,
	eMSG_MOUSERELEASED,
	eMSG_SELECTEDMOVETOLOCATION,
	eMSG_MOVETOLOCATION,
	eMSG_MOVECOMPLETE,
	eMSG_CREATETESTSCENE,
	eMSG_ANIMATIONSTART,
	eMSG_ANIMATIONSTOP
};

// the "Message" data type (note: 16 bytes so hopefully we can align it)
// but, as we're storing it in a vector right now, it wont, but we can
// replace the vector once we know the max rate of messages etc.
// TODO: perhaps add some method of knowing how many messages we send?
struct Message
{
	MessageType type;
	unsigned int idSender;
	int iValue;
	void * pData;
};

// the base class for a message handler class. Most classes that do useful things
// will derive from this at some level
class MessageListener
{
public:
	MessageListener();
	~MessageListener();
	virtual bool HandleMessage(Message *pMessage);
   void SendMessage(unsigned int idSender,MessageType msgType,int iValue,void *pData);
};

typedef FastDelegate1<Message *> MessageDelegate;


// the global singleton message relay manager system doobry
class MessageManager 
{
	
	std::vector<MessageDelegate> mDelegates;
	std::multimap<MessageType,FastDelegate1<Message*> *> mSubscribers;

   typedef std::multimap<MessageType,FastDelegate1<Message*> *>::iterator mapIterator;
   std::queue<Message> mStoredMessages;
	MessageManager();
	~MessageManager();
public:
	static MessageManager& GetInstance() { static MessageManager theMessageManager; return theMessageManager; };
	MessageDelegate* SubscribeMessage(MessageType msg);
	void UnSubscribeMessage(MessageType msg, void *pSubscriber);
   void SendMessage(unsigned int idSender,MessageType msgType,int iValue,void *pData);
	void SendToSubscribers(Message *pMsg);
	void PrintMessageInfo(const char *logtext,Message *pMsg);

};

