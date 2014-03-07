#ifndef __MCTL_EVENT_H__
#define __MCTL_EVENT_H__

#include <stdint.h>

#include <audiomanagertypes.h>

#include <ios>
#include <string>
#include <vector>


namespace mctl {

    typedef enum {
        EVENT_TYPE_CONNECT = 0,
        EVENT_TYPE_REMOVE,
        EVENT_TYPE_DOMAIN_CONN_UP,
        EVENT_TYPE_DOMAIN_CONN_DOWN,
        EVENT_TYPE_PLAY,
        EVENT_TYPE_STOP,
        EVENT_TYPE_NEW_ROUTE,
        EVENT_TYPE_NO_ROUTE,
        EVENT_TYPE_ROUTE_READY,
        EVENT_TYPE_ROUTE_DELETED,
        EVENT_TYPE_DONE,
        EVENT_TYPE_MAX
    } EventType;
    
    class Event {        
    public:
        Event(const Event& ev);
        virtual ~Event();
        
        EventType getType();

        operator const char * ();
        
    protected:
        Event(EventType type);

        virtual void copyMembers(const Event& ev);

        EventType mType;
        std::string mString;
    };


    class ConnectEvent: public Event {
    public:
        ConnectEvent(std::vector<am::am_Route_s>& routeList);
        ConnectEvent(const ConnectEvent& ev);

        size_t size();
        am::am_Route_s& route(const size_t index);
        am::am_Route_s& operator [](const size_t index);

    private:
        std::vector<am::am_Route_s> mRouteList;
    };

    class RemoveEvent: public Event {
    public:
        RemoveEvent();
        RemoveEvent(const RemoveEvent& ev);
    };

    class DomainConnUpEvent: public Event {
    public:
        DomainConnUpEvent(const am::am_Handle_s& handle);
        DomainConnUpEvent(const DomainConnUpEvent& ev);

        am::am_Handle_s& getHandle();

    private:
        am::am_Handle_s mHandle;
    };

    class DomainConnDownEvent: public Event {
    public:
        DomainConnDownEvent(const am::am_Handle_s& handle);
        DomainConnDownEvent(const DomainConnDownEvent& ev);

        am::am_Handle_s& getHandle();

    private:
        am::am_Handle_s mHandle;
    };

    class PlayEvent: public Event {
    public:
        PlayEvent();
        PlayEvent(const PlayEvent& ev);
    };

    class StopEvent: public Event {
    public:
        StopEvent();
        StopEvent(const StopEvent& ev);
    };

    class NewRouteEvent: public Event {
    public:
        NewRouteEvent(const am::am_Route_s& route);
        NewRouteEvent(const NewRouteEvent& ev);

        am::am_sourceID_t mainSourceID();
        am::am_sinkID_t mainSinkID();

        size_t size();
        am::am_RoutingElement_s& routingElement(const size_t index);
        am::am_RoutingElement_s& operator [](const size_t index); 

    private:
        am::am_sourceID_t mMainSourceID;
        am::am_sinkID_t mMainSinkID;
        std::vector<am::am_RoutingElement_s> mRoute;
    };

    class NoRouteEvent: public Event {
    public:
        NoRouteEvent();
        NoRouteEvent(const NoRouteEvent& ev);
    };

    class RouteReadyEvent: public Event {
    public:
        RouteReadyEvent();
        RouteReadyEvent(const RouteReadyEvent& ev);
    };

    class RouteDeletedEvent: public Event {
    public:
        RouteDeletedEvent();
        RouteDeletedEvent(const RouteDeletedEvent& ev);
    };

    class DoneEvent: public Event {
    public:
        DoneEvent();
        DoneEvent(const DoneEvent& ev);
    };


}

#endif // __MCTL_EVENT_H__
