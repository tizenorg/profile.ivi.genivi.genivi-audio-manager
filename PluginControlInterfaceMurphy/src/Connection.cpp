#include <iostream>

#include "Connection.h"


using namespace mctl;


std::map<am::am_mainConnectionID_t, MainConnection *>  MainConnection::mIdMap;
std::map<am::am_connectionID_t, DomainConnection *>  DomainConnection::mIdMap;
std::map<uint16_t, DomainConnection *> DomainConnection::mHandleMap;

DomainConnection::DomainConnection(MainConnection *mainConnection,
                                   am::am_RoutingElement_s& routingElement):
    mMainConnection(mainConnection),
    mConnectionID(0),
    mDomainID(routingElement.domainID),
    mSourceID(routingElement.sourceID),
    mSinkID(routingElement.sinkID),
    mConnectionFormat(routingElement.connectionFormat),
    mState(am::CS_UNKNOWN)
{
    std::cout << mainConnection->getName() << ": setting up domain connection "
              << mSourceID << "->" << mSinkID
              << std::endl;

    mHandle.handleType = am::H_UNKNOWN;
    mHandle.handle = 0;

    if (connect())
        mIdMap[mConnectionID] = this;
}

DomainConnection::~DomainConnection()
{
    uint16_t handle = getHandle(mHandle);

    mIdMap.erase(mConnectionID);

    std::cout << mMainConnection->getName() << ": destroying connection "
              << mConnectionID << " ("<< mSourceID << "->" << mSinkID << ")"
              << std::endl;

    if (handle > 0) {
        mMainConnection->mReceiver->abortAction(mHandle);
        mHandleMap.erase(handle);
    }


    if (mConnectionID > 0) {
        disconnect();
    }
}

uint16_t DomainConnection::getHandle(const am::am_Handle_s& h)
{
    if (h.handle > 0) {
        return (uint16_t(h.handleType) & 0x003f) << 10 |
               (uint16_t(h.handle) & 0x03ff);
    }

    return 0;
}

am::am_ConnectionState_e DomainConnection::getState()
{
    return mState;
}


bool DomainConnection::operator ==(DomainConnection& dcon)
{
    return mConnectionID == dcon.mConnectionID;
}


bool DomainConnection::connect()
{
    am::IAmControlReceive *receiver = mMainConnection->mReceiver;
    bool success = true;

    if (mState != am::CS_UNKNOWN) {
        std::cout << mMainConnection->getName()
                  << "subsequent connection request for connection "
                  << mConnectionID << " (" << mSourceID << "->" << mSinkID << ")"
                  << std::endl;
        success = false;
    }
    else {
        am::am_Error_e err = receiver->connect(mHandle, mConnectionID,
                                               mConnectionFormat,
                                               mSourceID, mSinkID);
        if (err != am::E_OK) {
            std::cout << mMainConnection->getName() << ": failed to connect ("
                      << mSourceID << "->" << mSinkID << ")"
                      << std::endl;

            mState = am::CS_UNKNOWN;
            mHandle.handle = 0;
            mConnectionID = 0;

            success = false;
        }
        else {
            mState = am::CS_CONNECTING;
            mHandleMap[getHandle(mHandle)] = this;
        }
    }

    return success;
}

bool DomainConnection::disconnect()
{
    am::IAmControlReceive *receiver = mMainConnection->mReceiver;
    bool success = true;
    uint16_t handle = getHandle(mHandle);

    if (handle > 0) {
        std::cout << mMainConnection->getName()
                  << ": aborting pending operation on connection "
                  << mConnectionID << "(" << mSourceID << "->" << mSinkID << ")"
                  << std::endl;
        receiver->abortAction(mHandle);
        mHandle.handle = 0;
        mHandleMap.erase(handle);
    }

    if (mConnectionID > 0 && mState == am::CS_CONNECTED) {
        std::cout << mMainConnection->getName() << ": disconnecting connection "
                  << mConnectionID << "(" << mSourceID << "->" << mSinkID << ")"
                  << std::endl;
        am::am_Error_e err = receiver->disconnect(mHandle, mConnectionID);

        if (err != am::E_OK) {
            std::cout << mMainConnection->getName()
                      << "disconnect request failed: error " << err
                      << std::endl;

            if (mConnectionID > 0)
                mIdMap.erase(mConnectionID);

            mState = am::CS_UNKNOWN;
            mHandle.handle = 0;
            mConnectionID = 0;

            success = false;
        }
        else {
            mState = am::CS_DISCONNECTING;
            mHandleMap[getHandle(mHandle)] = this;
        }
    }

    return success;
}


void DomainConnection::connectionIsUp()
{
    if (mState != am::CS_CONNECTING) {
        std::cout << mMainConnection->getName() << ": attempt to finalise connection "
                  << mConnectionID << " when not in connecting state" << std::endl;
    }
    else {
        mState = am::CS_CONNECTED;

        std::cout << mMainConnection->getName() << ": connection "
                  << mConnectionID << " is up" << std::endl;
    }

    if (mHandle.handle > 0) {
        mHandleMap.erase(getHandle(mHandle));
        mHandle.handle = 0;
    }
}

void DomainConnection::connectionIsDown()
{
    if (mState != am::CS_DISCONNECTING) {
        std::cout << mMainConnection->getName() << ": attempt to dismantle connection "
                  << mConnectionID << " when not in disconnecting state" << std::endl;
    }
    else {
        mState = am::CS_DISCONNECTED;

        std::cout << mMainConnection->getName() << ": connection "
                  << mConnectionID << " is down" << std::endl;
    }

    if (mHandle.handle > 0) {
        mHandleMap.erase(getHandle(mHandle));
        mHandle.handle = 0;
    }
}

bool MainConnection::setState(const am::am_ConnectionState_e state)
{
    am::am_Error_e err = mReceiver->changeMainConnectionStateDB(mMainConnectionID, state);

    if (err != am::E_OK) {
        std::cout << getName() << ": failed to set state " << state << std::endl;
        return false;
    }

    return true;
}


DomainConnection *DomainConnection::findDomainConnection(const am::am_connectionID_t connectionID)
{
    return mIdMap[connectionID];
}

DomainConnection *DomainConnection::findDomainConnection(const am::am_Handle_s& handle)
{
    uint16_t h = getHandle(handle);

    if (h > 0)
        return mHandleMap[h];

    return NULL;
}


MainConnection::MainConnection(const char *name,
                               am::IAmControlSend *sender,
                               am::IAmControlReceive *receiver,
                               const am::am_sourceID_t mainSourceID,
                               const am::am_sinkID_t mainSinkID,
                               uint32_t rsetID) :
    StateMachine(name),
    mSender(sender),
    mReceiver(receiver),
    mMainSourceID(mainSourceID),
    mMainSinkID(mainSinkID),
    mRsetID(rsetID),
    mMainConnectionID(0)
{
    am::am_MainConnection_s connData;

    connData.mainConnectionID = 0;
    connData.sinkID = mMainSinkID;
    connData.sourceID = mMainSourceID;
    connData.connectionState = am::CS_CONNECTING;
    connData.delay = 0;

    mReceiver->enterMainConnectionDB(connData, mMainConnectionID);

    mIdMap[mMainConnectionID] = this;
}


MainConnection::~MainConnection()
{
    std::cout << getName() << ": main connection " << mMainConnectionID
              << " is going to be destroyed" << std::endl;

    mReceiver->removeMainConnectionDB(mMainConnectionID);

    for (size_t i = 0;  i < mDomainConnections.size();   i++)
        delete mDomainConnections[i];

    mIdMap.erase(mMainConnectionID);
}


am::am_mainConnectionID_t MainConnection::getID()
{
    return mMainConnectionID;
}

size_t MainConnection::size()
{
    return mDomainConnections.size();
}

DomainConnection * MainConnection::domainConnection(const size_t index)
{
    if (index < mDomainConnections.size())
        return mDomainConnections[index];

    return NULL;
}

DomainConnection * MainConnection::operator [](const size_t index)
{
    return domainConnection(index);
}

bool MainConnection::setupDomainConnections(NewRouteEvent& ev)
{
    if (!mDomainConnections.empty()) {
        std::cout << __FUNCTION__ << "(): refuse to deal with NewRoute request for existing main connection" << std::endl;
        return false;
    }

    for (size_t i = 0;  i < ev.size();   i++)
        mDomainConnections.push_back(new DomainConnection(this, ev[i]));

    return true;
}

bool MainConnection::domainConnectionIsUp(DomainConnUpEvent& ev)
{
    DomainConnection *domainConnection = DomainConnection::findDomainConnection(ev.getHandle());

    domainConnection->connectionIsUp();

    return true;
}


bool MainConnection::tearDownDomainConnections()
{
    if (mDomainConnections.empty()) {
        std::cout << __FUNCTION__ << "(): refuse to deal with teardown request for main connection" << std::endl;
        return false;
    }

    for (size_t i = 0;  i < mDomainConnections.size();   i++)
        mDomainConnections[i]->disconnect();

    return true;
}

bool MainConnection::domainConnectionIsDown(DomainConnDownEvent& ev)
{
    DomainConnection *domainConnection = DomainConnection::findDomainConnection(ev.getHandle());

    std::vector<DomainConnection *>::iterator it = mDomainConnections.begin();
    for (;  it != mDomainConnections.end();  it++) {
        if (*domainConnection == **it) {
            (*it)->connectionIsDown();
            mDomainConnections.erase(it);
            delete domainConnection;
            return true;
        }
    }

    return false;
}


MainConnection *MainConnection::findMainConnection(const am::am_mainConnectionID_t mainConnectionID)
{
    return mIdMap[mainConnectionID];
}

MainConnection *MainConnection::findMainConnection(const am::am_Handle_s& handle)
{
    DomainConnection *domainConnection = DomainConnection::findDomainConnection(handle);

    if (domainConnection != NULL)
        return domainConnection->mMainConnection;

    return NULL;
}
