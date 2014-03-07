#include <stdlib.h>

#include <iostream>

#include "Transition.h"


using namespace mctl;


Transition::Transition()
{
}

Transition::~Transition()
{
}


Transition::operator const char * ()
{
    return mString.c_str();
}


StateId Transition::function(StateMachine& sm, Event& ev)
{
    (void)(ev);

    return sm.getStateID();
}


NoTransition::NoTransition()
{
    mString = "Ignoring the event (no transition).";
}

StateId NoTransition::function(StateMachine& sm, Event& ev)
{
    (void)(ev);

    return sm.getStateID();
}

DirectTransition::DirectTransition(StateId nextState):
    mNextState(nextState)
{
    mString = "Direct transition.";
}

StateId DirectTransition::function(StateMachine &sm, Event &ev)
{
    (void)(sm);
    (void)(ev);

    return mNextState;
}


BasicTransition::BasicTransition(StateId nextState,
                                 TransitionFunction function,
                                 const char *functionName):
    mNextState(nextState),
    mFunction(function)
{
    mString  = "Transiting via ";
    mString += functionName;
    mString += "() function.";
}

StateId BasicTransition::function(StateMachine& sm, Event &ev)
{
    bool success;

    success = mFunction(sm, ev);

    return success ? mNextState : sm.getStateID();
}

AlternativeTransition::AlternativeTransition(ConditionFunction condition,
                                             StateId nextState):
    mCondition(condition)
{
    DirectTransition *directTransition = new DirectTransition(nextState);

    mTransition = static_cast<Transition *>(directTransition);
}


AlternativeTransition::AlternativeTransition(ConditionFunction condition,
                                             StateId nextState,
                                             TransitionFunction function,
                                             const char *functionName):
    mCondition(condition)
{
    BasicTransition *basicTransition = new BasicTransition(nextState,
                                                           function,
                                                           functionName);
    mTransition = static_cast<Transition *>(basicTransition);
}


AlternativeTransition::~AlternativeTransition()
{
    delete mTransition;
}

AlternativeTransition::operator const char * ()
{
    return *mTransition;
}


bool AlternativeTransition::function(StateMachine& sm,
                                     Event& ev,
                                     StateId& nextState)
{
    if (!mCondition(sm, ev))
        return false;

    std::cout << sm.getName() << ": " << *mTransition << std::endl;

    nextState = mTransition->function(sm, ev);

    return true;
}

ConditionalTransition::ConditionalTransition(std::list<AlternativeTransition *> *alternatives):
    mAlternatives(alternatives)
{
    mString = "Conditional transition";
}

ConditionalTransition::~ConditionalTransition()
{
    std::list<AlternativeTransition *>::iterator it = mAlternatives->begin();
    for (;   it != mAlternatives->end();   it++)
        delete (*it);

    delete mAlternatives;
}


StateId ConditionalTransition::function(StateMachine& sm, Event& ev)
{
    StateId nextState;

    std::list<AlternativeTransition *>::iterator it = mAlternatives->begin();
    for (;   it != mAlternatives->end();   it++) {
        if ((*it)->function(sm, ev, nextState))
            return nextState;
    }

    std::cout << sm.getName() << ": Ignoring event (no transition)" << std::endl;

    return sm.getStateID();
}
