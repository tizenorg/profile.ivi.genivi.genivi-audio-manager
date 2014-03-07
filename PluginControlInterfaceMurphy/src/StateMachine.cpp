#include <iostream>
#include <list>

#include "StateMachine.h"
#include "TransitionRoutines.h"

using namespace mctl;

#define TRANSITION      static_cast<Transition *>

#define IGNORE          TRANSITION(new NoTransition())
#define GOTO(_s)        TRANSITION(new DirectTransition(STATE_ ## _s))
#define TRANSIT(_s,_f)  TRANSITION(new BasicTransition(STATE_ ## _s, _f, # _f))
#define CONDITIONAL(_a) TRANSITION(new ConditionalTransition(_a))

#if 0
#define ALTERNATIVE_LIST(_n, ... )                                                \
    static AlternativeTransition ListOf ## _n[] = {                               \
        __VA_ARGS__                                                               \
    };                                                                            \
    static std::list<AlternativeTransition> _n(ListOf ## _n, ListOf ## _n +       \
                                 (sizeof(ListOf ## _n) / sizeof(ListOf ## _n [0])))

#define ALTERNATIVE(_c, _s, _f) AlternativeTransition(_c, STATE_ ## _s, _f, # _f)
#endif

#define ALTERNATIVE_LIST(_n, ... )                                                \
    static AlternativeTransition *ListOf ## _n[] = {                              \
        __VA_ARGS__                                                               \
    };                                                                            \
    static std::list<AlternativeTransition *> *_n =                               \
        new std::list<AlternativeTransition *> (ListOf ## _n, ListOf ## _n +      \
                                 (sizeof(ListOf ## _n) / sizeof(ListOf ## _n [0])))

#define ALTERNATIVE(_c, _s, _f) new AlternativeTransition(_c, STATE_ ## _s, _f, # _f)


//--------------------------- Statemachine definition -----------------------------

ALTERNATIVE_LIST (RouteReadyAlternatives,
    ALTERNATIVE (CanPlay   , CONNECTED, FinishAndConnectMainConnection),
    ALTERNATIVE (CannotPlay, SUSPENDED, FinishAndSuspendMainConnection)
);


Transition *StateMachine::mTransitions [int(STATE_MAX)] [int(EVENT_TYPE_MAX)] = {
    //-------------------------
    // State START
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ TRANSIT      (INITIAL_CONNECT, RouteAndPlaybackRequest)       ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State INITIAL_CONNECT
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ TRANSIT      (END, RouteAndPlaybackRelease)                   ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ TRANSIT      (CONNECTING, SetupDomainConnections)             ,
        /* NO_ROUTE         */ GOTO         (DISCONNECTED)                                   ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State CONNECTING
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ TRANSIT      (REMOVE, TearDownDomainConnections)              ,
        /* DOMAIN_CONN_UP   */ TRANSIT      (CONNECTING, DomainConnectionIsUp)               ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ TRANSIT      (CONNECTING, MarkPlay)                           ,
        /* STOP             */ TRANSIT      (CONNECTING, MarkPlay)                           ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ TRANSIT      (DISCONNECTED, FinishAndDisconnectMainConnection),
        /* ROUTE_READY      */ CONDITIONAL  (RouteReadyAlternatives)                         ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State CONNECTED
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ TRANSIT      (REMOVE, TearDownDomainConnections)              ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ TRANSIT      (SUSPENDED, SuspendMainConnection)               ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State SUSPENDED
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ TRANSIT      (REMOVE, TearDownDomainConnections)              ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ TRANSIT      (CONNECTED, ResumeMainConnection)                ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ TRANSIT      (REROUTE, TearDownDomainConnections)             ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State DISCONNECTING
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State DISCONNECTED
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State REROUTE
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ TRANSIT     (REROUTE, DomainConnectionIsDown)                 ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ TRANSIT      (CONNECTING, SetupDomainConnections)             ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State REMOVE
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ TRANSIT     (REMOVE, DomainConnectionIsDown)                  ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ TRANSIT     (END, RouteAndPlaybackRelease)                    ,
        /* DONE             */ IGNORE                                                        ,
    },

    //-------------------------
    // State END
    //-------------------------
    {   /* Events:          */
        /* CONNECT          */ IGNORE                                                        ,
        /* REMOVE           */ IGNORE                                                        ,
        /* DOMAIN_CONN_UP   */ IGNORE                                                        ,
        /* DOMAIN_CONN_DOWN */ IGNORE                                                        ,
        /* PLAY             */ IGNORE                                                        ,
        /* STOP             */ IGNORE                                                        ,
        /* NEW_ROUTE        */ IGNORE                                                        ,
        /* NO_ROUTE         */ IGNORE                                                        ,
        /* ROUTE_READY      */ IGNORE                                                        ,
        /* ROUTE_DELETED    */ IGNORE                                                        ,
        /* DONE             */ IGNORE                                                        ,
    }
};

//---------------------------------------------------------------------------------

#undef ALTERNATIVE
#undef ALTERNATIVE_LIST

#undef CONDITIONAL
#undef TRANSIT
#undef GOTO
#undef IGNORE

#undef TRANSITION



StateMachine::StateMachine(const char *name):
    mName(name),
    mBusy(false),
    mStateID(STATE_START),
    mCanPlay(false)
{
}


StateMachine::~StateMachine()
{
}


void StateMachine::queueEvent(Event& ev)
{
    Event *dup;

    std::cout << mName << ": queue event " << ev << std::endl;

#define DUP(_t,_e) static_cast<Event *>(new _t ## Event(* static_cast<_t ## Event *>(&_e)))

    switch (ev.getType()) {
    case EVENT_TYPE_CONNECT:          dup = DUP (Connect       , ev);  break;
    case EVENT_TYPE_REMOVE:           dup = DUP (Remove        , ev);  break;
    case EVENT_TYPE_DOMAIN_CONN_UP:   dup = DUP (DomainConnUp  , ev);  break;
    case EVENT_TYPE_DOMAIN_CONN_DOWN: dup = DUP (DomainConnDown, ev);  break;
    case EVENT_TYPE_PLAY:             dup = DUP (Play          , ev);  break;
    case EVENT_TYPE_STOP:             dup = DUP (Stop          , ev);  break;
    case EVENT_TYPE_NEW_ROUTE:        dup = DUP (NewRoute      , ev);  break;
    case EVENT_TYPE_NO_ROUTE:         dup = DUP (NoRoute       , ev);  break;
    case EVENT_TYPE_ROUTE_READY:      dup = DUP (RouteReady    , ev);  break;
    case EVENT_TYPE_ROUTE_DELETED:    dup = DUP (RouteDeleted  , ev);  break;
    case EVENT_TYPE_DONE:             dup = DUP (Done          , ev);  break;
    default:
        std::cout << mName << ": refuse to queue unsupported event type "
                  << ev.getType() << std::endl;
        return;
    }

#undef DUP

    mQueue.push_back(dup);
    processEvents();
}

void StateMachine::processEvents()
{
    Transition *transition;

    if (!mBusy) {
        while (!mQueue.empty()) {
            StateId oldStateID = mStateID;

            Event *ev = mQueue.front();
            mQueue.pop_front();

            transition = mTransitions [int(mStateID)] [int(ev->getType())];

            std::cout << mName << ": handling event '" << *ev
                      << " in '" << *this << "' state. "
                      << *transition << std::endl;

            mBusy = true;
            mStateID = transition->function(*this, *ev);
            mBusy = false;

            if (oldStateID == mStateID) {
                std::cout << mName << ": stayed in '" << *this << "' state"
                          << std::endl;
            }
            else {
                std::cout << mName << ": transited to new state '"
                          << *this << "'" << std::endl;
            }

            delete ev;
        }
    }
}

const char *StateMachine::getName()
{
    return mName.c_str();
}

StateId StateMachine::getStateID()
{
    return mStateID;
}


StateMachine::operator const char * ()
{
    switch (mStateID) {
    case STATE_START:               return "Start";
    case STATE_INITIAL_CONNECT:     return "InitialConnect";
    case STATE_CONNECTING:          return "Connecting";
    case STATE_CONNECTED:           return "Connected";
    case STATE_SUSPENDED:           return "Suspended";
    case STATE_DISCONNECTING:       return "Disconnecting";
    case STATE_DISCONNECTED:        return "Disconnected";
    case STATE_REROUTE:             return "Reroute";
    case STATE_REMOVE:              return "Remove";
    case STATE_END:                 return "End";
    default:                        return "<unknown>";
    }
}

void StateMachine::freeTransitions()
{
    int i,j;

    std::cout << "Freeing state machine" << std::endl;

    for (i = 0;  i < int(STATE_MAX);  i++) {
        for (j = 0;  j < int(EVENT_TYPE_MAX); j++) {
            delete mTransitions[i][j];
        }
    }
}
