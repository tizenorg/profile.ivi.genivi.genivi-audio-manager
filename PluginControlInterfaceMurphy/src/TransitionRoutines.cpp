#include <iostream>


#include "TransitionRoutines.h"
#include "Connection.h"

#define ENTER \
    std::cout << sm.getName() << ": " << __FUNCTION__ << "() enter" << std::endl;

#define LEAVE(_v) \
    do { \
        std::cout << sm.getName() << ": " << __FUNCTION__ << "() leave (" \
        << (_v ? "true" : "false") << ")" << std::endl; \
        return _v; \
    } while (0)


using namespace mctl;

/***************************************************************************************
 *                                                                                     *
 *                                  condition functions                                *
 *                                                                                     *
 ***************************************************************************************/
bool mctl::CanPlay(StateMachine& sm, Event& ev)
{
    (void)(ev);
#if 0
    ENTER;

    LEAVE(false);
#else
    ENTER;

    LEAVE(sm.getCanPlay());
#endif
}

bool mctl::CannotPlay(StateMachine& sm, Event& ev)
{
    (void)(ev);

#if 0
    ENTER;

    LEAVE(true);
#else
    ENTER;

    LEAVE(!sm.getCanPlay());
#endif
}

/***************************************************************************************
 *                                                                                     *
 *                                  transition routines                                *
 *                                                                                     *
 ***************************************************************************************/
bool mctl::RouteAndPlaybackRequest(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    ConnectEvent *cev = static_cast<ConnectEvent *>(&ev);

    ENTER;

    NewRouteEvent newrt(cev->route(0));

    conn->queueEvent(newrt);


    LEAVE(true);
}


bool mctl::RouteAndPlaybackRelease(StateMachine& sm, Event& ev)
{
    (void)(ev);

    ENTER;

    LEAVE(true);
}


bool mctl::SavePlaybackGrant(StateMachine& sm, Event& ev)
{
    (void)(ev);

    ENTER;

    LEAVE(true);
}


bool mctl::SetupDomainConnections(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);

    ENTER;

    conn->setupDomainConnections(static_cast<NewRouteEvent&>(ev));

    LEAVE(true);
}


bool mctl::TearDownDomainConnections(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);

    (void)(ev);

    ENTER;

    conn->tearDownDomainConnections();

    LEAVE(true);
}


bool mctl::DomainConnectionIsUp(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    DomainConnUpEvent *upev = static_cast<DomainConnUpEvent *>(&ev);

    ENTER;

    conn->domainConnectionIsUp(*upev);

    bool ready = true;
    for (size_t i = 0;   i < conn->size();   i++) {
        if (conn->domainConnection(i)->getState() != am::CS_CONNECTED) {
            ready = false;
            break;
        }
    }

    if (ready) {
        RouteReadyEvent readyev;
        sm.queueEvent(readyev);
    }

    LEAVE(true);
}


bool mctl::DomainConnectionIsDown(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    DomainConnDownEvent *downev = static_cast<DomainConnDownEvent *>(&ev);

    (void)(ev);

    ENTER;

    conn->domainConnectionIsDown(*downev);

    bool done = true;
    for (size_t i = 0;   i < conn->size();   i++) {
        if (conn->domainConnection(i)->getState() != am::CS_DISCONNECTED) {
            done = false;
            break;
        }
    }

    if (done) {
        RouteDeletedEvent doneev;
        conn->queueEvent(doneev);
    }

    LEAVE(true);
}

bool mctl::MarkPlay(StateMachine& sm, Event& ev)
{
    ENTER;

    sm.setCanPlay(ev.getType() == EVENT_TYPE_PLAY);

    LEAVE(true);
}

bool mctl::FinishMainConnection(StateMachine& sm, Event& ev)
{
    (void)(ev);

    ENTER;

    LEAVE(true);
}


bool mctl::ConnectMainConnection(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    bool success;

    (void)(ev);

    ENTER;

    success = conn->setState(am::CS_CONNECTED);

    LEAVE(success);
}


bool mctl::SuspendMainConnection(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    bool success;

    (void)(ev);

    ENTER;

    success = conn->setState(am::CS_SUSPENDED);

    LEAVE(success);
}


bool mctl::ResumeMainConnection(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    bool success;

    (void)(ev);

    ENTER;

    success = conn->setState(am::CS_CONNECTED);

    LEAVE(success);
}


bool mctl::DisconnectMainConnection(StateMachine& sm, Event& ev)
{
    MainConnection *conn = static_cast<MainConnection *>(&sm);
    bool success;

    (void)(ev);

    ENTER;

    success = conn->setState(am::CS_DISCONNECTED);

    LEAVE(success);
}

bool mctl::FinishAndConnectMainConnection(StateMachine& sm, Event& ev)
{
    return FinishMainConnection(sm, ev) && ConnectMainConnection(sm, ev);
}

bool mctl::FinishAndSuspendMainConnection(StateMachine& sm, Event& ev)
{
    return FinishMainConnection(sm, ev) && SuspendMainConnection(sm, ev);
}


bool mctl::FinishAndDisconnectMainConnection(StateMachine& sm, Event& ev)
{
    return FinishMainConnection(sm, ev) && DisconnectMainConnection(sm, ev);
}
