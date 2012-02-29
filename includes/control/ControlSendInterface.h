/** Copyright (c) 2012 GENIVI Alliance
 *  Copyright (c) 2012 BMW
 *
 *  @author Christian Mueller, BMW
 *
 *  @copyright
 *  {
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  }
 *
 * 
 * THIS CODE HAS BEEN GENERATED BY ENTERPRISE ARCHITECT GENIVI MODEL. PLEASE CHANGE ONLY IN ENTERPRISE ARCHITECT AND GENERATE AGAIN
 */
#if !defined(EA_3F77CFC8_1A30_4d2d_88FD_3E946AA5AB38__INCLUDED_)
#define EA_3F77CFC8_1A30_4d2d_88FD_3E946AA5AB38__INCLUDED_

#include <vector>
#include <string>
#include "audiomanagertypes.h"

namespace am {
class ControlReceiveInterface;
}

#define ControlSendVersion "1.0" 
namespace am {
	/**
	 * This interface is presented by the AudioManager controller.
	 * All the hooks represent system events that need to be handled. The callback functions are used to handle for example answers to function calls on the AudioManagerCoreInterface.
	 * There are two rules that have to be kept in mind when implementing against this interface:\n<b>
	 * 1. CALLS TO THIS INTERFACE ARE NOT THREAD SAFE !!!! \n
	 * 2. YOU MAY NOT THE CALLING INTERFACE DURING AN SYNCHRONOUS OR ASYNCHRONOUS CALL THAT EXPECTS A RETURN VALUE.</b>\n
	 * Violation these rules may lead to unexpected behavior! Nevertheless you can implement thread safe by using the deferred-call pattern described on the wiki which also helps to implement calls that are forbidden.\n
	 * For more information, please check CAmSerializer
	 * @author Christian Mueller
	 * @created 29-Feb-2012 12:54:30 PM
	 */
	class ControlSendInterface
	{

	public:
		ControlSendInterface() {

		}

		virtual ~ControlSendInterface() {

		}

		/**
		 * Starts up the controller.
		 * 
		 * @param controlreceiveinterface    This is a pointer to the ControlReceiveInterface so that the controller knows to whom to communicate.
		 */
		virtual am_Error_e startupController(ControlReceiveInterface* controlreceiveinterface) =0;
		/**
		 * this message is used tell the controller that it should get ready. This message must be acknowledged via confirmControllerReady.
		 */
		virtual void setControllerReady() =0;
		/**
		 * this message tells the controller that he should prepare everything for the power to be switched off. This message must be acknowledged via confirmControllerRundown.
		 */
		virtual void setControllerRundown() =0;
		/**
		 * is called when a connection request comes in via the command interface
		 * @return E_OK on success, E_NOT_POSSIBLE on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param sourceID
		 * @param sinkID
		 * @param mainConnectionID
		 */
		virtual am_Error_e hookUserConnectionRequest(const am_sourceID_t sourceID, const am_sinkID_t sinkID, am_mainConnectionID_t& mainConnectionID) =0;
		/**
		 * is called when a disconnection request comes in via the command interface
		 * @return E_OK on success, E_NOT_POSSIBLE on error, E_NON_EXISTENT if connection does not exists
		 * 
		 * @param connectionID
		 */
		virtual am_Error_e hookUserDisconnectionRequest(const am_mainConnectionID_t connectionID) =0;
		/**
		 * sets a user MainSinkSoundProperty
		 * @return E_OK on success, E_OUT_OF_RANGE if out of range, E_UNKNOWN on error
		 * 
		 * @param sinkID
		 * @param soundProperty
		 */
		virtual am_Error_e hookUserSetMainSinkSoundProperty(const am_sinkID_t sinkID, const am_MainSoundProperty_s& soundProperty) =0;
		/**
		 * sets a user MainSourceSoundProperty
		 * @return E_OK on success, E_OUT_OF_RANGE if out of range, E_UNKNOWN on error
		 * 
		 * @param sourceID
		 * @param soundProperty
		 */
		virtual am_Error_e hookUserSetMainSourceSoundProperty(const am_sourceID_t sourceID, const am_MainSoundProperty_s& soundProperty) =0;
		/**
		 * sets a user SystemProperty
		 * @return E_OK on success, E_OUT_OF_RANGE if out of range, E_UNKNOWN on error
		 * 
		 * @param property
		 */
		virtual am_Error_e hookUserSetSystemProperty(const am_SystemProperty_s& property) =0;
		/**
		 * sets a user volume
		 * @return E_OK on success, E_OUT_OF_RANGE if out of range, E_UNKNOWN on error
		 * 
		 * @param SinkID
		 * @param newVolume
		 */
		virtual am_Error_e hookUserVolumeChange(const am_sinkID_t SinkID, const am_mainVolume_t newVolume) =0;
		/**
		 * sets a user volume as increment 
		 * @return E_OK on success, E_OUT_OF_RANGE if out of range, E_UNKNOWN on error
		 * 
		 * @param SinkID
		 * @param increment    the steps
		 */
		virtual am_Error_e hookUserVolumeStep(const am_sinkID_t SinkID, const int16_t increment) =0;
		/**
		 * sets the mute state of a sink
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param sinkID
		 * @param muteState    true=muted
		 */
		virtual am_Error_e hookUserSetSinkMuteState(const am_sinkID_t sinkID, const am_MuteState_e muteState) =0;
		/**
		 * is called when a routing adaptor registers its domain
		 * @return E_OK on success, E_UNKNOWN on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param domainData    ID is omitted here since it has not been created yet
		 * @param domainID
		 */
		virtual am_Error_e hookSystemRegisterDomain(const am_Domain_s& domainData, am_domainID_t& domainID) =0;
		/**
		 * is called when a routing adaptor wants to derigister a domain
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if not found
		 * 
		 * @param domainID
		 */
		virtual am_Error_e hookSystemDeregisterDomain(const am_domainID_t domainID) =0;
		/**
		 * is called when a domain registered all the elements
		 * 
		 * @param domainID
		 */
		virtual void hookSystemDomainRegistrationComplete(const am_domainID_t domainID) =0;
		/**
		 * is called when a routing adaptor registers a sink
		 * @return E_OK on success, E_UNKNOWN on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param sinkData    Id is omitted here, since it has not been created yet
		 * @param sinkID
		 */
		virtual am_Error_e hookSystemRegisterSink(const am_Sink_s& sinkData, am_sinkID_t& sinkID) =0;
		/**
		 * is called when a routing adaptor deregisters a sink
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if not found
		 * 
		 * @param sinkID
		 */
		virtual am_Error_e hookSystemDeregisterSink(const am_sinkID_t sinkID) =0;
		/**
		 * is called when a routing adaptor registers a source
		 * @return E_OK on success, E_UNKNOWN on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param sourceData    ID is omitted here since it is not yet created
		 * @param sourceID
		 */
		virtual am_Error_e hookSystemRegisterSource(const am_Source_s& sourceData, am_sourceID_t& sourceID) =0;
		/**
		 * is called when a routing adaptor deregisters a source
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if not found
		 * 
		 * @param sourceID
		 */
		virtual am_Error_e hookSystemDeregisterSource(const am_sourceID_t sourceID) =0;
		/**
		 * is called when a routing adaptor registers a gateway
		 * @return E_OK on success, E_UNKNOWN on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param gatewayData    gatewayID is not set here since it is not created at this point of time
		 * @param gatewayID
		 */
		virtual am_Error_e hookSystemRegisterGateway(const am_Gateway_s& gatewayData, am_gatewayID_t& gatewayID) =0;
		/**
		 * is called when a routing adaptor deregisters a gateway
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if not found
		 * 
		 * @param gatewayID
		 */
		virtual am_Error_e hookSystemDeregisterGateway(const am_gatewayID_t gatewayID) =0;
		/**
		 * is called when a routing adaptor registers a crossfader
		 * @return E_OK on success, E_UNKNOWN on error, E_ALREADY_EXISTENT if already exists
		 * 
		 * @param crossfaderData    gatewayID is not set here since it is not created at this point of time
		 * @param crossfaderID
		 */
		virtual am_Error_e hookSystemRegisterCrossfader(const am_Crossfader_s& crossfaderData, am_crossfaderID_t& crossfaderID) =0;
		/**
		 * is called when a routing adaptor deregisters a crossfader
		 * @return E_OK on success, E_UNKNOWN on error, E_NON_EXISTENT if not found
		 * 
		 * @param crossfaderID
		 */
		virtual am_Error_e hookSystemDeregisterCrossfader(const am_crossfaderID_t crossfaderID) =0;
		/**
		 * volumeticks. therse are used to indicate volumechanges during a ramp
		 * 
		 * @param handle
		 * @param sinkID
		 * @param volume
		 */
		virtual void hookSystemSinkVolumeTick(const am_Handle_s handle, const am_sinkID_t sinkID, const am_volume_t volume) =0;
		/**
		 * volumeticks. therse are used to indicate volumechanges during a ramp
		 * 
		 * @param handle
		 * @param sourceID
		 * @param volume
		 */
		virtual void hookSystemSourceVolumeTick(const am_Handle_s handle, const am_sourceID_t sourceID, const am_volume_t volume) =0;
		/**
		 * is called when an low level interrupt changed its state
		 * 
		 * @param sourceID
		 * @param interruptState
		 */
		virtual void hookSystemInterruptStateChange(const am_sourceID_t sourceID, const am_InterruptState_e interruptState) =0;
		/**
		 * id called when a sink changed its availability
		 * 
		 * @param sinkID
		 * @param availability
		 */
		virtual void hookSystemSinkAvailablityStateChange(const am_sinkID_t sinkID, const am_Availability_s& availability) =0;
		/**
		 * id called when a source changed its availability
		 * 
		 * @param sourceID
		 * @param availability
		 */
		virtual void hookSystemSourceAvailablityStateChange(const am_sourceID_t sourceID, const am_Availability_s& availability) =0;
		/**
		 * id called when domainstate was changed
		 * 
		 * @param domainID
		 * @param state
		 */
		virtual void hookSystemDomainStateChange(const am_domainID_t domainID, const am_DomainState_e state) =0;
		/**
		 * when early data was received
		 * 
		 * @param data
		 */
		virtual void hookSystemReceiveEarlyData(const std::vector<am_EarlyData_s>& data) =0;
		/**
		 * this hook provides information about speed changes.
		 * The quantization and sampling rate of the speed can be adjusted at compile time of the AudioManagerDaemon.
		 * 
		 * @param speed
		 */
		virtual void hookSystemSpeedChange(const am_speed_t speed) =0;
		/**
		 * this hook is fired whenever the timing information of a mainconnection has changed.
		 * 
		 * @param mainConnectionID
		 * @param time
		 */
		virtual void hookSystemTimingInformationChanged(const am_mainConnectionID_t mainConnectionID, const am_timeSync_t time) =0;
		/**
		 * ack for connect
		 * 
		 * @param handle
		 * @param errorID
		 */
		virtual void cbAckConnect(const am_Handle_s handle, const am_Error_e errorID) =0;
		/**
		 * ack for disconnect
		 * 
		 * @param handle
		 * @param errorID
		 */
		virtual void cbAckDisconnect(const am_Handle_s handle, const am_Error_e errorID) =0;
		/**
		 * ack for crossfading
		 * 
		 * @param handle
		 * @param hostsink
		 * @param error
		 */
		virtual void cbAckCrossFade(const am_Handle_s handle, const am_HotSink_e hostsink, const am_Error_e error) =0;
		/**
		 * ack for sink volume changes
		 * 
		 * @param handle
		 * @param volume
		 * @param error
		 */
		virtual void cbAckSetSinkVolumeChange(const am_Handle_s handle, const am_volume_t volume, const am_Error_e error) =0;
		/**
		 * ack for source volume changes
		 * 
		 * @param handle    the handle that is connected to the volume change
		 * @param voulme    the volume after the action ended (the desired volume if everything went right, the actual one in case of abortion)
		 * @param error
		 */
		virtual void cbAckSetSourceVolumeChange(const am_Handle_s handle, const am_volume_t voulme, const am_Error_e error) =0;
		/**
		 * ack for setting of source states
		 * 
		 * @param handle
		 * @param error
		 */
		virtual void cbAckSetSourceState(const am_Handle_s handle, const am_Error_e error) =0;
		/**
		 * ack for setting of sourcesoundproperties
		 * 
		 * @param handle
		 * @param error
		 */
		virtual void cbAckSetSourceSoundProperties(const am_Handle_s handle, const am_Error_e error) =0;
		/**
		 * ack for setting of sourcesoundproperties
		 * 
		 * @param handle
		 * @param error
		 */
		virtual void cbAckSetSourceSoundProperty(const am_Handle_s handle, const am_Error_e error) =0;
		/**
		 * ack for setting of sinksoundproperties
		 * 
		 * @param handle
		 * @param error
		 */
		virtual void cbAckSetSinkSoundProperties(const am_Handle_s handle, const am_Error_e error) =0;
		/**
		 * ack for setting of sinksoundproperties
		 * 
		 * @param handle
		 * @param error
		 */
		virtual void cbAckSetSinkSoundProperty(const am_Handle_s handle, const am_Error_e error) =0;
		/**
		 * This function is used by the routing algorithm to retrieve a priorized list of connectionFormats from the Controller.
		 * @return E_OK in case of successfull priorisation.
		 * 
		 * @param sourceID    sourceID of source that shall be connected
		 * @param sinkID    sinkID of sink that shall be connected
		 * @param listRoute    This route is the one the priorized connectionFormats is for.
		 * @param listPossibleConnectionFormats    list of possible connectionformats
		 * @param listPrioConnectionFormats    the list return with prioos from the controller. Best choice on first position.
		 */
		virtual am_Error_e getConnectionFormatChoice(const am_sourceID_t sourceID, const am_sinkID_t sinkID, const am_Route_s listRoute, const std::vector<am_ConnectionFormat_e> listPossibleConnectionFormats, std::vector<am_ConnectionFormat_e>& listPrioConnectionFormats) =0;
		/**
		 * This function returns the version of the interface
		 * returns E_OK, E_UNKOWN if version is unknown.
		 * 
		 * @param version
		 */
		virtual void getInterfaceVersion(std::string& version) const =0;
		/**
		 * confirms the setCommandReady call
		 */
		virtual void confirmCommandReady() =0;
		/**
		 * confirms the setRoutingReady call
		 */
		virtual void confirmRoutingReady() =0;
		/**
		 * confirms the setCommandRundown call
		 */
		virtual void confirmCommandRundown() =0;
		/**
		 * confirms the setRoutingRundown command
		 */
		virtual void confirmRoutingRundown() =0;

	};
}
#endif // !defined(EA_3F77CFC8_1A30_4d2d_88FD_3E946AA5AB38__INCLUDED_)
