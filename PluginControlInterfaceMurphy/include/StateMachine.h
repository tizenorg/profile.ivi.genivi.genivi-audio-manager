#ifndef __MCTL_STATE_MACHINE_H__
#define __MCTL_STATE_MACHINE_H__

#include "Event.h"

#include <list>


namespace mctl {
    class StateMachine;
    class Transition;

    typedef enum {
        STATE_START = 0,
        STATE_INITIAL_CONNECT,
        STATE_CONNECTING,
        STATE_CONNECTED,
        STATE_SUSPENDED,
        STATE_DISCONNECTING,
        STATE_DISCONNECTED,
        STATE_REROUTE,
        STATE_REMOVE,
        STATE_END,
        STATE_MAX
    } StateId;

    class StateMachine {
    public:
        StateMachine(const char *name);
        ~StateMachine();

        void queueEvent(Event& event);
        const char *getName();
        StateId getStateID();
        void setCanPlay(bool canPlay) { mCanPlay = canPlay; }
        bool getCanPlay() { return mCanPlay; }

        operator const char * (); // prints the state name

        static void freeTransitions();

    private:
        void processEvents();

        std::string mName;
        bool mBusy;
        StateId mStateID;
        bool mCanPlay;
        std::list<Event *> mQueue;

        static Transition *mTransitions[int(STATE_MAX)][int(EVENT_TYPE_MAX)];
    };
}

#endif // __MCTL_STATE_MACHINE_H__
