/**
 *  Copyright (c) 2014 Intel
 *  Copyright (c) 2012 BMW
 *
 *  \author Jaska Uimonen
 *  \author Christian Mueller, christian.ei.mueller@bmw.de BMW 2011,2012
 *
 *  \copyright
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  For further information see http://www.genivi.org/.
 */

#include "IAmControlReceiverDbusShadow.h"
#include "control/IAmControlReceive.h"

#include <string.h>
#include <fstream>
#include <stdexcept>
#include <cassert>
#include "shared/CAmDbusWrapper.h"
#include "shared/CAmDltWrapper.h"
#include "configControlDbus.h"

namespace am
{

DLT_IMPORT_CONTEXT(controlDbus)

/**
 * static ObjectPathTable is needed for DBus Callback handling
 */
static DBusObjectPathVTable gObjectPathVTable;

IAmControlReceiverDbusShadow::IAmControlReceiverDbusShadow(IAmControlReceive *iReceiveInterface) :
        mpIAmControlReceiver(iReceiveInterface),
        mDBusWrapper(NULL),
        mFunctionMap(createMap()),
        mDBUSMessageHandler(),
        mHandle(0)
{
}

IAmControlReceiverDbusShadow::~IAmControlReceiverDbusShadow()
{

}

am_Error_e IAmControlReceiverDbusShadow::getRoute(bool onlyfree, am_sourceID_t sourceID, am_sinkID_t sinkID, std::vector<am_Route_s> & returnList)
{
    am_Error_e error = E_OK;
    mpIAmControlReceiver->getRoute(onlyfree, sourceID, sinkID, returnList);
    return (error);
}

void IAmControlReceiverDbusShadow::connect(DBusConnection *conn, DBusMessage *msg)
{
    (void) ((conn));
    assert(mpIAmControlReceiver != NULL);
    mDBUSMessageHandler.initReceive(msg);
    am_mainConnectionID_t id;
    am_ConnectionFormat_e format = (am_ConnectionFormat_e)mDBUSMessageHandler.getInt();
    am_sourceID_t sourceID = mDBUSMessageHandler.getUInt();
    am_sinkID_t sinkID = mDBUSMessageHandler.getUInt();

    am_Error_e returnCode = autoConnectionRequest(sourceID, sinkID, id);

    mDBUSMessageHandler.initReply(msg);
    mDBUSMessageHandler.append(returnCode);
    mDBUSMessageHandler.sendMessage();
}


void IAmControlReceiverDbusShadow::disconnect(DBusConnection *conn, DBusMessage *msg)
{
	(void)((conn));

    bool found = false;
    am_Handle_s handle;
    am_connectionID_t connectionID;
    am_sourceID_t sourceID;
    am_sinkID_t sinkID;
	am_Error_e error;
    std::vector<am_MainConnection_s> listAllMainConnections;

    assert(mpIAmControlReceiver != NULL);
    mDBUSMessageHandler.initReceive(msg);

    connectionID = mDBUSMessageHandler.getUInt();
    sourceID= mDBUSMessageHandler.getUInt();
    sinkID = mDBUSMessageHandler.getUInt();

    mpIAmControlReceiver->getListMainConnections(listAllMainConnections);

    // find the connection
    std::vector<am_MainConnection_s>::iterator itAll(listAllMainConnections.begin());
    for (; itAll != listAllMainConnections.end(); ++itAll)
    {
        if (itAll->sinkID == sinkID && itAll->sourceID == sourceID) {
            found = true;
            continue;
        }
    }

    if (!found)
        return;

    error = removeMainConnectionDB(itAll->mainConnectionID);

    mDBUSMessageHandler.initReply(msg);
    mDBUSMessageHandler.append(error);
    mDBUSMessageHandler.sendMessage();
}

am_Error_e IAmControlReceiverDbusShadow::crossfade(am_Handle_s & handle, am_HotSink_e hotSource, am_crossfaderID_t crossfaderID, am_RampType_e rampType, am_time_t rampTime)
{
	(void)handle;
	(void)hotSource;
	(void)crossfaderID;
	(void)rampType;
	(void)rampTime;

	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::abortAction(am_Handle_s handle)
{
	(void)handle;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSourceState(am_Handle_s & handle, am_sourceID_t sourceID, am_SourceState_e state)
{
	(void)handle;
	(void)sourceID;
	(void)state;
	am_Error_e error = E_OK;
    return (error);

}

am_Error_e IAmControlReceiverDbusShadow::setSinkVolume(am_Handle_s & handle, am_sinkID_t sinkID, am_volume_t volume, am_RampType_e ramp, am_time_t time)
{
	(void)handle;
	(void)sinkID;
	(void)volume;
	(void)ramp;
	(void)time;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSourceVolume(am_Handle_s & handle, am_sourceID_t sourceID, am_volume_t volume, am_RampType_e rampType, am_time_t time)
{
	(void)handle;
	(void)sourceID;
	(void)volume;
	(void)rampType;
	(void)time;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSinkSoundProperties(am_Handle_s & handle, am_sinkID_t sinkID, std::vector<am_SoundProperty_s> & soundProperty)
{
	(void)handle;
	(void)sinkID;
	(void)soundProperty;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSinkSoundProperty(am_Handle_s & handle, am_sinkID_t sinkID, am_SoundProperty_s & soundProperty)
{
	(void)handle;
	(void)sinkID;
	(void)soundProperty;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSourceSoundProperties(am_Handle_s & handle, am_sourceID_t sourceID, std::vector<am_SoundProperty_s> & soundProperty)
{
	(void)handle;
	(void)sourceID;
	(void)soundProperty;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setSourceSoundProperty(am_Handle_s & handle, am_sourceID_t sourceID, am_SoundProperty_s & soundProperty)
{
	(void)handle;
	(void)sourceID;
	(void)soundProperty;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::setDomainState(am_domainID_t domainID, am_DomainState_e domainState)
{
	(void)domainID;
	(void)domainState;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterDomainDB(am_Domain_s & domainData, am_domainID_t & domainID)
{
	(void)domainData;
	(void)domainID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterMainConnectionDB(am_MainConnection_s & mainConnectionData, am_mainConnectionID_t & connectionID)
{
	(void)mainConnectionData;
	(void)connectionID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterSinkDB(am_Sink_s & sinkData, am_sinkID_t & sinkID)
{
	(void)sinkData;
	(void)sinkID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterCrossfaderDB(am_Crossfader_s & crossfaderData, am_crossfaderID_t & crossfaderID)
{
	(void)crossfaderData;
	(void)crossfaderID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterGatewayDB(am_Gateway_s & gatewayData, am_gatewayID_t & gatewayID)
{
	(void)gatewayData;
	(void)gatewayID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterSourceDB(am_Source_s & sourceData, am_sourceID_t & sourceID)
{
	(void)sourceData;
	(void)sourceID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterSinkClassDB(am_SinkClass_s & sinkClass, am_sinkClass_t & sinkClassID)
{
	(void)sinkClass;
	(void)sinkClassID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterSourceClassDB(am_sourceClass_t & sourceClassID, am_SourceClass_s & sourceClass)
{
	(void)sourceClassID;
	(void)sourceClass;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSinkClassInfoDB(am_SinkClass_s & sinkClass)
{
	(void)sinkClass;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSourceClassInfoDB(am_SourceClass_s & sourceClass)
{
	(void)sourceClass;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::enterSystemPropertiesListDB(std::vector<am_SystemProperty_s> & listSystemProperties)
{
	(void)listSystemProperties;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeMainConnectionRouteDB(am_mainConnectionID_t mainconnectionID, std::vector<am_connectionID_t> & listConnectionID)
{
	(void)mainconnectionID;
	(void)listConnectionID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeMainConnectionStateDB(am_mainConnectionID_t mainconnectionID, am_ConnectionState_e connectionState)
{
	(void)mainconnectionID;
	(void)connectionState;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSinkMainVolumeDB(am_mainVolume_t mainVolume, am_sinkID_t sinkID)
{
	(void)mainVolume;
	(void)sinkID;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListMainConnections(std::vector<am_MainConnection_s> & listMainConnections)
{
	(void)listMainConnections;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListDomains(std::vector<am_Domain_s> & listDomains)
{
	(void)listDomains;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListConnections(std::vector<am_Connection_s> & listConnections)
{
	(void)listConnections;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSinks(std::vector<am_Sink_s> & listSinks)
{
	(void)listSinks;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSources(std::vector<am_Source_s> & listSources)
{
	(void)listSources;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSourceClasses(std::vector<am_SourceClass_s> & listSourceClasses)
{
	(void)listSourceClasses;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListHandles(std::vector<am_Handle_s> & listHandles)
{
	(void)listHandles;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListCrossfaders(std::vector<am_Crossfader_s> & listCrossfaders)
{
	(void)listCrossfaders;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListGateways(std::vector<am_Gateway_s> & listGateways)
{
	(void)listGateways;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSinkClasses(std::vector<am_SinkClass_s> & listSinkClasses)
{
	(void)listSinkClasses;
	am_Error_e error = E_OK;
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSystemProperties(std::vector<am_SystemProperty_s> & listSystemProperties)
{
	(void)listSystemProperties;
	am_Error_e error = E_OK;
    return (error);
}

void IAmControlReceiverDbusShadow::setControlReady()
{
    // mpIAmControlReceiver->setControlReady();
}

void IAmControlReceiverDbusShadow::setControlRundown()
{
    // mCAmSerializer.asyncCall<IAmControlReceive>(mpIAmControlReceiver, &IAmControlReceive::setRoutingRundown);
}

void am::IAmControlReceiverDbusShadow::confirmControllerReady(am_Error_e error)
{
     mpIAmControlReceiver->confirmControllerReady(error);
}

void am::IAmControlReceiverDbusShadow::confirmControllerRundown(am_Error_e error)
{
	mpIAmControlReceiver->confirmControllerRundown(error);
}

am_Error_e IAmControlReceiverDbusShadow::getSocketHandler(CAmSocketHandler *& socketHandler)
{
    am_Error_e error = mpIAmControlReceiver->getSocketHandler(socketHandler);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSinkAvailabilityDB(am_Availability_s& availability, am_sinkID_t sinkID)
{
    am_Error_e error = mpIAmControlReceiver->changeSinkAvailabilityDB(availability, sinkID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changDomainStateDB(am_DomainState_e domainState, am_domainID_t domainID)
{
    am_Error_e error = mpIAmControlReceiver->changDomainStateDB(domainState, domainID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSinkMuteStateDB(am_MuteState_e muteState, am_sinkID_t sinkID)
{
    am_Error_e error = mpIAmControlReceiver->changeSinkMuteStateDB(muteState, sinkID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeMainSinkSoundPropertyDB(am_MainSoundProperty_s& soundProperty, am_sinkID_t sinkID)
{
    am_Error_e error = mpIAmControlReceiver->changeMainSinkSoundPropertyDB(soundProperty, sinkID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeMainSourceSoundPropertyDB(am_MainSoundProperty_s& soundProperty, am_sourceID_t sourceID)
{
    am_Error_e error = mpIAmControlReceiver->changeMainSourceSoundPropertyDB(soundProperty, sourceID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSourceAvailabilityDB(am_Availability_s& availability, am_sourceID_t sourceID)
{
    am_Error_e error = mpIAmControlReceiver->changeSourceAvailabilityDB(availability, sourceID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::changeSystemPropertyDB(am_SystemProperty_s& property)
{
    am_Error_e error = mpIAmControlReceiver->changeSystemPropertyDB(property);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeMainConnectionDB(am_mainConnectionID_t mainConnectionID)
{
    am_Error_e error = mpIAmControlReceiver->removeMainConnectionDB(mainConnectionID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeSinkDB(am_sinkID_t sinkID)
{
    am_Error_e error = mpIAmControlReceiver->removeSinkDB(sinkID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeSourceDB(am_sourceID_t sourceID)
{
    am_Error_e error = mpIAmControlReceiver->removeSourceDB(sourceID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeGatewayDB(am_gatewayID_t gatewayID)
{
    am_Error_e error = mpIAmControlReceiver->removeGatewayDB(gatewayID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeCrossfaderDB(am_crossfaderID_t crossfaderID)
{
    am_Error_e error = mpIAmControlReceiver->removeCrossfaderDB(crossfaderID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeDomainDB(am_domainID_t domainID)
{
    am_Error_e error = mpIAmControlReceiver->removeDomainDB(domainID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeSinkClassDB(am_sinkClass_t sinkClassID)
{
    am_Error_e error = mpIAmControlReceiver->removeSinkClassDB(sinkClassID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::removeSourceClassDB(am_sourceClass_t sourceClassID)
{
    am_Error_e error = mpIAmControlReceiver->removeSourceClassDB(sourceClassID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getSourceClassInfoDB(am_sourceID_t sourceID, am_SourceClass_s& classInfo)
{
    am_Error_e error = mpIAmControlReceiver->getSourceClassInfoDB(sourceID, classInfo);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getSinkClassInfoDB(am_sinkID_t sinkID, am_SinkClass_s& sinkClass)
{
    am_Error_e error = mpIAmControlReceiver->getSinkClassInfoDB(sinkID, sinkClass);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getSinkInfoDB(am_sinkID_t sinkID, am_Sink_s& sinkData)
{
    am_Error_e error = mpIAmControlReceiver->getSinkInfoDB(sinkID, sinkData);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getSourceInfoDB(am_sourceID_t sourceID, am_Source_s& sourceData)
{
    am_Error_e error = mpIAmControlReceiver->getSourceInfoDB(sourceID, sourceData);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getGatewayInfoDB(am_gatewayID_t gatewayID, am_Gateway_s& gatewayData)
{
    am_Error_e error = mpIAmControlReceiver->getGatewayInfoDB(gatewayID, gatewayData);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getCrossfaderInfoDB(am_crossfaderID_t crossfaderID, am_Crossfader_s& crossfaderData)
{
    am_Error_e error = mpIAmControlReceiver->getCrossfaderInfoDB(crossfaderID, crossfaderData);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getMainConnectionInfoDB(am_mainConnectionID_t mainConnectionID, am_MainConnection_s& mainConnectionData)
{
    am_Error_e error = mpIAmControlReceiver->getMainConnectionInfoDB(mainConnectionID, mainConnectionData);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSinksOfDomain(am_domainID_t domainID, std::vector<am_sinkID_t>& listSinkID)
{
    am_Error_e error = E_OK;
    mpIAmControlReceiver->getListSinksOfDomain(domainID, listSinkID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListSourcesOfDomain(am_domainID_t domainID, std::vector<am_sourceID_t>& listSourceID)
{
    am_Error_e error = mpIAmControlReceiver->getListSourcesOfDomain(domainID, listSourceID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListCrossfadersOfDomain(am_domainID_t domainID, std::vector<am_crossfaderID_t>& listCrossfadersID)
{
    am_Error_e error = mpIAmControlReceiver->getListCrossfadersOfDomain(domainID, listCrossfadersID);
    return (error);
}

am_Error_e IAmControlReceiverDbusShadow::getListGatewaysOfDomain(am_domainID_t domainID, std::vector<am_gatewayID_t>& listGatewaysID)
{
    am_Error_e error = mpIAmControlReceiver->getListGatewaysOfDomain(domainID, listGatewaysID);
    return (error);
}

DBusHandlerResult IAmControlReceiverDbusShadow::receiveCallback(DBusConnection* conn, DBusMessage* msg, void* user_data)
{
    assert(conn != NULL);
    assert(msg != NULL);
    assert(user_data != NULL);
    IAmControlReceiverDbusShadow* reference = (IAmControlReceiverDbusShadow*) ((user_data));
    return (reference->receiveCallbackDelegate(conn, msg));
}

void IAmControlReceiverDbusShadow::sendIntrospection(DBusConnection* conn, DBusMessage* msg)
{
    assert(conn != NULL);
    assert(msg != NULL);
    DBusMessage* reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;

    // create a reply from the message
    reply = dbus_message_new_method_return(msg);
    std::string fullpath(CONTROL_DBUS_INTROSPECTION_FILE);
    std::ifstream in(fullpath.c_str(), std::ifstream::in);
    if (!in)
    {
        logError("IAmControlReceiverDbusShadow::sendIntrospection could not load xml file ",fullpath);
        throw std::runtime_error("IAmControlReceiverDbusShadow::sendIntrospection Could not load introspecton XML");
    }
    std::string introspect((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const char* string = introspect.c_str();

    // add the arguments to the reply
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string))
    {
        log(&controlDbus, DLT_LOG_INFO, "DBUS handler Out Of Memory!");
    }

    // send the reply && flush the connection
    if (!dbus_connection_send(conn, reply, &serial))
    {
        log(&controlDbus, DLT_LOG_INFO, "DBUS handler Out Of Memory!");
    }
    dbus_connection_flush(conn);

    // free the reply
    dbus_message_unref(reply);
}

DBusHandlerResult IAmControlReceiverDbusShadow::receiveCallbackDelegate(DBusConnection* conn, DBusMessage* msg)
{
    if (dbus_message_is_method_call(msg, DBUS_INTERFACE_INTROSPECTABLE, "Introspect"))
    {
        sendIntrospection(conn, msg);
        return (DBUS_HANDLER_RESULT_HANDLED);
    }
    functionMap_t::iterator iter = mFunctionMap.begin();
    std::string k(dbus_message_get_member(msg));
    log(&controlDbus, DLT_LOG_INFO, k.c_str());
    iter = mFunctionMap.find(k);
    if (iter != mFunctionMap.end())
    {
        std::string p(iter->first);
        CallBackMethod cb = iter->second;
        (this->*cb)(conn, msg);
        return (DBUS_HANDLER_RESULT_HANDLED);
    }
    return (DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
}

void IAmControlReceiverDbusShadow::setControlReceiver(IAmControlReceive*& receiver)
{
    assert(receiver != NULL);
    mpIAmControlReceiver = receiver;
    gObjectPathVTable.message_function = IAmControlReceiverDbusShadow::receiveCallback;
    DBusConnection* connection;
    mpIAmControlReceiver->getDBusConnectionWrapper(mDBusWrapper);
    assert(mDBusWrapper != NULL);
    mDBusWrapper->getDBusConnection(connection);
    assert(connection != NULL);
    mDBUSMessageHandler.setDBusConnection(connection);
    std::string path(CONTROL_NODE);
    {
        assert(receiver != NULL);
    }
    mDBusWrapper->registerCallback(&gObjectPathVTable, path, this);
}

void IAmControlReceiverDbusShadow::gotReady(int16_t numberDomains, uint16_t handle)
{
	(void)numberDomains;
	(void)handle;
}

void IAmControlReceiverDbusShadow::gotRundown(int16_t numberDomains, uint16_t handle)
{
	(void)numberDomains;
	(void)handle;
}

IAmControlReceiverDbusShadow::functionMap_t IAmControlReceiverDbusShadow::createMap()
{
    functionMap_t m;
    m["connect"] = &IAmControlReceiverDbusShadow::connect;
    m["disconnect"] = &IAmControlReceiverDbusShadow::disconnect;
    return (m);
}

am_Error_e IAmControlReceiverDbusShadow::autoConnectionRequest(const am_sourceID_t sourceID, const am_sinkID_t sinkID, am_mainConnectionID_t & mainConnectionID)
{
    std::vector<am_Route_s> listRoutes;
    std::vector<am_connectionID_t> listConnectionIDs;
    am_mainConnectionID_t currentMainConnection;
    am_mainConnectionID_t newMainConnection;
    am_sourceID_t oldSourceID;
    am_MainConnection_s mainConnectionData;
    std::vector<am_MainConnection_s> listAllMainConnections;

    mpIAmControlReceiver->getRoute(false, sourceID, sinkID, listRoutes);

    if (listRoutes.empty())
        return (E_NOT_POSSIBLE);


    mpIAmControlReceiver->getListMainConnections(listAllMainConnections);

    //go through all connections
    std::vector<am_MainConnection_s>::iterator itAll(listAllMainConnections.begin());
    for (; itAll != listAllMainConnections.end(); ++itAll)
    {
        if (itAll->sinkID == sinkID && itAll->sourceID == sourceID)
            return (E_ALREADY_EXISTS);
    }

    mainConnectionData.mainConnectionID = 0;
    mainConnectionData.sinkID = sinkID;
    mainConnectionData.sourceID = sourceID;
    mainConnectionData.connectionState = CS_CONNECTED;
    mainConnectionData.delay = 0;

    mpIAmControlReceiver->enterMainConnectionDB(mainConnectionData, mainConnectionID);
    newMainConnection=mainConnectionID;

    connect_auto(sourceID, sinkID, newMainConnection);

    return (E_OK);
}

void IAmControlReceiverDbusShadow::connect_auto(am_sourceID_t sourceID, am_sinkID_t sinkID, am_mainConnectionID_t mainConnectionID)
{
    std::vector<am_Route_s> listRoutes;
    std::vector<am_connectionID_t> listConnectionIDs;
    am_SourceClass_s sourceClass, tempSourceClass;
    am_Handle_s handle;
    am_connectionID_t connectionID;

    mpIAmControlReceiver->connect(handle, connectionID, CF_GENIVI_AUTO, sourceID, sinkID);

    listConnectionIDs.push_back(connectionID);

    mpIAmControlReceiver->changeMainConnectionRouteDB(mainConnectionID,listConnectionIDs);
}

} /* namespace am */
