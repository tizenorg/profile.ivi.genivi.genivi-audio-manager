/**
 * Copyright (C) 2012, BMW AG
 *
 * This file is part of GENIVI Project AudioManager.
 *
 * Contributions are licensed to the GENIVI Alliance under one or more
 * Contribution License Agreements.
 *
 * \copyright
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a  copy of the MPL was not distributed with
 * this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * \author Christian Mueller, christian.linke@bmw.de BMW 2011,2012
 *
 * \file
 * For further information see http://www.genivi.org/.
 *
 * THIS CODE HAS BEEN GENERATED BY ENTERPRISE ARCHITECT GENIVI MODEL. PLEASE CHANGE ONLY IN ENTERPRISE ARCHITECT AND GENERATE AGAIN
 */
#if !defined(EA_D485A61C_B7C5_4803_B4BD_2AD92893E9CA__INCLUDED_)
#define EA_D485A61C_B7C5_4803_B4BD_2AD92893E9CA__INCLUDED_

#include <vector>
#include <string>
#include "audiomanagertypes.h"
namespace am {
class CAmSocketHandler;
}


#include "audiomanagertypes.h"
#include "projecttypes.h"
#include "NodeStateManager.h"

#define ControlReceiveVersion "2.0" 
namespace am {
	/**
	 * This interface gives access to all important functions of the audiomanager that are used by the AudioManagerController
	 * to control the system.
	 * There are two rules that have to be kept in mind when implementing against this interface:\n
	 * \warning
	 * 1. CALLS TO THIS INTERFACE ARE NOT THREAD SAFE !!!! \n
	 * 2. YOU MAY NOT CALL THE CALLING INTERFACE DURING AN SYNCHRONOUS OR ASYNCHRONOUS CALL THAT EXPECTS A RETURN VALUE.\n
	 * \details
	 * Violation these rules may lead to unexpected behavior! Nevertheless you can implement thread safe by using the deferred-
	 * call pattern described on the wiki which also helps to implement calls that are forbidden.\n
	 * For more information, please check CAmSerializer
	 * @author Christian Mueller
	 * @created 17-Jan-2013 10:00:24
	 */
	class IAmControlReceive
	{

	public:
		IAmControlReceive() {

		}

		virtual ~IAmControlReceive() {

		}

		/**
		 * This function returns the version of the interface
		 * 
		 * @param version
		 */
		virtual void getInterfaceVersion(std::string& version) const =0;
		/**
		 * calculates a route from source to sink.
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param onlyfree    if true return only routes which use gateways that are not in use at the moment
		 * @param sourceID
		 * @param sinkID
		 * @param returnList    this is a list of routes that are possible to take. unsorted! The longest could be first.
		 * In case not route can be found, the list will return empty.
		 */
		virtual am_Error_e getRoute(const bool onlyfree, const am_sourceID_t sourceID, const am_sinkID_t sinkID, std::vector<am_Route_s>& returnList) =0;
		/**
		 * With this function, elementary connects can be triggered by the controller.
		 * @return E_OK on success, E_UNKNOWN on error, E_WRONG_FORMAT of connectionFormats do not match, E_NO_CHANGE if the
		 * desired connection is already build up
		 * 
		 * @param handle
		 * @param connectionID
		 * @param format
		 * @param sourceID
		 * @param sinkID
		 */
		virtual am_Error_e connect(am_Handle_s& handle, am_connectionID_t& connectionID, const am_ConnectionFormat_e format, const am_sourceID_t sourceID, const am_sinkID_t sinkID) =0;
		/**
		 * is used to disconnect a connection
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if connection was not found, E_NO_CHANGE if no change is
		 * neccessary
		 * 
		 * @param handle
		 * @param connectionID
		 */
		virtual am_Error_e disconnect(am_Handle_s& handle, const am_connectionID_t connectionID) =0;
		/**
		 * triggers a cross fade.
		 * @return E_OK on success, E_UNKNOWN on error E_NO_CHANGE if no change is neccessary
		 * 
		 * @param handle
		 * @param hotSource    this is the source that is going to be the active one after the fading
		 * @param crossfaderID
		 * @param rampType
		 * @param rampTime    rampTime 0 means default value
		 */
		virtual am_Error_e crossfade(am_Handle_s& handle, const am_HotSink_e hotSource, const am_crossfaderID_t crossfaderID, const am_RampType_e rampType, const am_time_t rampTime) =0;
		/**
		 * with this method, all actions that have a handle assigned can be stopped.
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param handle    the handle of the action to be stopped
		 */
		virtual am_Error_e abortAction(const am_Handle_s handle) =0;
		/**
		 * this method sets a source state for a source. This function will trigger the callback cbAckSetSourceState
		 * @return E_OK on success, E_NO_CHANGE if the desired value is already correct, E_UNKNOWN on error, E_NO_CHANGE if no
		 * change is neccessary
		 * 
		 * @param handle
		 * @param sourceID
		 * @param state
		 */
		virtual am_Error_e setSourceState(am_Handle_s& handle, const am_sourceID_t sourceID, const am_SourceState_e state) =0;
		/**
		 * with this function, setting of sinks volumes is done. The behavior of the volume set is depended on the given ramp and
		 * time information.
		 * This function is not only used to ramp volume, but also to mute and direct set the level. Exact behavior is depended on
		 * the selected mute ramps.
		 * @return E_OK on success, E_NO_CHANGE if the volume is already on the desired value, E_OUT_OF_RANGE is the volume is out
		 * of range, E_UNKNOWN on every other error.
		 * 
		 * @param handle
		 * @param sinkID
		 * @param volume
		 * @param ramp
		 * @param time
		 */
		virtual am_Error_e setSinkVolume(am_Handle_s& handle, const am_sinkID_t sinkID, const am_volume_t volume, const am_RampType_e ramp, const am_time_t time) =0;
		/**
		 * with this function, setting of source volumes is done. The behavior of the volume set is depended on the given ramp and
		 * time information.
		 * This function is not only used to ramp volume, but also to mute and direct set the level. Exact behavior is depended on
		 * the selected mute ramps.
		 * @return E_OK on success, E_NO_CHANGE if the volume is already on the desired value, E_OUT_OF_RANGE is the volume is out
		 * of range, E_UNKNOWN on every other error.
		 * 
		 * @param handle
		 * @param sourceID
		 * @param volume
		 * @param rampType
		 * @param time
		 */
		virtual am_Error_e setSourceVolume(am_Handle_s& handle, const am_sourceID_t sourceID, const am_volume_t volume, const am_RampType_e rampType, const am_time_t time) =0;
		/**
		 * is used to set several sinkSoundProperties at a time
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE  if property is out of range, E_NO_CHANGE if no change is
		 * neccessary
		 * 
		 * @param handle
		 * @param sinkID
		 * @param soundProperty
		 */
		virtual am_Error_e setSinkSoundProperties(am_Handle_s& handle, const am_sinkID_t sinkID, const std::vector<am_SoundProperty_s>& soundProperty) =0;
		/**
		 * is used to set sinkSoundProperties
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE  if property is out of range, E_NO_CHANGE if no change is
		 * neccessary
		 * 
		 * @param handle
		 * @param sinkID
		 * @param soundProperty
		 */
		virtual am_Error_e setSinkSoundProperty(am_Handle_s& handle, const am_sinkID_t sinkID, const am_SoundProperty_s& soundProperty) =0;
		/**
		 * is used to set several SourceSoundProperties at a time
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE  if property is out of range. E_NO_CHANGE if no change is
		 * neccessary
		 * 
		 * @param handle
		 * @param sourceID
		 * @param soundProperty
		 */
		virtual am_Error_e setSourceSoundProperties(am_Handle_s& handle, const am_sourceID_t sourceID, const std::vector<am_SoundProperty_s>& soundProperty) =0;
		/**
		 * is used to set sourceSoundProperties
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE  if property is out of range. E_NO_CHANGE if no change is
		 * neccessary
		 * 
		 * @param handle
		 * @param sourceID
		 * @param soundProperty
		 */
		virtual am_Error_e setSourceSoundProperty(am_Handle_s& handle, const am_sourceID_t sourceID, const am_SoundProperty_s& soundProperty) =0;
		/**
		 * sets the domain state of a domain
		 * @return E_OK on success, E_UNKNOWN on error, E_NO_CHANGE if no change is neccessary
		 * 
		 * @param domainID
		 * @param domainState
		 */
		virtual am_Error_e setDomainState(const am_domainID_t domainID, const am_DomainState_e domainState) =0;
		/**
		 * enters a domain in the database, creates and ID
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param domainData    domainID in am_Domain_s must be 0 here
		 * @param domainID
		 */
		virtual am_Error_e enterDomainDB(const am_Domain_s& domainData, am_domainID_t& domainID) =0;
		/**
		 * enters a mainconnection in the database, creates and ID
		 * @return E_OK on success, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param mainConnectionData    the MainConnectionID is omitted since it is created during the registration
		 * @param connectionID
		 */
		virtual am_Error_e enterMainConnectionDB(const am_MainConnection_s& mainConnectionData, am_mainConnectionID_t& connectionID) =0;
		/**
		 * enters a sink in the database.
		 * The sinkID in am_Sink_s shall be 0 in case of a dynamic added source A sinkID greater than 100 will be assigned. If a
		 * specific sinkID with a value <100 is given, the given value will be used. This is for a static setup where the ID's are
		 * predefined.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param sinkData    the sinkID will be omitted since it is created during the registration
		 * @param sinkID
		 */
		virtual am_Error_e enterSinkDB(const am_Sink_s& sinkData, am_sinkID_t& sinkID) =0;
		/**
		 * enters a crossfader in the database.
		 * The crossfaderID in am_Crossfader_s shall be 0 in case of a dynamic added source A crossfaderID greater than 100 will
		 * be assigned. If a specific crossfaderID with a value <100 is given, the given value will be used. This is for a static
		 * setup where the ID's are predefined.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param crossfaderData    the ID in the data will be ignored since it is created during the registration
		 * @param crossfaderID
		 */
		virtual am_Error_e enterCrossfaderDB(const am_Crossfader_s& crossfaderData, am_crossfaderID_t& crossfaderID) =0;
		/**
		 * enters a gateway in the database.
		 * The gatewayID in am_Gateway_s shall be 0 in case of a dynamic added source A gatewayID greater than 100 will be
		 * assigned. If a specific gatewayID with a value <100 is given, the given value will be used. This is for a static setup
		 * where the ID's are predefined.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param gatewayData    In a fixed setup, the gatewayID must be below 100. In a dynamic setup, the gatewayID shall be 0!
		 * listSourceFormats and listSinkFormats are empty at registration time. Values are taken over when sources and sinks are
		 * registered.
		 * @param gatewayID
		 */
		virtual am_Error_e enterGatewayDB(const am_Gateway_s& gatewayData, am_gatewayID_t& gatewayID) =0;
		/**
		 * enters a source in the database.
		 * The sourceID in am_Source_s shall be 0 in case of a dynamic added source A sourceID greater than 100 will be assigned.
		 * If a specific sourceID with a value <100 is given, the given value will be used. This is for a static setup where the
		 * ID's are predefined.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param sourceData    sourceID is omitted here since it is created during registration
		 * @param sourceID
		 */
		virtual am_Error_e enterSourceDB(const am_Source_s& sourceData, am_sourceID_t& sourceID) =0;
		/**
		 * Enters a sourceClass into the database.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param sinkClass    IDs given with the SinkCLassID are valid below 100 (static). If given as 0, sinkClass will be
		 * enterd dynamic and ID above 100 will be created.
		 * @param sinkClassID
		 */
		virtual am_Error_e enterSinkClassDB(const am_SinkClass_s& sinkClass, am_sinkClass_t& sinkClassID) =0;
		/**
		 * Enters a sourceClass into the database.
		 * The sourceClassID in am_sourceClass_s shall be 0 in case of a dynamic added source A sourceClassID greater than 100
		 * will be assigned. If a specific sourceClassID with a value <100 is given, the given value will be used. This is for a
		 * static setup where the ID's are predefined.
		 * @return E_OK on success, E_ALREADY_EXISTENT if the ID or name is already in the database, E_DATABASE_ERROR if the
		 * database had an error
		 * 
		 * @param sourceClassID
		 * @param sourceClass    IDs given with the SourceCLassID are valid below 100 (static). If given as 0, sourceClass will be
		 * enterd dynamic and ID above 100 will be created.
		 */
		virtual am_Error_e enterSourceClassDB(am_sourceClass_t& sourceClassID, const am_SourceClass_s& sourceClass) =0;
		/**
		 * changes class information of a sinkclass.
		 * The properties will overwrite the values of the sinkClassID given in the sinkClass.
		 * It is the duty of the controller to check if the property is valid. If it does not exist, the daemon will not return an
		 * error.
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sinkClassID was not found.
		 * 
		 * @param sinkClass    if a SourceClass ID other than 0 is given, the classInformation replaces the existing one. In case
		 * of ClassID 0, a new classID is created and returned
		 */
		virtual am_Error_e changeSinkClassInfoDB(const am_SinkClass_s& sinkClass) =0;
		/**
		 * changes class information of a sourceClass.
		 * The properties will overwrite the values of the sourceClassID given in the sourceClass.
		 * It is the duty of the controller to check if the property is valid. If it does not exist, the daemon will not return an
		 * error.
		 * @return E_OK on success, E_DATABASE_ERROR on error and E_NON_EXISTENT if the ClassID does not exist.
		 * 
		 * @param sourceClass    if a SourceClass ID other than 0 is given, the classInformation replaces the existing one. In
		 * case of ClassID 0, a new classID is created and returned
		 */
		virtual am_Error_e changeSourceClassInfoDB(const am_SourceClass_s& sourceClass) =0;
		/**
		 * This function is used to enter the system Properties into the database.
		 * All entries in the database will be erased before entering the new List. It should only be called once at system
		 * startup.
		 * @return E_OK on success,  E_DATABASE_ERROR if the database had an error
		 * 
		 * @param listSystemProperties
		 */
		virtual am_Error_e enterSystemPropertiesListDB(const std::vector<am_SystemProperty_s>& listSystemProperties) =0;
		/**
		 * changes the mainConnectionState of MainConnection
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if mainconnection
		 * 
		 * @param mainconnectionID
		 * @param listConnectionID
		 */
		virtual am_Error_e changeMainConnectionRouteDB(const am_mainConnectionID_t mainconnectionID, const std::vector<am_connectionID_t>& listConnectionID) =0;
		/**
		 * changes the mainConnectionState of MainConnection
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if mainconnection
		 * 
		 * @param mainconnectionID
		 * @param connectionState
		 */
		virtual am_Error_e changeMainConnectionStateDB(const am_mainConnectionID_t mainconnectionID, const am_ConnectionState_e connectionState) =0;
		/**
		 * changes the sink volume of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param mainVolume
		 * @param sinkID
		 */
		virtual am_Error_e changeSinkMainVolumeDB(const am_mainVolume_t mainVolume, const am_sinkID_t sinkID) =0;
		/**
		 * changes the availablility of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param availability
		 * @param sinkID
		 */
		virtual am_Error_e changeSinkAvailabilityDB(const am_Availability_s& availability, const am_sinkID_t sinkID) =0;
		/**
		 * changes the domainstate of a domain
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if domain was not found
		 * 
		 * @param domainState
		 * @param domainID
		 */
		virtual am_Error_e changDomainStateDB(const am_DomainState_e domainState, const am_domainID_t domainID) =0;
		/**
		 * changes the mute state of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param muteState
		 * @param sinkID
		 */
		virtual am_Error_e changeSinkMuteStateDB(const am_MuteState_e muteState, const am_sinkID_t sinkID) =0;
		/**
		 * changes the mainsinksoundproperty of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param soundProperty
		 * @param sinkID
		 */
		virtual am_Error_e changeMainSinkSoundPropertyDB(const am_MainSoundProperty_s& soundProperty, const am_sinkID_t sinkID) =0;
		/**
		 * changes the mainsourcesoundproperty of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if source was not found
		 * 
		 * @param soundProperty
		 * @param sourceID
		 */
		virtual am_Error_e changeMainSourceSoundPropertyDB(const am_MainSoundProperty_s& soundProperty, const am_sourceID_t sourceID) =0;
		/**
		 * changes the availablility of a source
		 * @return E_OK on success, E_DATABASE_ERROR  on error, E_NON_EXISTENT if source was not found
		 * 
		 * @param availability
		 * @param sourceID
		 */
		virtual am_Error_e changeSourceAvailabilityDB(const am_Availability_s& availability, const am_sourceID_t sourceID) =0;
		/**
		 * changes a systemProperty
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if property was not found
		 * 
		 * @param property
		 */
		virtual am_Error_e changeSystemPropertyDB(const am_SystemProperty_s& property) =0;
		/**
		 * removes a mainconnection from the DB
		 * @return E_OK on success, E_NON_EXISTENT if main connection was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param mainConnectionID
		 */
		virtual am_Error_e removeMainConnectionDB(const am_mainConnectionID_t mainConnectionID) =0;
		/**
		 * removes a sink from the DB
		 * @return E_OK on success, E_NON_EXISTENT if sink was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param sinkID
		 */
		virtual am_Error_e removeSinkDB(const am_sinkID_t sinkID) =0;
		/**
		 * removes a source from the DB
		 * @return E_OK on success, E_NON_EXISTENT if source was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param sourceID
		 */
		virtual am_Error_e removeSourceDB(const am_sourceID_t sourceID) =0;
		/**
		 * removes a gateway from the DB
		 * @return E_OK on success, E_NON_EXISTENT if gateway was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param gatewayID
		 */
		virtual am_Error_e removeGatewayDB(const am_gatewayID_t gatewayID) =0;
		/**
		 * removes a crossfader from the DB
		 * @return E_OK on success, E_NON_EXISTENT if crossfader was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param crossfaderID
		 */
		virtual am_Error_e removeCrossfaderDB(const am_crossfaderID_t crossfaderID) =0;
		/**
		 * removes a domain from the DB
		 * @return E_OK on success, E_NON_EXISTENT if domain was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param domainID
		 */
		virtual am_Error_e removeDomainDB(const am_domainID_t domainID) =0;
		/**
		 * removes a domain from the DB
		 * @return E_OK on success, E_NON_EXISTENT if domain was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param sinkClassID
		 */
		virtual am_Error_e removeSinkClassDB(const am_sinkClass_t sinkClassID) =0;
		/**
		 * removes a domain from the DB
		 * @return E_OK on success, E_NON_EXISTENT if domain was not found, E_DATABASE_ERROR if the database had an error
		 * 
		 * @param sourceClassID
		 */
		virtual am_Error_e removeSourceClassDB(const am_sourceClass_t sourceClassID) =0;
		/**
		 * returns the ClassInformation of a source
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if source was not found
		 * 
		 * @param sourceID
		 * @param classInfo
		 */
		virtual am_Error_e getSourceClassInfoDB(const am_sourceID_t sourceID, am_SourceClass_s& classInfo) const =0;
		/**
		 * returns the ClassInformation of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param sinkID
		 * @param sinkClass
		 */
		virtual am_Error_e getSinkClassInfoDB(const am_sinkID_t sinkID, am_SinkClass_s& sinkClass) const =0;
		/**
		 * returns the sinkData of a sink
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param sinkID
		 * @param sinkData
		 */
		virtual am_Error_e getSinkInfoDB(const am_sinkID_t sinkID, am_Sink_s& sinkData) const =0;
		/**
		 * returns the sourcekData of a source
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if sink was not found
		 * 
		 * @param sourceID
		 * @param sourceData
		 */
		virtual am_Error_e getSourceInfoDB(const am_sourceID_t sourceID, am_Source_s& sourceData) const =0;
		/**
		 * return source and sink of a gateway
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if gateway was not found
		 * 
		 * @param gatewayID
		 * @param gatewayData
		 */
		virtual am_Error_e getGatewayInfoDB(const am_gatewayID_t gatewayID, am_Gateway_s& gatewayData) const =0;
		/**
		 * returns sources and the sink of a crossfader
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if crossfader was not found
		 * 
		 * @param crossfaderID
		 * @param crossfaderData
		 */
		virtual am_Error_e getCrossfaderInfoDB(const am_crossfaderID_t crossfaderID, am_Crossfader_s& crossfaderData) const =0;
		/**
		 * returns sources and the sink of a crossfader
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if crossfader was not found
		 * 
		 * @param mainConnectionID
		 * @param mainConnectionData
		 */
		virtual am_Error_e getMainConnectionInfoDB(const am_mainConnectionID_t mainConnectionID, am_MainConnection_s& mainConnectionData) const =0;
		/**
		 * returns all sinks of a domain
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if domain was not found
		 * 
		 * @param domainID
		 * @param listSinkID
		 */
		virtual am_Error_e getListSinksOfDomain(const am_domainID_t domainID, std::vector<am_sinkID_t>& listSinkID) const =0;
		/**
		 * returns all source of a domain
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if domain was not found
		 * 
		 * @param domainID
		 * @param listSourceID
		 */
		virtual am_Error_e getListSourcesOfDomain(const am_domainID_t domainID, std::vector<am_sourceID_t>& listSourceID) const =0;
		/**
		 * returns all crossfaders of a domain
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if domain was not found
		 * 
		 * @param domainID
		 * @param listCrossfadersID
		 */
		virtual am_Error_e getListCrossfadersOfDomain(const am_domainID_t domainID, std::vector<am_crossfaderID_t>& listCrossfadersID) const =0;
		/**
		 * returns all gateways of a domain
		 * @return E_OK on success, E_DATABASE_ERROR on error, E_NON_EXISTENT if domain was not found
		 * 
		 * @param domainID
		 * @param listGatewaysID
		 */
		virtual am_Error_e getListGatewaysOfDomain(const am_domainID_t domainID, std::vector<am_gatewayID_t>& listGatewaysID) const =0;
		/**
		 * returns a complete list of all MainConnections
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listMainConnections
		 */
		virtual am_Error_e getListMainConnections(std::vector<am_MainConnection_s>& listMainConnections) const =0;
		/**
		 * returns a complete list of all domains
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listDomains
		 */
		virtual am_Error_e getListDomains(std::vector<am_Domain_s>& listDomains) const =0;
		/**
		 * returns a complete list of all Connections
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listConnections
		 */
		virtual am_Error_e getListConnections(std::vector<am_Connection_s>& listConnections) const =0;
		/**
		 * returns a list of all sinks
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listSinks
		 */
		virtual am_Error_e getListSinks(std::vector<am_Sink_s>& listSinks) const =0;
		/**
		 * returns a list of all sources
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listSources
		 */
		virtual am_Error_e getListSources(std::vector<am_Source_s>& listSources) const =0;
		/**
		 * returns a list of all source classes
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listSourceClasses
		 */
		virtual am_Error_e getListSourceClasses(std::vector<am_SourceClass_s>& listSourceClasses) const =0;
		/**
		 * returns a list of all handles
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listHandles
		 */
		virtual am_Error_e getListHandles(std::vector<am_Handle_s>& listHandles) const =0;
		/**
		 * returns a list of all crossfaders
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listCrossfaders
		 */
		virtual am_Error_e getListCrossfaders(std::vector<am_Crossfader_s>& listCrossfaders) const =0;
		/**
		 * returns a list of  gateways
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listGateways
		 */
		virtual am_Error_e getListGateways(std::vector<am_Gateway_s>& listGateways) const =0;
		/**
		 * returns a list of all sink classes
		 * @return E_OK on success, E_DATABASE_ERROR on error
		 * 
		 * @param listSinkClasses
		 */
		virtual am_Error_e getListSinkClasses(std::vector<am_SinkClass_s>& listSinkClasses) const =0;
		/**
		 * returns the list of SystemProperties
		 * 
		 * @param listSystemProperties
		 */
		virtual am_Error_e getListSystemProperties(std::vector<am_SystemProperty_s>& listSystemProperties) const =0;
		/**
		 * sets the command interface to ready. Will send setCommandReady to each of the plugins. The corresponding answer is
		 * confirmCommandReady. 
		 */
		virtual void setCommandReady() =0;
		/**
		 * sets the command interface into the rundown state. Will send setCommandRundown to each of the plugins. The
		 * corresponding answer is confirmCommandRundown. 
		 */
		virtual void setCommandRundown() =0;
		/**
		 * sets the routinginterface to  ready. Will send the command  setRoutingReady to each of the plugins. The related answer
		 * is confirmRoutingReady.
		 */
		virtual void setRoutingReady() =0;
		/**
		 * sets the routinginterface to the rundown state. Will send the command  setRoutingRundown to each of the plugins. The
		 * related answer is confirmRoutingRundown.
		 */
		virtual void setRoutingRundown() =0;
		/**
		 * acknowledges the setControllerReady call.
		 * 
		 * @param error    E_OK if the ready command succeeded. E_UNKNOWN if an error happened.
		 */
		virtual void confirmControllerReady(const am_Error_e error) =0;
		/**
		 * Acknowledges the setControllerRundown call.
		 * 
		 * @param error    E_OK if the ready command succeeded. E_UNKNOWN if an error happened.
		 * If an error !=E_OK is returned than the AudioManager will terminate with an exception.
		 * If E_OK is returned, the AudioManager will exit clean
		 */
		virtual void confirmControllerRundown(const am_Error_e error) =0;
		/**
		 * This function returns the pointer to the socketHandler. This can be used to integrate socket-based activites like
		 * communication with the mainloop of the AudioManager.
		 * returns E_OK if pointer is valid, E_UNKNOWN in case AudioManager was compiled without socketHandler support,
		 * 
		 * @param socketHandler
		 */
		virtual am_Error_e getSocketHandler(CAmSocketHandler*& socketHandler) =0;
		/**
		 * Change the data of the source.
		 * 
		 * @param sourceID    The sourceID of the source that needs to be changed.
		 * @param sourceClassID    If this sourceClassID !=0, the sourceClassID of the source will be changed.
		 * @param listSoundProperties    If the size of the list !=0, the list replaces the current one.
		 * @param listConnectionFormats    If the size of the list !=0, the list replaces the current one.
		 * @param listMainSoundProperties    If the size of the list !=0, the list replaces the current one.
		 */
		virtual am_Error_e changeSourceDB(const am_sourceID_t sourceID, const am_sourceClass_t sourceClassID, const std::vector<am_SoundProperty_s>& listSoundProperties, const std::vector<am_ConnectionFormat_e>& listConnectionFormats, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties) =0;
		/**
		 * Change the data of the sink.
		 * 
		 * @param sinkID    the sinkID of the sink to be changed
		 * @param sinkClassID    If !=0, the sinkClass will replace the current one.
		 * @param listSoundProperties    The list of the sound properties. If The lenght of the list !=0, the current
		 * soundProperties will be changed.
		 * @param listConnectionFormats    The list of the connectionformats to be changed. They will only be changed, if length
		 * of the list !=0
		 * @param listMainSoundProperties    The list of the mainsoundproperties that need to be changed. Will be changed. if the
		 * length of the list !=0
		 */
		virtual am_Error_e changeSinkDB(const am_sinkID_t sinkID, const am_sinkClass_t sinkClassID, const std::vector<am_SoundProperty_s>& listSoundProperties, const std::vector<am_ConnectionFormat_e>& listConnectionFormats, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties) =0;
		/**
		 * changes Gateway Data
		 * 
		 * @param gatewayID    the gatewayData to be changed
		 * @param listSourceConnectionFormats    The list of the sourceConnectionFormats. Will be changed it the lenght of the
		 * list !=0.
		 * @param listSinkConnectionFormats    The list of the sinkConnectionFormats. Will be changed it the lenght of the list
		 * !=0.
		 * @param convertionMatrix    the convertionmatrix
		 */
		virtual am_Error_e changeGatewayDB(const am_gatewayID_t gatewayID, const std::vector<am_ConnectionFormat_e>& listSourceConnectionFormats, const std::vector<am_ConnectionFormat_e>& listSinkConnectionFormats, const std::vector<bool>& convertionMatrix) =0;
		/**
		 * with this function, setting of multiple volumes at a time is done. The behavior of the volume set is depended on the
		 * given ramp and time information.
		 * This function is not only used to ramp volume, but also to mute and direct set the level. Exact behavior is depended on
		 * the selected mute ramps.
		 * @return E_OK on success, E_NO_CHANGE if the volume is already on the desired value, E_OUT_OF_RANGE is the volume is out
		 * of range, E_UNKNOWN on every other error.
		 * 
		 * @param handle
		 * @param listVolumes
		 */
		virtual am_Error_e setVolumes(am_Handle_s& handle, const std::vector<am_Volumes_s>& listVolumes) =0;
		/**
		 * set a sink notification configuration
		 * 
		 * @param handle    the handle that will be assigned for this operation
		 * @param sinkID    the sinkID
		 * @param notificationConfiguration
		 */
		virtual am_Error_e setSinkNotificationConfiguration(am_Handle_s& handle, const am_sinkID_t sinkID, const am_NotificationConfiguration_s& notificationConfiguration) =0;
		/**
		 * set a source notification configuration
		 * 
		 * @param handle    the handle that will be assigned for this operation
		 * @param sourceID    the sinkID
		 * @param notificationConfiguration
		 */
		virtual am_Error_e setSourceNotificationConfiguration(am_Handle_s& handle, const am_sourceID_t sourceID, const am_NotificationConfiguration_s& notificationConfiguration) =0;
		/**
		 * Sends out the main notificiation of a sink
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param sinkID
		 * @param notificationPayload    the payload
		 */
		virtual void sendMainSinkNotificationPayload(const am_sinkID_t sinkID, const am_NotificationPayload_s& notificationPayload) =0;
		/**
		 * Sends out the main notificiation of a source
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param sourceID
		 * @param notificationPayload    the payload
		 */
		virtual void sendMainSourceNotificationPayload(const am_sourceID_t sourceID, const am_NotificationPayload_s& notificationPayload) =0;
		/**
		 * change the mainNotificationConfiguration of a sink
		 * @return E_OK when successful, E_DATABASE on error
		 * 
		 * @param sinkID    the sinkID
		 * @param mainNotificationConfiguration    the mainNotificationConfiguration
		 */
		virtual am_Error_e changeMainSinkNotificationConfigurationDB(const am_sinkID_t sinkID, const am_NotificationConfiguration_s& mainNotificationConfiguration) =0;
		/**
		 * change the mainNotificationConfiguration of a source
		 * @return E_OK when successful, E_DATABASE on error
		 * 
		 * @param sourceID    the sourceID
		 * @param mainNotificationConfiguration    the mainNotificationConfiguration
		 */
		virtual am_Error_e changeMainSourceNotificationConfigurationDB(const am_sourceID_t sourceID, const am_NotificationConfiguration_s& mainNotificationConfiguration) =0;
		/**
		 * Retrieves the Restart Reason Property
		 * @return E_OK on success E_UNKNOWN if property could not be retrieved.
		 * 
		 * @param restartReason    the restart reason
		 */
		virtual am_Error_e getRestartReasonPropertyNSM(NsmRestartReason_e& restartReason) =0;
		/**
		 * Retrieves the shutdown reason property
		 * @return E_OK on success E_UNKNOWN if property could not be retrieved.
		 * 
		 * @param ShutdownReason    The shutdown reason
		 */
		virtual am_Error_e getShutdownReasonPropertyNSM(NsmShutdownReason_e& ShutdownReason) =0;
		/**
		 * Retrieves the running reason
		 * @return E_OK on success E_UNKNOWN if property could not be retrieved.
		 * 
		 * @param nsmRunningReason    the running reson
		 */
		virtual am_Error_e getRunningReasonPropertyNSM(NsmRunningReason_e& nsmRunningReason) =0;
		/**
		 * @return NsmErrorStatus_Ok in case of success
		 * 
		 * @param nsmNodeState    The current node state
		 */
		virtual NsmErrorStatus_e getNodeStateNSM(NsmNodeState_e& nsmNodeState) =0;
		/**
		 * Retrieves the current session state
		 * @return NsmErrorStatus_Ok on success E_UNKNOWN if property could not be retrieved.
		 * 
		 * @param sessionName    The current session state
		 * @param seatID
		 * @param sessionState    The sessionState
		 */
		virtual NsmErrorStatus_e getSessionStateNSM(const std::string& sessionName, const NsmSeat_e seatID, NsmSessionState_e& sessionState) =0;
		/**
		 * Retrieves the current applicationmode
		 * @return NsmErrorStatus_Ok on success E_UNKNOWN if property could not be retrieved.
		 * 
		 * @param applicationMode    The current application Mode
		 */
		virtual NsmErrorStatus_e getApplicationModeNSM(NsmApplicationMode_e& applicationMode) =0;
		/**
		 * Registers the AudioManager as shutdown client
		 * @return NsmErrorStatus_Ok on success
		 * The interface for the client will be provided by the AudioManagerDeamon, therefore is no need to mention it here.
		 * 
		 * @param shutdownMode    The shutdown mode that the controller wants to register for.
		 * @param timeoutMs    Max. Timeout to wait for response from shutdown client in ms
		 */
		virtual NsmErrorStatus_e registerShutdownClientNSM(const uint32_t shutdownMode, const uint32_t timeoutMs) =0;
		/**
		 * unregisters the AudioManager as shutdown client
		 * @return NsmErrorStatus_Ok on success
		 * 
		 * @param shutdownMode    Shutdown mode for which client wants to unregister (NSM_SHUTDOWNTYPE_NORMAL,
		 * NSM_SHUTDOWNTYPE_FAST)
		 */
		virtual NsmErrorStatus_e unRegisterShutdownClientNSM(const uint32_t shutdownMode) =0;
		/**
		 * @return E_OK on success, E_UNKOWN if interface could not be reached
		 * 
		 * @param version
		 */
		virtual am_Error_e getInterfaceVersionNSM(uint32_t& version) =0;
		/**
		 * Sends out the lifecyclerequest complete to the NSM
		 * @return NsmErrorStatus_Ok in case of success
		 * 
		 * @param RequestId    The request ID of the request.
		 * @param status    The result of the call to "LifecycleRequest". NsmErrorStatus_Ok: Request successfully processed.
		 * NsmErrorStatus_Error: An error occured while processing the "LifecycleRequest".
		 */
		virtual NsmErrorStatus_e sendLifecycleRequestCompleteNSM(const uint32_t RequestId, const NsmErrorStatus_e status) =0;

	};
}
#endif // !defined(EA_D485A61C_B7C5_4803_B4BD_2AD92893E9CA__INCLUDED_)