#include "BackendBridgeReceiver.h"
#include "BackendBridgeClient.h"

BackendBridgeReceiver* BackendBridgeReceiver::instance = nullptr;

BackendBridgeReceiver* BackendBridgeReceiver::getInstance()
{
    if (instance == nullptr) {
        instance = new BackendBridgeReceiver();
    }
    return instance;
}

ReadResult BackendBridgeReceiver::readFromShm()
{
    return BackendBridgeClient::getInstance()->waitForResponse();
}
