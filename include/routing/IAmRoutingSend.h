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
#if !defined(EA_3921A27D_CF17_4db4_BE29_6A5BD7C5FE53__INCLUDED_)
#define EA_3921A27D_CF17_4db4_BE29_6A5BD7C5FE53__INCLUDED_

#include <vector>
#include <string>
#include "audiomanagertypes.h"

namespace am {
class IAmRoutingReceive;
}

#include "audiomanagertypes.h"
#include "IAmRoutingReceive.h"
#include "projecttypes.h"

#define RoutingSendVersion "2.0" 
namespace am {
	/**
	 * This class implements everything from Audiomanager -> RoutingAdapter
	 * There are two rules that have to be kept in mind when implementing against this interface:\n
	 * \warning
	 * 1. CALLS TO THIS INTERFACE ARE NOT THREAD SAFE !!!! \n
	 * 2. YOU MAY NOT CALL THE CALLING INTERFACE DURING AN SYNCHRONOUS OR ASYNCHRONOUS CALL THAT EXPECTS A RETURN VALUE.\n
	 * \details
	 * Violation these rules may lead to unexpected behavior! Nevertheless you can implement thread safe by using the deferred-
	 * call pattern described on the wiki which also helps to implement calls that are forbidden.\n
	 * For more information, please check CAmSerializer
	 * @author Christian Mueller
	 * @created 16-Dez-2012 15:58:16
	 */
	class IAmRoutingSend
	{

	public:
		IAmRoutingSend() {

		}

		virtual ~IAmRoutingSend() {

		}

		/**
		 * This function returns the version of the interface
		 * 
		 * @param version
		 */
		virtual void getInterfaceVersion(std::string& version) const =0;
		/**
		 * starts up the interface. In the implementations, here is the best place for init routines.
		 * 
		 * @param routingreceiveinterface    pointer to the receive interface
		 */
		virtual am_Error_e startupInterface(IAmRoutingReceive* routingreceiveinterface) =0;
		/**
		 * indicates that the routing now ready to be used. Should be used as trigger to register all sinks, sources, etc...
		 * 
		 * @param handle    handle that uniquely identifies the request
		 */
		virtual void setRoutingReady(const uint16_t handle) =0;
		/**
		 * indicates that the routing plugins need to be prepared to switch the power off or be ready again.
		 * 
		 * @param handle    the handle that uniquely identifies the request
		 */
		virtual void setRoutingRundown(const uint16_t handle) =0;
		/**
		 * aborts an asynchronous action.
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if handle was not found
		 * 
		 * @param handle
		 */
		virtual am_Error_e asyncAbort(const am_Handle_s handle) =0;
		/**
		 * connects a source to a sink
		 * @return E_OK on success, E_UNKNOWN on error, E_WRONG_FORMAT in case am_ConnectionFormat_e does not match
		 * 
		 * @param handle
		 * @param connectionID
		 * @param sourceID
		 * @param sinkID
		 * @param connectionFormat
		 */
		virtual am_Error_e asyncConnect(const am_Handle_s handle, const am_connectionID_t connectionID, const am_sourceID_t sourceID, const am_sinkID_t sinkID, const am_ConnectionFormat_e connectionFormat) =0;
		/**
		 * disconnect a connection with given connectionID
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if connection was not found
		 * 
		 * @param handle
		 * @param connectionID
		 */
		virtual am_Error_e asyncDisconnect(const am_Handle_s handle, const am_connectionID_t connectionID) =0;
		/**
		 * this method is used to set the volume of a sink. This function is used to drive ramps, to mute or unmute or directly
		 * set the value. The difference is made through the ramptype.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE if new volume is out of range
		 * 
		 * @param handle
		 * @param sinkID
		 * @param volume
		 * @param ramp
		 * @param time
		 */
		virtual am_Error_e asyncSetSinkVolume(const am_Handle_s handle, const am_sinkID_t sinkID, const am_volume_t volume, const am_RampType_e ramp, const am_time_t time) =0;
		/**
		 * sets the volume of a source. This method is used to set the volume of a sink. This function is used to drive ramps, to
		 * mute or unmute or directly set the value. The difference is made through the ramptype.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE if volume is out of range.
		 * triggers the acknowledge ackSourceVolumeChange
		 * 
		 * @param handle
		 * @param sourceID
		 * @param volume
		 * @param ramp
		 * @param time
		 */
		virtual am_Error_e asyncSetSourceVolume(const am_Handle_s handle, const am_sourceID_t sourceID, const am_volume_t volume, const am_RampType_e ramp, const am_time_t time) =0;
		/**
		 * This function is used to set the source state of a particular source.
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param handle
		 * @param sourceID
		 * @param state
		 */
		virtual am_Error_e asyncSetSourceState(const am_Handle_s handle, const am_sourceID_t sourceID, const am_SourceState_e state) =0;
		/**
		 * this function sets the sinksoundproperty.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE in case the propery value is out of range
		 * 
		 * @param handle
		 * @param sinkID
		 * @param listSoundProperties
		 */
		virtual am_Error_e asyncSetSinkSoundProperties(const am_Handle_s handle, const am_sinkID_t sinkID, const std::vector<am_SoundProperty_s>& listSoundProperties) =0;
		/**
		 * this function sets the sinksoundproperty.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE in case the propery value is out of range
		 * 
		 * @param handle
		 * @param sinkID
		 * @param soundProperty
		 */
		virtual am_Error_e asyncSetSinkSoundProperty(const am_Handle_s handle, const am_sinkID_t sinkID, const am_SoundProperty_s& soundProperty) =0;
		/**
		 * this function sets the sourcesoundproperty.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE in case the propery value is out of range
		 * 
		 * @param handle
		 * @param sourceID
		 * @param listSoundProperties
		 */
		virtual am_Error_e asyncSetSourceSoundProperties(const am_Handle_s handle, const am_sourceID_t sourceID, const std::vector<am_SoundProperty_s>& listSoundProperties) =0;
		/**
		 * this function sets the sourcesoundproperty.
		 * @return E_OK on success, E_UNKNOWN on error, E_OUT_OF_RANGE in case the propery value is out of range
		 * 
		 * @param handle
		 * @param sourceID
		 * @param soundProperty
		 */
		virtual am_Error_e asyncSetSourceSoundProperty(const am_Handle_s handle, const am_sourceID_t sourceID, const am_SoundProperty_s& soundProperty) =0;
		/**
		 * this function triggers crossfading.
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param handle
		 * @param crossfaderID
		 * @param hotSink
		 * @param rampType
		 * @param time
		 */
		virtual am_Error_e asyncCrossFade(const am_Handle_s handle, const am_crossfaderID_t crossfaderID, const am_HotSink_e hotSink, const am_RampType_e rampType, const am_time_t time) =0;
		/**
		 * this function is used for early and late audio functions to set the domain state
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param domainID
		 * @param domainState
		 */
		virtual am_Error_e setDomainState(const am_domainID_t domainID, const am_DomainState_e domainState) =0;
		/**
		 * this method is used to retrieve the busname during startup of the plugin. Needs to be implemented
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param BusName
		 */
		virtual am_Error_e returnBusName(std::string& BusName) const =0;
		/**
		 * This command sets multiple source or and sink volumes within a domain at a time. It can be used to synchronize volume
		 * setting events.
		 * @return E_OK on success, E_UNKNOWN on error.
		 * 
		 * @param handle    the handle
		 * @param listVolumes    a list of volumes that shall be set a the same time
		 */
		virtual am_Error_e asyncSetVolumes(const am_Handle_s handle, const std::vector<am_Volumes_s>& listVolumes) =0;
		/**
		 * sets the notification configuration of a source.
		 * @return E_OK on success, E_UNKNOWN on error.
		 * 
		 * @param handle    The handle for this operation.
		 * @param sinkID    the sourceID of the source the notification should be set
		 * @param notificationConfiguration    The notification configuration
		 */
		virtual am_Error_e asyncSetSinkNotificationConfiguration(const am_Handle_s handle, const am_sinkID_t sinkID, const am_NotificationConfiguration_s& notificationConfiguration) =0;
		/**
		 * sets the notification configuration of a source.
		 * @return E_OK on success, E_UNKNOWN on error.
		 * 
		 * @param handle    The handle for this operation.
		 * @param sourceID    the sourceID of the source the notification should be set
		 * @param notificationConfiguration    The notification configuration
		 */
		virtual am_Error_e asyncSetSourceNotificationConfiguration(const am_Handle_s handle, const am_sourceID_t sourceID, const am_NotificationConfiguration_s& notificationConfiguration) =0;

	};
}
#endif // !defined(EA_3921A27D_CF17_4db4_BE29_6A5BD7C5FE53__INCLUDED_)
