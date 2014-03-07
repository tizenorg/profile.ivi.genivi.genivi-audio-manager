#ifndef __MCTL_CONNECTION_H__
#define __MCTL_CONNECTION_H__

#include <control/IAmControlSend.h>
#include <control/IAmControlReceive.h>

#include <map>
#include <vector>

#include "StateMachine.h"


namespace mctl {
    class MainConnection;

    class DomainConnection {
        friend class MainConnection;

    public:
        virtual ~DomainConnection();

        am::am_ConnectionState_e getState();

        static DomainConnection *findDomainConnection(const am::am_connectionID_t connectionID);

    private:
        DomainConnection(MainConnection *mainConnection,
                         am::am_RoutingElement_s& routingElement);
        bool connect();
        bool disconnect();

        void connectionIsUp();
        void connectionIsDown();

        bool operator ==(DomainConnection& dconn);

        MainConnection *mMainConnection;
        am::am_connectionID_t mConnectionID;
        am::am_Handle_s mHandle;
        am::am_domainID_t mDomainID;
        am::am_sourceID_t mSourceID;
        am::am_sinkID_t mSinkID;
        am::am_ConnectionFormat_e mConnectionFormat;
        am::am_ConnectionState_e mState;

        static uint16_t getHandle(const am::am_Handle_s& handle);
        static DomainConnection *findDomainConnection(const am::am_Handle_s& handle);

        static std::map<am::am_connectionID_t, DomainConnection *> mIdMap;
        static std::map<uint16_t, DomainConnection *> mHandleMap;
    };

    class MainConnection: public StateMachine {
        friend class DomainConnection;

    public:
        MainConnection(const char *name,
                       am::IAmControlSend *mSender,
                       am::IAmControlReceive *mReceiver,
                       am::am_sourceID_t mainSourceID,
                       am::am_sinkID_t mainSinkID,
                       uint32_t rsetID = 0);
        virtual ~MainConnection();

        am::am_mainConnectionID_t getID();

        size_t size();
        DomainConnection *domainConnection(const size_t index);
        DomainConnection *operator [](const size_t index);

        bool setupDomainConnections(NewRouteEvent& ev);
        bool domainConnectionIsUp(DomainConnUpEvent& ev);
        bool tearDownDomainConnections();
        bool domainConnectionIsDown(DomainConnDownEvent& ev);

        bool setState(const am::am_ConnectionState_e state);

        static MainConnection *findMainConnection(const am::am_mainConnectionID_t mainConnectionID);
        static MainConnection *findMainConnection(const am::am_Handle_s& handle);

        // These can be dug out also using size() and [], but still...
        am::am_sourceID_t mainSourceID() { return mMainSourceID; }
        am::am_sinkID_t mainSinkID() { return mMainSinkID; }

        uint32_t rsetID() { return mRsetID; }
        void setRoute(am::am_Route_s &route) { mRoute = route; }
        void getRoute(am::am_Route_s &route) { route = mRoute; }

    private:
        am::IAmControlSend *mSender;
        am::IAmControlReceive *mReceiver;

        am::am_sourceID_t mMainSourceID;
        am::am_sinkID_t mMainSinkID;
        uint32_t mRsetID;
        am::am_mainConnectionID_t mMainConnectionID;
        am::am_Route_s mRoute;

        std::vector<DomainConnection *> mDomainConnections;

        static std::map<am::am_mainConnectionID_t, MainConnection *>  mIdMap;
    };
}

#endif // __MCTL_CONNECTION_H__
