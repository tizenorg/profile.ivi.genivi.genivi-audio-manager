#include <iostream>
#include <sstream>

#include <audiomanagertypes.h>

#include "Event.h"

using namespace mctl;

Event::Event(EventType type)
{
    mType = type;
}

Event::Event(const Event& ev)
{
    copyMembers(ev);
}


Event::~Event()
{
}

void Event::copyMembers(const Event& ev)
{
    mType = ev.mType;
    mString = ev.mString;
}


EventType Event::getType()
{
    return mType;
}


Event::operator const char * ()
{
    return mString.c_str();
}


ConnectEvent::ConnectEvent(std::vector<am::am_Route_s>& routeList):
    Event(EVENT_TYPE_CONNECT),
    mRouteList(routeList)
{
    mString = "Connect";
}

ConnectEvent::ConnectEvent(const ConnectEvent& ev):
    Event(ev)
{
    mRouteList = ev.mRouteList;
}

size_t ConnectEvent::size()
{
    return mRouteList.size();
}

am::am_Route_s& ConnectEvent::route(const size_t index)
{
    static am::am_Route_s emptyRoute;

    if (index >= mRouteList.size())
        return emptyRoute;

    return mRouteList[index];
}

am::am_Route_s& ConnectEvent::operator [](const size_t index)
{
    return route(index);
}

RemoveEvent::RemoveEvent():
    Event(EVENT_TYPE_REMOVE)
{
    mString = "Remove";
}

RemoveEvent::RemoveEvent(const RemoveEvent& ev):
    Event(ev)
{
}

DomainConnUpEvent::DomainConnUpEvent(const am::am_Handle_s& handle):
    Event(EVENT_TYPE_DOMAIN_CONN_UP),
    mHandle(handle)
{
    mString = "Domain Connection Up";
}

DomainConnUpEvent::DomainConnUpEvent(const DomainConnUpEvent& ev):
    Event(ev)
{
    mHandle = ev.mHandle;
}

am::am_Handle_s& DomainConnUpEvent::getHandle()
{
    return mHandle;
}


DomainConnDownEvent::DomainConnDownEvent(const am::am_Handle_s& handle):
    Event(EVENT_TYPE_DOMAIN_CONN_DOWN),
    mHandle(handle)
{
    mString = "Domain Connection Down";
}

DomainConnDownEvent::DomainConnDownEvent(const DomainConnDownEvent& ev):
    Event(ev)
{
    mHandle = ev.mHandle;
}

am::am_Handle_s& DomainConnDownEvent::getHandle()
{
    return mHandle;
}

PlayEvent::PlayEvent():
    Event(EVENT_TYPE_PLAY)
{
    mString = "Play";
}

PlayEvent::PlayEvent(const PlayEvent& ev):
    Event(ev)
{
}

StopEvent::StopEvent():
    Event(EVENT_TYPE_STOP)
{
    mString = "Stop";
}

StopEvent::StopEvent(const StopEvent& ev):
    Event(ev)
{
}

NewRouteEvent::NewRouteEvent(const am::am_Route_s& route):
    Event(EVENT_TYPE_NEW_ROUTE)
{
    std::ostringstream str;

    mMainSourceID = route.sourceID;
    mMainSinkID = route.sinkID;
    mRoute = route.route;

    str << "New Route (" << mMainSourceID << ", " << mMainSinkID << ", [";
    for (size_t i = 0;  i < size();   i++) {
        am::am_RoutingElement_s rtel = mRoute[i];
        if (i > 0)
            str << ",";
        str << rtel.sourceID << "->" << rtel.sinkID << "@" << rtel.domainID;
    }
    str << "])";

    mString = str.str();
}

NewRouteEvent::NewRouteEvent(const NewRouteEvent& ev):
    Event(ev)
{
    mMainSourceID = ev.mMainSourceID;
    mMainSinkID = ev.mMainSinkID;
    mRoute = ev.mRoute;
}


am::am_sourceID_t NewRouteEvent::mainSourceID()
{
    return mMainSourceID;
}

am::am_sinkID_t NewRouteEvent::mainSinkID()
{
    return mMainSinkID;
}

size_t NewRouteEvent::size()
{
    return mRoute.size();
}

am::am_RoutingElement_s& NewRouteEvent::routingElement(const size_t index)
{
    static am::am_RoutingElement_s emptyRoute;

    if (index >= mRoute.size())
        return emptyRoute;

    return mRoute[index];
}


am::am_RoutingElement_s& NewRouteEvent::operator [](const size_t index)
{
    return routingElement(index);
}


NoRouteEvent::NoRouteEvent():
    Event(EVENT_TYPE_NO_ROUTE)
{
    mString = "No Route";
}

NoRouteEvent::NoRouteEvent(const NoRouteEvent& ev):
    Event(ev)
{
}

RouteReadyEvent::RouteReadyEvent():
    Event(EVENT_TYPE_ROUTE_READY)
{
    mString = "Route Ready";
}

RouteReadyEvent::RouteReadyEvent(const RouteReadyEvent& ev):
    Event(ev)
{
}

RouteDeletedEvent::RouteDeletedEvent():
    Event(EVENT_TYPE_ROUTE_DELETED)
{
    mString = "Route Deleted";
}

RouteDeletedEvent::RouteDeletedEvent(const RouteDeletedEvent& ev):
    Event(ev)
{
}

DoneEvent::DoneEvent():
    Event(EVENT_TYPE_DONE)
{
    mString = "Done";
}

DoneEvent::DoneEvent(const DoneEvent& ev):
    Event(ev)
{
}


