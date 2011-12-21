/**
* Copyright (C) 2011, BMW AG
*
* GeniviAudioMananger
*
* \file  
*
* \date 20-Oct-2011 3:42:04 PM
* \author Christian Mueller (christian.ei.mueller@bmw.de)
*
* \section License
* GNU Lesser General Public License, version 2.1, with special exception (GENIVI clause)
* Copyright (C) 2011, BMW AG Christian M?ller  Christian.ei.mueller@bmw.de
*
* This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License, version 2.1, as published by the Free Software Foundation.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License, version 2.1, for more details.
* You should have received a copy of the GNU Lesser General Public License, version 2.1, along with this program; if not, see <http://www.gnu.org/licenses/lgpl-2.1.html>.
* Note that the copyright holders assume that the GNU Lesser General Public License, version 2.1, may also be applicable to programs even in cases in which the program is not a library in the technical sense.
* Linking AudioManager statically or dynamically with other modules is making a combined work based on AudioManager. You may license such other modules under the GNU Lesser General Public License, version 2.1. If you do not want to license your linked modules under the GNU Lesser General Public License, version 2.1, you may use the program under the following exception.
* As a special exception, the copyright holders of AudioManager give you permission to combine AudioManager with software programs or libraries that are released under any license unless such a combination is not permitted by the license of such a software program or library. You may copy and distribute such a system following the terms of the GNU Lesser General Public License, version 2.1, including this special exception, for AudioManager and the licenses of the other code concerned.
* Note that people who make modified versions of AudioManager are not obligated to grant this special exception for their modified versions; it is their choice whether to do so. The GNU Lesser General Public License, version 2.1, gives permission to release a modified version without this exception; this exception also makes it possible to release a modified version which carries forward this exception.
*
* THIS CODE HAS BEEN GENERATED BY ENTERPRISE ARCHITECT GENIVI MODEL. PLEASE CHANGE ONLY IN ENTERPRISE ARCHITECT AND GENERATE AGAIN
*/
#if !defined(EA_D28BAC35_CEA8_4ec8_AF26_00050F6CD9FA__INCLUDED_)
#define EA_D28BAC35_CEA8_4ec8_AF26_00050F6CD9FA__INCLUDED_

#include <vector>
#include <string>
#include "../audiomanagertypes.h"
#include "CommandReceiveInterface.h"

#include "CommandReceiveInterface.h"

namespace am {
	/**
	 * This interface handles all communication from the AudioManagerDaemon towards the system. It is designed in such a way that only callbacks with no return types are implemented. So when the CommandInterfacePlugins are designed in such a way that they broadcast signals to any node who is interested in the particular information (like signals on Dbus for example), more information can be retrieved via the CommandReceiveInterface.
	 * @author christian
	 * @version 1.0
	 * @created 22-Dec-2011 12:55:17 AM
	 */
	class CommandSendInterface
	{

	public:
		/**
		 * This command starts the interface, the plugin itself. This is not meant to start communication with the HMI itself. It is a good idea to implement here everything that sets up the basic communication like DbusCommunication etc...
		 * @return E_OK on success, E_UNKNOWN on error
		 * 
		 * @param commandreceiveinterface    pointer to the receive interface. Is used to call the audiomanagerdaemon
		 */
		virtual am_Error_e startupInterface(CommandReceiveInterface* commandreceiveinterface) =0;
		/**
		 * This command stops the interface before the plugin is unloaded.
		 * @return E_OK on success, E_UNKNOWN on error
		 */
		virtual am_Error_e stopInterface() =0;
		/**
		 * This callback is fired when the Interface is ready to be used. Before this command, all communication will be ignored by the Audiomanager
		 */
		virtual am_Error_e cbCommunicationReady() =0;
		/**
		 * This callback is fired when the AudioManager is about to rundown. After this command no more action will be carried out by the AudioManager.
		 */
		virtual am_Error_e cbCommunicationRundown() =0;
		/**
		 * Callback that is called when the number of connections change
		 */
		virtual void cbNumberOfMainConnectionsChanged() =0;
		/**
		 * Callback that is called when the number of sinks change
		 */
		virtual void cbNumberOfSinksChanged() =0;
		/**
		 * Callback that is called when the number of sources change
		 */
		virtual void cbNumberOfSourcesChanged() =0;
		/**
		 * this callback is fired if the number of sink classes changed
		 */
		virtual void cbNumberOfSinkClassesChanged() =0;
		/**
		 * this callback is fired if the number of source classes changed
		 */
		virtual void cbNumberOfSourceClassesChanged() =0;
		/**
		 * This callback is called when the ConnectionState of a connection changed.
		 * 
		 * @param connectionID
		 * @param connectionState
		 */
		virtual void cbMainConnectionStateChanged(const am_mainConnectionID_t connectionID, const am_ConnectionState_e connectionState) =0;
		/**
		 * this callback indicates that a sinkSoundProperty has changed.
		 * 
		 * @param sinkID
		 * @param soundProperty
		 */
		virtual void cbMainSinkSoundPropertyChanged(const am_sinkID_t sinkID, const am_MainSoundProperty_s soundProperty) =0;
		/**
		 * this callback indicates that a sourceSoundProperty has changed.
		 * 
		 * @param sourceID
		 * @param soundProperty
		 */
		virtual void cbMainSourceSoundPropertyChanged(const am_sourceID_t sourceID, const am_MainSoundProperty_s& soundProperty) =0;
		/**
		 * this callback is called when the availability of a sink has changed
		 * 
		 * @param sinkID
		 * @param availability
		 */
		virtual void cbSinkAvailabilityChanged(const am_sinkID_t sinkID, const am_Availability_s& availability) =0;
		/**
		 * this callback is called when the availability of source has changed.
		 * 
		 * @param sourceID
		 * @param availability
		 */
		virtual void cbSourceAvailabilityChanged(const am_sourceID_t sourceID, const am_Availability_s& availability) =0;
		/**
		 * this callback indicates a volume change on the indicated sink
		 * 
		 * @param sinkID
		 * @param volume
		 */
		virtual void cbVolumeChanged(const am_sinkID_t sinkID, const am_mainVolume_t volume) =0;
		/**
		 * this callback indicates a mute state change on a sink.
		 * 
		 * @param sinkID
		 * @param muteState
		 */
		virtual void cbSinkMuteStateChanged(const am_sinkID_t sinkID, const am_MuteState_e muteState) =0;
		/**
		 * is fired if a systemProperty changed
		 * 
		 * @param systemProperty
		 */
		virtual void cbSystemPropertyChanged(const am_SystemProperty_s& systemProperty) =0;
		/**
		 * this callback is called when the timinginformation for a mainconnection has changed.
		 * 
		 * @param mainConnection
		 * @param time
		 */
		virtual void cbTimingInformationChanged(const am_mainConnectionID_t mainConnection, const am_timeSync_t time) =0;

	};
}
#endif // !defined(EA_D28BAC35_CEA8_4ec8_AF26_00050F6CD9FA__INCLUDED_)
