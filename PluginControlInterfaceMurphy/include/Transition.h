#ifndef __MCTL_TRANSITION_H__
#define __MCTL_TRANSITION_H__

#include <stdbool.h>

#include <list>

#include "StateMachine.h"

namespace mctl {
    typedef bool (*TransitionFunction)(StateMachine& sm, Event& ev);
    typedef bool (*ConditionFunction)(StateMachine& sm, Event& ev);

    class Transition {
    public:
        virtual ~Transition();

        operator const char * ();

        virtual StateId function(StateMachine &sm, Event &ev);

    protected:
        Transition();

        std::string mString;
    };

    class NoTransition: public Transition {
    public:
        NoTransition();

        virtual StateId function(StateMachine& sm, Event& ev);
    };

    class DirectTransition: public Transition {
    public:
        DirectTransition(StateId nextState);

        virtual StateId function(StateMachine &sm, Event &ev);

    private:
        StateId mNextState;
    };


    class BasicTransition: public Transition {
    public:
        BasicTransition(StateId nextState,
                        TransitionFunction function,
                        const char *functionName);

        virtual StateId function(StateMachine &sm, Event &ev);

    private:
        StateId mNextState;
        TransitionFunction mFunction;
    };


    class AlternativeTransition {
    public:
        AlternativeTransition(ConditionFunction condition,
                              StateId nextState);
        AlternativeTransition(ConditionFunction condition,
                              StateId nextState,
                              TransitionFunction function,
                              const char *functionName);

        virtual ~AlternativeTransition();

        operator const char * ();

        bool function(StateMachine& sm, Event& ev, StateId& nextState);

    private:
        ConditionFunction mCondition;
        Transition *mTransition;
    };


    class ConditionalTransition: public Transition {
    public:
        ConditionalTransition(std::list<AlternativeTransition *> *alternatives);
        virtual ~ConditionalTransition();

        virtual StateId function(StateMachine& sm, Event& ev);

    private:
        std::list<AlternativeTransition *> *mAlternatives;
    };
}

#endif // __MCTL_TRANSITION_H__
