#pragma once

#include "Protocol.h"
#include "InterServerProtocol.h"

// Windows edition: BackendBridgeReceiver now just proxies responses coming from the TCP bridge
// managed by BackendBridgeClient.
class BackendBridgeReceiver {
private:
    static BackendBridgeReceiver* instance;
    BackendBridgeReceiver() = default;
public:
    static BackendBridgeReceiver* getInstance();
    ReadResult readFromShm();
};
