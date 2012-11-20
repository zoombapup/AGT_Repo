#include "Message.h"
#include "Ogre.h"
using namespace Ogre;
#undef SendMessage

MessageListener::MessageListener(void)
{
}

MessageListener::~MessageListener(void)
{
}

bool MessageListener::HandleMessage(Message *pMessage)
{
	return true;
};

// send a message (simply pass through to the MessageManager)
void MessageListener::SendMessage(unsigned int idSender,MessageType msgType,int iValue,void *pData)
{
   MessageManager::GetInstance().SendMessage(idSender,msgType,iValue,pData);
}

MessageManager::MessageManager(void)
{
   mDelegates.reserve(500);
}

MessageManager::~MessageManager(void)
{
}


MessageDelegate* MessageManager::SubscribeMessage(MessageType msg)
{
	//mDelegates.push_back(*d);
	MessageDelegate *pDelegate = new MessageDelegate;
   mSubscribers.insert(std::pair<MessageType,FastDelegate1<Message*> *>(msg,pDelegate));
	return pDelegate;
};

void MessageManager::UnSubscribeMessage(MessageType msg,void *pSubscriber)
{

};

void MessageManager::PrintMessageInfo(const char *logtext,Message *pMsg)
{
		String str(logtext);
		StringConverter conv;
		str += " type: ";
		str += conv.toString(pMsg->type);
		str += " sender: ";
		str += conv.toString(pMsg->idSender);
		str += " value: ";
		str += conv.toString(pMsg->iValue);
		str += " data: ";
		str += conv.toString((int)pMsg->pData);
		LogManager::getSingleton().logMessage(str);
}

// sends a message to all subscribers for a given message type.
// handles re-entrancy of message sending (probably quite badly right now)
// TODO: Create a proper crit section class for this..
void MessageManager::SendMessage(unsigned int idSender,MessageType msgType,int iValue,void *pData)
{
	static bool bIsSending = false;

	Message msg;
	msg.idSender = idSender;
	msg.type = msgType;
	msg.iValue = iValue;
	msg.pData = pData;

	// use bIsSending as a sort of mutex
	if(bIsSending)
	{
		// sending already, so lets throw the message into a buffer and return
		mStoredMessages.push(msg);
		String str("Storing Message: ");
		StringConverter conv;
		str += conv.toString(mStoredMessages.size());
		str += " type: ";
		str += conv.toString(msg.type);
		LogManager::getSingleton().getDefaultLog()->setDebugOutputEnabled(true);
		LogManager::getSingleton().logMessage(str);

		return;
	}

	// we are REALLY sending.. use this as a critical section
	bIsSending = true;

	// send the message to all subscribers
	SendToSubscribers(&msg);

	bIsSending = false;
	unsigned int size = (unsigned int)mStoredMessages.size();
	// and then transmit any stored messages
	while ( !mStoredMessages.empty() )
	{
		Message *pMsg = &mStoredMessages.front();
		mStoredMessages.pop();
		PrintMessageInfo("Sending Stored Message: ",pMsg);
		// because sending can cause more messages so we need to crit sec this too..
		bIsSending = true;
		SendToSubscribers(pMsg);	
		bIsSending = false;
	}
};


void MessageManager::SendToSubscribers(Message *pMsg)
{
	std::pair<mapIterator,mapIterator> range;
	range = mSubscribers.equal_range(pMsg->type);
	for(mapIterator it = range.first; it != range.second; ++it)
	{
		(it->second)->operator ()(pMsg);
	}
}