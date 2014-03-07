/**
 * XXX TODO: add an appropriate copyright and license notice
 */

#include <sstream>

#include <cassert>
#include <algorithm>

#include <murphy/common/debug.h>

#include "CAmControlSenderMurphy.h"
#include "shared/CAmDltWrapper.h"
#include "control/IAmControlReceive.h"

#include "Connection.h"
#include "MurphyInterface.h"

#define AM_UNUSED(var) (void)var

#define FUNCTION   __FUNCTION__, "(): "
#define LOCATION   __FILE__, ":", __LINE__, " "
#define WHERE      ""

#define INFO(...)  log(&MRPC, DLT_LOG_INFO , WHERE, __VA_ARGS__)
#define ERROR(...) log(&MRPC, DLT_LOG_ERROR, WHERE, __VA_ARGS__)
#define WARN(...)  log(&MRPC, DLT_LOG_WARN , WHERE, __VA_ARGS__)
#define DEBUG(...) mrp_debug(__VA_ARGS__)

namespace am
{
  DLT_DECLARE_CONTEXT(MRPC)

  extern "C" IAmControlSend* PluginControlInterfaceMurphyFactory()
  {
    CAmDltWrapper::instance()->registerContext(MRPC, "MRPC",
                                               "Murphy Control Interface");
    return (new CAmControlSenderMurphy());
  }

  extern "C" void destroyControlPluginInterface(IAmControlSend* csif)
  {
    delete csif;
  }

  CAmControlSenderMurphy::CAmControlSenderMurphy() :
    mMif(NULL),
    mControlReceiveInterface(NULL)
  {
  }

  CAmControlSenderMurphy::~CAmControlSenderMurphy()
  {
  }

  am_Error_e CAmControlSenderMurphy::startupController(IAmControlReceive *crif)
  {
    CAmSocketHandler *handler;
    am_Error_e err;

    INFO("Starting Murphy control interface");

    assert(crif);
    mControlReceiveInterface = crif;

    if ((err = crif->getSocketHandler(handler)) != E_OK)
      return err;

    mctl::MurphyInterface::setSocketHandler(handler);
    mMif = mctl::MurphyInterface::getMurphyInterface();
    assert(mMif);

    mMif->setControlReceiver(mControlReceiveInterface);
    mMif->setControlSender(this);

    return E_OK;
  }

  void CAmControlSenderMurphy::setControllerReady()
  {
    INFO("Setting Murphy control interface ready");

    //here is a good place to insert Source and SinkClasses into the database...
    mControlReceiveInterface->setRoutingReady();
    mControlReceiveInterface->setCommandReady();
  }


    am_Source_s CAmControlSenderMurphy::getIcoSourceData(am_domainID_t domainID)
  {
    am_Source_s sourceData;
    am_SoundProperty_s soundProperty;
    am_CustomConnectionFormat_t connectionFormat;
    am_MainSoundProperty_s mainSoundProperty;

    sourceData.sourceID = 0;
    sourceData.domainID = domainID;
    sourceData.name = "icoApplication";
    sourceData.sourceClassID = 67;
    sourceData.sourceState = am::SS_UNKNNOWN;
    sourceData.volume = 32767;
    sourceData.visible = true;
    sourceData.available.availability = am::A_AVAILABLE;
    sourceData.available.availabilityReason = am::AR_UNKNOWN;
    sourceData.interruptState = am::IS_OFF;

    soundProperty.type = am::SP_GENIVI_TREBLE;
    soundProperty.value = 0;
    sourceData.listSoundProperties.push_back(soundProperty);
    soundProperty.type = am::SP_GENIVI_MID;
    soundProperty.value = 0;
    sourceData.listSoundProperties.push_back(soundProperty);

    connectionFormat = am::CF_GENIVI_MONO;
    sourceData.listConnectionFormats.push_back(connectionFormat);

    mainSoundProperty.type = am::MSP_GENIVI_TREBLE;
    mainSoundProperty.value = 0;
    sourceData.listMainSoundProperties.push_back(mainSoundProperty);
    mainSoundProperty.type = am::MSP_GENIVI_MID;
    mainSoundProperty.value = 0;
    sourceData.listMainSoundProperties.push_back(mainSoundProperty);

    return (sourceData);
  }

  am_Source_s CAmControlSenderMurphy::getWrtSourceData(am_domainID_t domainID)
  {
    am_Source_s sourceData;
    am_SoundProperty_s soundProperty;
    am_CustomConnectionFormat_t connectionFormat;
    am_MainSoundProperty_s mainSoundProperty;

    sourceData.sourceID = 0;
    sourceData.domainID = domainID;
    sourceData.name = "wrtApplication";
    sourceData.sourceClassID = 67;
    sourceData.sourceState = am::SS_UNKNNOWN;
    sourceData.volume = 32767;
    sourceData.visible = true;
    sourceData.available.availability = am::A_AVAILABLE;
    sourceData.available.availabilityReason = am::AR_UNKNOWN;
    sourceData.interruptState = IS_OFF;

    soundProperty.type = am::SP_GENIVI_TREBLE;
    soundProperty.value = 0;
    sourceData.listSoundProperties.push_back(soundProperty);
    soundProperty.type = am::SP_GENIVI_MID;
    soundProperty.value = 0;
    sourceData.listSoundProperties.push_back(soundProperty);

    connectionFormat = am::CF_GENIVI_MONO;
    sourceData.listConnectionFormats.push_back(connectionFormat);

    mainSoundProperty.type = am::MSP_GENIVI_TREBLE;
    mainSoundProperty.value = 0;
    sourceData.listMainSoundProperties.push_back(mainSoundProperty);
    mainSoundProperty.type = am::MSP_GENIVI_MID;
    mainSoundProperty.value = 0;
    sourceData.listMainSoundProperties.push_back(mainSoundProperty);

    return (sourceData);
  }

  void CAmControlSenderMurphy::ExecuteConnect(mctl::MainConnection *conn)
  {
    am::am_Route_s route;

    INFO("Executing connect for connection ", conn->getID());

    conn->getRoute(route);

    std::vector<am::am_Route_s> routes;
    routes.push_back(route);

    mctl::ConnectEvent connect(routes);

    conn->queueEvent(connect);

    mctl::PlayEvent play;

    conn->queueEvent(play);

    if (conn->getStateID() == mctl::STATE_END)
      delete conn;
  }

  void CAmControlSenderMurphy::ExecuteDisconnect(mctl::MainConnection *conn)
  {
    mctl::RemoveEvent disconnect;

    INFO("Executing disconnect for connection ", conn->getID());

    mMif->NotifyDisconnect(conn);
    conn->queueEvent(disconnect);

    if (conn->getStateID() == mctl::STATE_END)
      delete conn;
  }

  void CAmControlSenderMurphy::ExecuteSuspend(mctl::MainConnection *conn)
  {
    mctl::StopEvent stop;

    INFO("Executing suspend (stop) for connection ", conn->getID());

    conn->queueEvent(stop);

    if (conn->getStateID() == mctl::STATE_END)
      delete conn;
  }

  void CAmControlSenderMurphy::ExecuteNoOp(mctl::MainConnection *conn)
  {
    INFO("Executing NoOp (<not yet>) for connection ", conn->getID());
  }

  void CAmControlSenderMurphy::ExecuteTeardown(mctl::MainConnection *conn)
  {
    mctl::RemoveEvent evt;

    INFO("Executing teardown for connection ", conn->getID());

    mMif->NotifyDisconnect(conn);
    conn->queueEvent(evt);

    if (conn->getStateID() == mctl::STATE_END)
      delete conn;
  }

  void CAmControlSenderMurphy::ExecuteActions()
  {
    std::vector<am_MainConnection_s> connections;
    mctl::MainConnection *conn;

    INFO("Executing policy actions");

    mControlReceiveInterface->getListMainConnections(connections);

    std::vector<am_MainConnection_s>::iterator c(connections.begin());
    while (c != connections.end()) {
      INFO("Checking disconnect/teardown/suspend for connection ",
           c->mainConnectionID);

      conn = mctl::MainConnection::findMainConnection(c->mainConnectionID);

      if (conn != NULL) {
        switch (mMif->Verdict(conn)) {
        case mctl::MurphyInterface::DECISION_DISCONNECT:
          ExecuteDisconnect(conn);
          break;
        case mctl::MurphyInterface::DECISION_TEARDOWN:
          ExecuteTeardown(conn);
          break;
        case mctl::MurphyInterface::DECISION_SUSPEND:
          ExecuteSuspend(conn);
          break;
        case mctl::MurphyInterface::DECISION_CONNECT:
          break;
        case mctl::MurphyInterface::DECISION_NOOP:
          ExecuteNoOp(conn);
          break;
        }
      }

      c++;
    }

    c = connections.begin();
    while (c != connections.end()) {
      INFO("Checking connect for connection ", c->mainConnectionID);

      conn = mctl::MainConnection::findMainConnection(c->mainConnectionID);

      if (conn != NULL) {
        switch (mMif->Verdict(conn)) {
        case mctl::MurphyInterface::DECISION_CONNECT:
          ExecuteConnect(conn);
          break;
        case mctl::MurphyInterface::DECISION_DISCONNECT:
          break;
        case mctl::MurphyInterface::DECISION_TEARDOWN:
          break;
        case mctl::MurphyInterface::DECISION_SUSPEND:
          break;
        }
      }

      c++;
    }
  }


  bool CAmControlSenderMurphy::ConnectionRoutes(const am_sourceID_t source,
                                                const am_sinkID_t sink,
                                                std::vector<am_Route_s> &routes)
  {
    mControlReceiveInterface->getRoute(false, source, sink, routes);

    return !routes.empty();
  }


  bool CAmControlSenderMurphy::ConnectionExists(const am_sourceID_t source,
                                                const am_sinkID_t sink)
  {
    std::vector<am_MainConnection_s> existing;
    mControlReceiveInterface->getListMainConnections(existing);

    std::vector<am_MainConnection_s>::iterator conn(existing.begin());
    while (conn != existing.end()) {
      if (conn->sinkID == sink && conn->sourceID == source)
        return true;

      conn++;
    }

    return false;
  }


  std::string CAmControlSenderMurphy::SourceName(const am_sourceID_t source)
  {
    std::vector<am_Source_s> sources;
    std::ostringstream ostr;
    std::string name;

    if (mControlReceiveInterface->getListSources(sources) == E_OK) {
      std::vector<am_Source_s>::iterator s(sources.begin());

      while (s != sources.end()) {
        if (s->sourceID == source)
          return s->name;

        s++;
      }
    }

    ostr << source;
    name = ostr.str();

    return name;
  }


  std::string CAmControlSenderMurphy::SinkName(const am_sinkID_t sink)
  {
    std::vector<am_Sink_s> sinks;
    std::ostringstream ostr;
    std::string name;

    if (mControlReceiveInterface->getListSinks(sinks) == E_OK) {
      std::vector<am_Sink_s>::iterator s(sinks.begin());

      while (s != sinks.end()) {
        if (s->sinkID == sink)
          return s->name;

        s++;
      }
    }

    ostr << sink;
    name = ostr.str();

    return name;
  }


  am_Error_e CAmControlSenderMurphy::CreateConnection(const am_sourceID_t source,
                                                      const am_sinkID_t sink,
                                                      uint32_t rset,
                                                      am_mainConnectionID_t &id)
  {
      mctl::MainConnection *conn;

      std::vector<am_MainConnection_s> connections;
      std::vector<am_Route_s> routes;
      std::ostringstream name;

      INFO("CreateConnection: ", source, " -> ", sink);

      if (ConnectionExists(source, sink))
        return E_ALREADY_EXISTS;

      if (!ConnectionRoutes(source, sink, routes))
        return E_NOT_POSSIBLE;

      name << SourceName(source) << " ->" << SinkName(sink);

      conn = new mctl::MainConnection(name.str().c_str(),
                                      static_cast<IAmControlSend *>(this),
                                      mControlReceiveInterface, source, sink,
                                      rset);

      id = conn->getID();

      if (!mMif->RequestRoute(conn, routes)) {
        delete conn;
        return E_UNKNOWN;
      }

      return E_OK;
  }


  am_Error_e CAmControlSenderMurphy::hookUserConnectionRequest(const am_sourceID_t sourceID, const am_sinkID_t sinkID, am_mainConnectionID_t & mainConnectionID)
  {



    return CreateConnection(sourceID, sinkID, 0, mainConnectionID);



#if 0
      std::vector<am_connectionID_t> listConnectionIDs;
      std::vector<am_MainConnection_s> listAllMainConnections;
      std::vector<am_Route_s> listRoutes;
      std::vector<am_Source_s> listSources;
      std::vector<am_Sink_s> listSinks;
      std::ostringstream name;
      am_Error_e err;
      bool found;

      INFO("User connection request: ", sourceID, " -> ", sinkID);

      mControlReceiveInterface->getRoute(false, sourceID, sinkID, listRoutes);
      if (listRoutes.empty())
          return (E_NOT_POSSIBLE);

      mControlReceiveInterface->getListMainConnections(listAllMainConnections);

      std::vector<am_MainConnection_s>::iterator itAll(listAllMainConnections.begin());
      for (; itAll != listAllMainConnections.end(); ++itAll)
          {
              if (itAll->sinkID == sinkID && itAll->sourceID == sourceID)
                  return (E_ALREADY_EXISTS);
          }


      if ((err = mControlReceiveInterface->getListSources(listSources)) != E_OK)
          return err;
      else {
          std::vector<am_Source_s>::iterator it(listSources.begin());
          for (found = false; it != listSources.end();  ++it) {
              if (sourceID == it->sourceID) {
                  found = true;
                  name << it->name;
                  break;
              }
          }
      }
      if (!found)
          name << sourceID;

      name << "->";

      if ((err = mControlReceiveInterface->getListSinks(listSinks)) != E_OK)
          return err;
      else {
          std::vector<am_Sink_s>::iterator it(listSinks.begin());
          for (found = false; it != listSinks.end();  ++it) {
              if (sinkID == it->sinkID) {
                  found = true;
                  name << it->name;
                  break;
              }
          }
      }
      if (!found)
          name << sinkID;

      mctl::MainConnection *conn = new mctl::MainConnection(name.str().c_str(),
                                                            static_cast<IAmControlSend *>(this),
                                                            mControlReceiveInterface,
                                                            sourceID,
                                                            sinkID);
      mainConnectionID = conn->getID();

#if 0
      mctl::ConnectEvent connev(listRoutes);
      conn->queueEvent(connev);

      if (conn->getStateID() == mctl::STATE_END)
          delete conn;
#else
      if (!mMif->RequestRoute(conn, listRoutes)) {
        delete conn;
        return E_UNKNOWN;
      }
#endif

      return (E_OK);
#endif
  }


  am_Error_e CAmControlSenderMurphy::hookUserDisconnectionRequest(const am_mainConnectionID_t connectionID)
  {
      mctl::MainConnection *conn = mctl::MainConnection::findMainConnection(connectionID);
      mctl::RemoveEvent remev;

      INFO("User disconnect request for connection ", connectionID);

      if (conn == NULL) {
        ERROR("can't find connection ", connectionID);
        return E_NON_EXISTENT;
      }

      mMif->NotifyDisconnect(conn);
      conn->queueEvent(remev);

      if (conn->getStateID() == mctl::STATE_END)
          delete conn;

      return (E_OK);
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetMainSinkSoundProperty(const am_sinkID_t sinkID, const am_MainSoundProperty_s & soundProperty)
  {
    AM_UNUSED(sinkID);
    AM_UNUSED(soundProperty);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetMainSourceSoundProperty(const am_sourceID_t sourceID, const am_MainSoundProperty_s & soundProperty)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(soundProperty);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetSystemProperty(const am_SystemProperty_s & property)
  {
    AM_UNUSED(property);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookUserVolumeChange(const am_sinkID_t sinkID, const am_mainVolume_t newVolume)
  {
    assert(sinkID!=0);
    mainVolumeSet set;
    set.sinkID = sinkID;
    set.mainVolume = newVolume;
    am_Error_e error;

    std::vector<mainVolumeSet>::iterator it(mListOpenVolumeChanges.begin());
    for (; it != mListOpenVolumeChanges.end(); ++it)
    {
        if (it->sinkID == sinkID)
            return E_NOT_POSSIBLE;
    }

    am_Sink_s sinkData;
    mControlReceiveInterface->getSinkInfoDB(sinkID, sinkData);

    if (sinkData.mainVolume == newVolume)
        return E_NO_CHANGE;

    if ((error = mControlReceiveInterface->setSinkVolume(set.handle, sinkID, (newVolume-10)*6, RAMP_UNKNOWN, 20)) != E_OK)
    {
        return error;
    }
    mListOpenVolumeChanges.push_back(set);
    return E_OK;
  }

  am_Error_e CAmControlSenderMurphy::hookUserVolumeStep(const am_sinkID_t sinkID, const int16_t increment)
  {
    assert(sinkID!=0);
    mainVolumeSet set;
    set.sinkID = sinkID;
    am_Error_e error;
    am_Sink_s sink;
    std::vector<mainVolumeSet>::iterator it(mListOpenVolumeChanges.begin());
    for (; it != mListOpenVolumeChanges.end(); ++it)
    {
        if (it->sinkID == sinkID)
            return E_NOT_POSSIBLE;
    }
    mControlReceiveInterface->getSinkInfoDB(sinkID, sink);
    set.mainVolume = sink.mainVolume + increment;
    if ((error = mControlReceiveInterface->setSinkVolume(set.handle, sinkID, (set.mainVolume-10)*6, RAMP_UNKNOWN, 20)) != E_OK)
    {
        return error;
    }
    mListOpenVolumeChanges.push_back(set);
    return E_OK;
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetSinkMuteState(const am_sinkID_t sinkID, const am_MuteState_e muteState)
  {
    assert(sinkID!=0);

    mainVolumeSet set;
    set.sinkID = sinkID;
    am_Error_e error;
    am_Sink_s sink;
    mControlReceiveInterface->getSinkInfoDB(sinkID, sink);

    if (muteState == MS_MUTED)
    {
        set.mainVolume = sink.mainVolume;
        if ((error = mControlReceiveInterface->setSinkVolume(set.handle, sinkID, 0, RAMP_GENIVI_DIRECT, 20)) != E_OK)
        {
            return error;
        }
    }
    else
    {
        set.mainVolume = sink.mainVolume;
        if ((error = mControlReceiveInterface->setSinkVolume(set.handle, sinkID, set.mainVolume, RAMP_GENIVI_DIRECT, 20)) != E_OK)
        {
            return error;
        }
    }
    mListOpenVolumeChanges.push_back(set);
    mControlReceiveInterface->changeSinkMuteStateDB(muteState, sinkID);
    return (E_OK);
  }

  am_Error_e CAmControlSenderMurphy::hookSystemRegisterDomain(const am_Domain_s & domainData, am_domainID_t & domainID)
  {
    am_Error_e error;

    error = mControlReceiveInterface->enterDomainDB(domainData, domainID);

    DEBUG("domain %s register%s as %u", domainData.name.c_str(),
          domainData.complete ? "ed" : "ing", domainID);

    if (error == E_OK && domainData.name == "PULSE") {
      am_sourceID_t sourceID = 0;
      am_Source_s sourceDataIco(getIcoSourceData(domainID));
      am_Source_s sourceDataWrt(getWrtSourceData(domainID));

      error = mControlReceiveInterface->enterSourceDB(sourceDataIco, sourceID);

      if (!error)
        DEBUG("domain %u registered static source '%s' as %u",
              sourceDataIco.domainID, sourceDataIco.name.c_str(), sourceID);
      else
        DEBUG("domain %u failed to register static source '%s'",
              sourceDataIco.domainID, sourceDataIco.name.c_str());

      error = mControlReceiveInterface->enterSourceDB(sourceDataWrt, sourceID);

      if (!error)
        DEBUG("domain %u registered static source '%s' as %u",
              sourceDataWrt.domainID, sourceDataWrt.name.c_str(), sourceID);
      else
        DEBUG("domain %u failed to register static source '%s'",
              sourceDataWrt.domainID, sourceDataWrt.name.c_str());
    }

    mMif->ExpectSinkAndSourceChanges(true);

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemDeregisterDomain(const am_domainID_t domainID)
  {
    am_Error_e error;

    error = mControlReceiveInterface->removeDomainDB(domainID);

    DEBUG("domain %u deregistered", domainID);

    return error;
  }

  void CAmControlSenderMurphy::hookSystemDomainRegistrationComplete(const am_domainID_t domainID)
  {
    DEBUG("registration of domain %u complete", domainID);
  }

  am_Error_e CAmControlSenderMurphy::hookSystemRegisterSink(const am_Sink_s & sinkData, am_sinkID_t & sinkID)
  {
    am_Error_e error;

    error = mControlReceiveInterface->enterSinkDB(sinkData, sinkID);

    if (!error)
      DEBUG("domain %u registered sink '%s' as %u", sinkData.domainID,
            sinkData.name.c_str(), sinkID);
    else
      DEBUG("domain %u failed to register sink '%s'", sinkData.domainID,
            sinkData.name.c_str());

    mMif->ScheduleSinkAndSourceUpdate();

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemDeregisterSink(const am_sinkID_t sinkID)
  {
    am_Error_e error;
    std::vector<am_connectionID_t> listConnectionIDs;
    std::vector<am_MainConnection_s> listAllMainConnections;
    bool found = false;

    mControlReceiveInterface->getListMainConnections(listAllMainConnections);

    /* find possible connection */
    std::vector<am_MainConnection_s>::iterator itAll(listAllMainConnections.begin());
    for (; itAll != listAllMainConnections.end(); ++itAll)
    {
        if (itAll->sinkID == sinkID) {
            found = true;
            break;
        }
    }

    if (found)
        mControlReceiveInterface->removeMainConnectionDB(itAll->mainConnectionID);

    error = mControlReceiveInterface->removeSinkDB(sinkID);

    if (!error)
      DEBUG("sink %u deregistered", sinkID);
    else
      DEBUG("deregistration of sink %u failed", sinkID);

    mMif->ScheduleSinkAndSourceUpdate();

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemRegisterSource(const am_Source_s & sourceData, am_sourceID_t & sourceID)
  {
    am_Error_e error;
    std::vector<am_Source_s> listSourcesShadowDarknessOfScorchedEarth;

    mControlReceiveInterface->getListSources(listSourcesShadowDarknessOfScorchedEarth);

    std::vector<am_Source_s>::iterator itAll(listSourcesShadowDarknessOfScorchedEarth.begin());
    for (; itAll != listSourcesShadowDarknessOfScorchedEarth.end(); ++itAll)
    {
        if (itAll->name == sourceData.name) {
            sourceID = itAll->sourceID;
            mControlReceiveInterface->changeSourceAvailabilityDB(sourceData.available, sourceID);
            mMif->ScheduleSinkAndSourceUpdate();
            return E_OK;
        }
    }

    error = mControlReceiveInterface->enterSourceDB(sourceData, sourceID);

    if (!error)
      DEBUG("domain %u registered source '%s' as %u", sourceData.domainID,
            sourceData.name.c_str(), sourceID);
    else
      DEBUG("domain %u failed to register source '%s'", sourceData.domainID,
            sourceData.name.c_str());

    mMif->ScheduleSinkAndSourceUpdate();

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemDeregisterSource(const am_sourceID_t sourceID)
  {
    am_Error_e error;
    std::vector<am_connectionID_t> listConnectionIDs;
    std::vector<am_MainConnection_s> listAllMainConnections;
    bool found = false;
    am_Availability_s available = {A_UNAVAILABLE, AR_GENIVI_NOMEDIA};

    mControlReceiveInterface->getListMainConnections(listAllMainConnections);

    /* find possible connection */
    std::vector<am_MainConnection_s>::iterator itAll(listAllMainConnections.begin());
    for (; itAll != listAllMainConnections.end(); ++itAll)
    {
        if (itAll->sourceID == sourceID) {
            found = true;
            break;
        }
    }

    /*
    if (found)
        mControlReceiveInterface->removeMainConnectionDB(itAll->mainConnectionID);
    */

    /* error = mControlReceiveInterface->removeSourceDB(sourceID); */
    error = mControlReceiveInterface->changeSourceAvailabilityDB(available, sourceID);

    if (!error)
      DEBUG("source %u deregistered", sourceID);
    else
      DEBUG("deregistration of source %u failed", sourceID);

    mMif->ScheduleSinkAndSourceUpdate();

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemRegisterGateway(const am_Gateway_s & gatewayData, am_gatewayID_t & gatewayID)
  {
    am_Error_e error;

    error = mControlReceiveInterface->enterGatewayDB(gatewayData, gatewayID);

    DEBUG("domain '%s' registered as %u", gatewayData.name.c_str(), gatewayID);

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemDeregisterGateway(const am_gatewayID_t gatewayID)
  {
    am_Error_e error;

    error = mControlReceiveInterface->removeGatewayDB(gatewayID);

    if (!error)
      DEBUG("gateway %u deregistered", gatewayID);
    else
      DEBUG("failed to deregister gateway %u", gatewayID);

    return error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemRegisterCrossfader(const am_Crossfader_s & crossfaderData, am_crossfaderID_t & crossfaderID)
  {
    return mControlReceiveInterface->enterCrossfaderDB(crossfaderData, crossfaderID);
  }

  am_Error_e CAmControlSenderMurphy::hookSystemDeregisterCrossfader(const am_crossfaderID_t crossfaderID)
  {
    return mControlReceiveInterface->removeCrossfaderDB(crossfaderID);
  }

  void CAmControlSenderMurphy::hookSystemSinkVolumeTick(const am_Handle_s handle, const am_sinkID_t sinkID, const am_volume_t volume)
  {
    AM_UNUSED(handle);
    AM_UNUSED(sinkID);
    AM_UNUSED(volume);
  }

  void CAmControlSenderMurphy::hookSystemSourceVolumeTick(const am_Handle_s handle, const am_sourceID_t sourceID, const am_volume_t volume)
  {
    AM_UNUSED(handle);
    AM_UNUSED(sourceID);
    AM_UNUSED(volume);
  }

  void CAmControlSenderMurphy::hookSystemInterruptStateChange(const am_sourceID_t sourceID, const am_InterruptState_e interruptState)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(interruptState);
  }

  void CAmControlSenderMurphy::hookSystemSinkAvailablityStateChange(const am_sinkID_t sinkID, const am_Availability_s & availability)
  {
    INFO("sink ", sinkID, " is now ",
         availability.availability != A_AVAILABLE ? "unavailable" : "available");

    mMif->ScheduleSinkAndSourceUpdate();
  }

  void CAmControlSenderMurphy::hookSystemSourceAvailablityStateChange(const am_sourceID_t sourceID, const am_Availability_s & availability)
  {
    INFO("source ", sourceID, " is now ",
         availability.availability != A_AVAILABLE ? "unavailable" : "available");

    mMif->ScheduleSinkAndSourceUpdate();
  }

  void CAmControlSenderMurphy::hookSystemDomainStateChange(const am_domainID_t domainID, const am_DomainState_e state)
  {
    AM_UNUSED(domainID);
    AM_UNUSED(state);
  }

  void CAmControlSenderMurphy::hookSystemReceiveEarlyData(const std::vector<am_EarlyData_s> & data)
  {
    AM_UNUSED(data);
  }

  void CAmControlSenderMurphy::hookSystemSpeedChange(const am_speed_t speed)
  {
    AM_UNUSED(speed);
  }
  void CAmControlSenderMurphy::hookSystemTimingInformationChanged(const am_mainConnectionID_t mainConnectionID, const am_timeSync_t time)
  {
    AM_UNUSED(mainConnectionID);
    AM_UNUSED(time);
  }

  void CAmControlSenderMurphy::cbAckConnect(const am_Handle_s handle, const am_Error_e errorID)
  {
    AM_UNUSED(errorID);

    mctl::MainConnection *conn = mctl::MainConnection::findMainConnection(handle);

    if (!conn) {
      ERROR("CAmControlSenderMurphy::cbAckConnect: can't find connection for handle ", handle.handleType, "/", handle.handle);
      return;
    }

    mctl::DomainConnUpEvent upev(handle);
    conn->queueEvent(upev);
  }

  void CAmControlSenderMurphy::cbAckDisconnect(const am_Handle_s handle, const am_Error_e errorID)
  {
    AM_UNUSED(errorID);

    mctl::MainConnection *conn = mctl::MainConnection::findMainConnection(handle);

    if (!conn) {
      ERROR("CAmControlSenderMurphy::cbAckDisconnect: can't find connection for handle ", handle.handleType, "/", handle.handle);
      return;
    }

    mctl::DomainConnDownEvent downev(handle);
    conn->queueEvent(downev);

    if (conn->getStateID() == mctl::STATE_END)
      delete conn;
  }

  void CAmControlSenderMurphy::cbAckCrossFade(const am_Handle_s handle, const am_HotSink_e hostsink, const am_Error_e error)
  {
    AM_UNUSED(handle);
    AM_UNUSED(hostsink);
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::cbAckSetSinkVolumeChange(const am_Handle_s handle, const am_volume_t volume, const am_Error_e error)
  {
    AM_UNUSED(error);
    AM_UNUSED(volume);
    //\todo:error checking
    std::vector<mainVolumeSet>::iterator it(mListOpenVolumeChanges.begin());
    for (; it != mListOpenVolumeChanges.end(); ++it)
    {
        if (handle.handle == it->handle.handle)
        {
            mControlReceiveInterface->changeSinkMainVolumeDB(it->mainVolume, it->sinkID);
            mListOpenVolumeChanges.erase(it);
            break;
        }
    }

    callStateFlowHandler();
  }

  void CAmControlSenderMurphy::cbAckSetSourceVolumeChange(const am_Handle_s handle, const am_volume_t volume, const am_Error_e error)
  {
    AM_UNUSED(error);
    AM_UNUSED(volume);
    AM_UNUSED(handle);

    callStateFlowHandler();
  }

  void CAmControlSenderMurphy::cbAckSetSourceState(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(error);
    AM_UNUSED(handle);

    callStateFlowHandler();
  }

  void CAmControlSenderMurphy::cbAckSetSourceSoundProperty(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(error);
    AM_UNUSED(handle);
  }

  void CAmControlSenderMurphy::cbAckSetSinkSoundProperty(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(error);

    //\todo:error checking
    std::vector<mainSinkSoundPropertySet>::iterator it(mListMainSoundPropertyChanges.begin());
    for (; it != mListMainSoundPropertyChanges.end(); ++it)
    {
        if (handle.handle == it->handle.handle)
        {
            mControlReceiveInterface->changeMainSinkSoundPropertyDB(it->mainSoundProperty, it->sinkID);
            mListMainSoundPropertyChanges.erase(it);
            break;
        }
    }
  }

  void CAmControlSenderMurphy::cbAckSetSourceSoundProperties(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(error);
    AM_UNUSED(handle);
  }

  void CAmControlSenderMurphy::cbAckSetSinkSoundProperties(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(handle);
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::setControllerRundown(const int16_t signal)
  {
    INFO("Shutting down Murphy control interface");

    mctl::StateMachine::freeTransitions();

    mControlReceiveInterface->confirmControllerRundown(E_OK);
  }

  am_Error_e CAmControlSenderMurphy::getConnectionFormatChoice(const am_sourceID_t sourceID, const am_sinkID_t sinkID, const am_Route_s listRoute, const std::vector<am_CustomConnectionFormat_t> listPossibleConnectionFormats, std::vector<am_CustomConnectionFormat_t> & listPrioConnectionFormats)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(sinkID);
    AM_UNUSED(listRoute);

    listPrioConnectionFormats = listPossibleConnectionFormats;

    return E_OK;
  }

  void CAmControlSenderMurphy::getInterfaceVersion(std::string & version) const
  {
    version = ControlSendVersion;
  }


  void CAmControlSenderMurphy::disconnect(am_mainConnectionID_t connectionID)
  {
    am_MainConnection_s mainConnection;
    am_Error_e error;
    std::vector<handleStatus> listHandleStaus;
    mainConnectionSet set;

    if ((error = mControlReceiveInterface->getMainConnectionInfoDB(connectionID, mainConnection)) != E_OK)
    {
      ERROR("getInfo for connection ", connectionID, " failed (", error, ")");
      return;
    }

    std::vector<am_connectionID_t>::iterator it(mainConnection.listConnectionID.begin());

    for (; it != mainConnection.listConnectionID.end(); ++it)
    {
        handleStatus status;
        status.status = false;
        if ((error = mControlReceiveInterface->disconnect(status.handle, *it)))
        {
          ERROR("Could not disconnect (error: ", error, ")");
        }
        listHandleStaus.push_back(status);
    }

    set.connectionID = connectionID;
    set.listHandleStaus = listHandleStaus;
    mListOpenDisconnections.push_back(set);
  }

  void CAmControlSenderMurphy::connect(am_sourceID_t sourceID, am_sinkID_t sinkID, am_mainConnectionID_t mainConnectionID)
  {
    std::vector<am_Route_s> listRoutes;
    std::vector<am_connectionID_t> listConnectionIDs;
    am_Handle_s handle;
    std::vector<handleStatus> listHandleStaus;
    mainConnectionSet set;

    mControlReceiveInterface->getRoute(false, sourceID, sinkID, listRoutes);

    if (listRoutes.empty())
      ERROR("No routes, connect not possible.");

    std::vector<am_RoutingElement_s>::iterator it(listRoutes[0].route.begin());

    for (; it != listRoutes[0].route.end(); ++it)
    {
        am_connectionID_t connectionID;
        mControlReceiveInterface->connect(handle, connectionID, it->connectionFormat, it->sourceID, it->sinkID);
        handleStatus status;
        status.handle = handle;
        status.status = false;
        listHandleStaus.push_back(status);
        listConnectionIDs.push_back(connectionID);
    }

    set.connectionID = mainConnectionID;
    set.listHandleStaus = listHandleStaus;
    mControlReceiveInterface->changeMainConnectionRouteDB(mainConnectionID,listConnectionIDs);
    mListOpenConnections.push_back(set);
  }

  void CAmControlSenderMurphy::callStateFlowHandler()
  {
  }

  void CAmControlSenderMurphy::confirmCommandReady(const am_Error_e error)
  {
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::confirmRoutingReady(const am_Error_e error)
  {
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::confirmCommandRundown(const am_Error_e error)
  {
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::confirmRoutingRundown(const am_Error_e error)
  {
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::hookSystemNodeStateChanged(const NsmNodeState_e NodeStateId)
  {
    AM_UNUSED(NodeStateId);
  }

  void CAmControlSenderMurphy::hookSystemNodeApplicationModeChanged(const NsmApplicationMode_e ApplicationModeId)
  {
    AM_UNUSED(ApplicationModeId);
  }


  void CAmControlSenderMurphy::cbAckSetSinkNotificationConfiguration(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(handle);
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::cbAckSetSourceNotificationConfiguration(const am_Handle_s handle, const am_Error_e error)
  {
    AM_UNUSED(handle);
    AM_UNUSED(error);
  }


  NsmErrorStatus_e CAmControlSenderMurphy::hookSystemLifecycleRequest(const uint32_t Request, const uint32_t RequestId)
  {
    AM_UNUSED(Request);
    AM_UNUSED(RequestId);

    return NsmErrorStatus_Error;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemUpdateSink(const am_sinkID_t sinkID, const am_sinkClass_t sinkClassID, const std::vector<am_SoundProperty_s>& listSoundProperties, const std::vector<am_CustomConnectionFormat_t>& listConnectionFormats, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties)
  {
    AM_UNUSED(sinkID);
    AM_UNUSED(sinkClassID);
    AM_UNUSED(listMainSoundProperties);
    AM_UNUSED(listConnectionFormats);
    AM_UNUSED(listSoundProperties);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemUpdateSource(const am_sourceID_t sourceID, const am_sourceClass_t sourceClassID, const std::vector<am_SoundProperty_s>& listSoundProperties, const std::vector<am_CustomConnectionFormat_t>& listConnectionFormats, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(sourceClassID);
    AM_UNUSED(listMainSoundProperties);
    AM_UNUSED(listConnectionFormats);
    AM_UNUSED(listSoundProperties);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookSystemUpdateGateway(const am_gatewayID_t gatewayID, const std::vector<am_CustomConnectionFormat_t>& listSourceConnectionFormats, const std::vector<am_CustomConnectionFormat_t>& listSinkConnectionFormats, const std::vector<bool>& convertionMatrix)
  {
    AM_UNUSED(gatewayID);
    AM_UNUSED(listSourceConnectionFormats);
    AM_UNUSED(listSinkConnectionFormats);
    AM_UNUSED(convertionMatrix);

    return E_NOT_USED;
  }

  void CAmControlSenderMurphy::cbAckSetVolumes(const am_Handle_s handle, const std::vector<am_Volumes_s>& listVolumes, const am_Error_e error)
  {
    AM_UNUSED(handle);
    AM_UNUSED(listVolumes);
    AM_UNUSED(error);
  }

  void CAmControlSenderMurphy::hookSinkNotificationDataChanged(const am_sinkID_t sinkID, const am_NotificationPayload_s& payload)
  {
    AM_UNUSED(sinkID);
    AM_UNUSED(payload);
  }

  void CAmControlSenderMurphy::hookSourceNotificationDataChanged(const am_sourceID_t sourceID, const am_NotificationPayload_s& payload)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(payload);
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetMainSinkNotificationConfiguration(const am_sinkID_t sinkID, const am_NotificationConfiguration_s& notificationConfiguration)
  {
    AM_UNUSED(sinkID);
    AM_UNUSED(notificationConfiguration);

    return E_NOT_USED;
  }

  am_Error_e CAmControlSenderMurphy::hookUserSetMainSourceNotificationConfiguration(const am_sourceID_t sourceID, const am_NotificationConfiguration_s& notificationConfiguration)
  {
    AM_UNUSED(sourceID);
    AM_UNUSED(notificationConfiguration);

    return E_NOT_USED;
  }

  void CAmControlSenderMurphy::hookSystemSessionStateChanged(const std::string& sessionName, const NsmSeat_e seatID, const NsmSessionState_e sessionStateID)
  {
    AM_UNUSED(sessionName);
    AM_UNUSED(seatID);
    AM_UNUSED(sessionStateID);
  }

}
