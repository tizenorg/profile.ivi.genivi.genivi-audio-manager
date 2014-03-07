#ifndef __MCTL_TRANSITION_ROUTINES_H__
#define __MCTL_TRANSITION_ROUTINES_H__

#include "Transition.h"

namespace mctl {
    //
    // Condition functions
    //
    bool CanPlay(StateMachine& sm, Event& ev);
    bool CannotPlay(StateMachine& sm, Event& ev);

    //
    // Transition routines
    //
    bool RouteAndPlaybackRequest(StateMachine& sm, Event& ev);
    bool RouteAndPlaybackRelease(StateMachine& sm, Event& ev);
    bool SavePlaybackGrant(StateMachine& sm, Event& ev);

    bool SetupDomainConnections(StateMachine& sm, Event& ev);
    bool TearDownDomainConnections(StateMachine& sm, Event& ev);
    bool DomainConnectionIsUp(StateMachine& sm, Event& ev);
    bool DomainConnectionIsDown(StateMachine& sm, Event& ev);

    bool FinishMainConnection(StateMachine& sm, Event& ev);
    bool ConnectMainConnection(StateMachine& sm, Event& ev);
    bool SuspendMainConnection(StateMachine& sm, Event& ev);
    bool ResumeMainConnection(StateMachine& sm, Event& ev);
    bool DisconnectMainConnection(StateMachine& sm, Event& ev);
 
    bool FinishAndConnectMainConnection(StateMachine& sm, Event& ev);
    bool FinishAndSuspendMainConnection(StateMachine& sm, Event& ev);
    bool FinishAndDisconnectMainConnection(StateMachine& sm, Event& ev);

    bool MarkPlay(StateMachine& sm, Event& ev);
}

#endif // __MCTL_TRANSITION_ROUTINES_H__
