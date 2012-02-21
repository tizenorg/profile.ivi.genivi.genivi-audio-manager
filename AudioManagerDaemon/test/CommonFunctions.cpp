/**
 * Copyright (C) 2011, BMW AG
 *
 * GeniviAudioMananger AudioManagerDaemon
 *
 * \file CommonFunctions.cpp
 *
 * \date 20-Oct-2011 3:42:04 PM
 * \author Christian Mueller (christian.ei.mueller@bmw.de)
 *
 * \section License
 * GNU Lesser General Public License, version 2.1, with special exception (GENIVI clause)
 * Copyright (C) 2011, BMW AG Christian Mueller  Christian.ei.mueller@bmw.de
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

#include "CommonFunctions.h"
#include "CommandInterfaceBackdoor.h"
#include "RoutingInterfaceBackdoor.h"
#include "ControlInterfaceBackdoor.h"
#include <cassert>
#include <sstream>

using namespace am;

CommandInterfaceBackdoor::CommandInterfaceBackdoor()
{
}
CommandInterfaceBackdoor::~CommandInterfaceBackdoor()
{
}

bool CommandInterfaceBackdoor::unloadPlugins(CommandSender *CommandSender)
{
    assert(CommandSender != NULL);
    CommandSender->unloadLibraries();
    CommandSender->mListInterfaces.clear();
    if (CommandSender->mListInterfaces.empty())
        return true;
    return false;
}

bool CommandInterfaceBackdoor::injectInterface(CommandSender *CommandSender, CommandSendInterface *CommandSendInterface)
{
    assert(CommandSender != NULL);
    assert(CommandSendInterface != NULL);
    CommandSender->mListInterfaces.push_back(CommandSendInterface);
    return true;
}

RoutingInterfaceBackdoor::RoutingInterfaceBackdoor()
{
}
RoutingInterfaceBackdoor::~RoutingInterfaceBackdoor()
{
}

bool RoutingInterfaceBackdoor::unloadPlugins(RoutingSender *RoutingSender)
{
    assert(RoutingSender != NULL);
    RoutingSender->unloadLibraries();
    RoutingSender->mListInterfaces.clear();
    if (RoutingSender->mListInterfaces.empty())
        return true;
    return false;
}

bool RoutingInterfaceBackdoor::injectInterface(RoutingSender *RoutingSender, RoutingSendInterface *newInterface, const std::string& busname)
{
    assert(RoutingSender != NULL);
    assert(newInterface != NULL);

    RoutingSender::InterfaceNamePairs newInterfacePair;
    newInterfacePair.routingInterface = newInterface;
    newInterfacePair.busName = busname;
    RoutingSender->mListInterfaces.push_back(newInterfacePair);
    return true;
}

ControlInterfaceBackdoor::ControlInterfaceBackdoor()
{
}

ControlInterfaceBackdoor::~ControlInterfaceBackdoor()
{
}

bool ControlInterfaceBackdoor::replaceController(ControlSender *controlSender, ControlSendInterface *newController)
{
    controlSender->mController = newController;
    return true;
}

//int GetRandomNumber(int nLow, int nHigh) {
//	return (rand() % (nHigh - nLow + 1)) + nLow;
//}

bool equalSoundProperty(const am_SoundProperty_s a, const am_SoundProperty_s b)
{
    return (a.type == b.type && a.value == b.value);
}

bool equalMainSoundProperty(const am_MainSoundProperty_s a, const am_MainSoundProperty_s b)
{
    return (a.type == b.type && a.value == b.value);
}

bool equalRoutingElement(const am_RoutingElement_s a, const am_RoutingElement_s b)
{
    return (a.connectionFormat == b.connectionFormat && a.domainID == b.domainID && a.sinkID == b.sinkID && a.sourceID == b.sourceID);
}

bool equalClassProperties(const am_ClassProperty_s a, const am_ClassProperty_s b)
{
    return (a.classProperty == b.classProperty && a.value == b.value);
}

std::string int2string(int i)
{
    std::stringstream out;
    out << i;
    return out.str();
}

bool CommonFunctions::compareSource(std::vector<am_Source_s>::iterator listIterator, const am_Source_s& sourceData)
{
    return (listIterator->available.availability == sourceData.available.availability) && (listIterator->available.availabilityReason == sourceData.available.availabilityReason) && (listIterator->sourceClassID == sourceData.sourceClassID) && (listIterator->domainID == sourceData.domainID) && (listIterator->interruptState == sourceData.interruptState) && (listIterator->visible == sourceData.visible) && (listIterator->name.compare(sourceData.name) == 0) && (listIterator->volume == sourceData.volume) && std::equal(listIterator->listConnectionFormats.begin(), listIterator->listConnectionFormats.end(), sourceData.listConnectionFormats.begin())
            && std::equal(listIterator->listMainSoundProperties.begin(), listIterator->listMainSoundProperties.end(), sourceData.listMainSoundProperties.begin(), equalMainSoundProperty) && std::equal(listIterator->listSoundProperties.begin(), listIterator->listSoundProperties.end(), sourceData.listSoundProperties.begin(), equalSoundProperty);
}

bool CommonFunctions::compareSink(std::vector<am_Sink_s>::iterator listIterator, const am_Sink_s& sinkData)
{
    return (listIterator->available.availability == sinkData.available.availability) && (listIterator->available.availabilityReason == sinkData.available.availabilityReason) && (listIterator->sinkClassID == sinkData.sinkClassID) && (listIterator->domainID == sinkData.domainID) && (listIterator->mainVolume == sinkData.mainVolume) && (listIterator->muteState == sinkData.muteState) && (listIterator->visible == sinkData.visible) && (listIterator->name.compare(sinkData.name) == 0) && (listIterator->volume == sinkData.volume) && std::equal(listIterator->listConnectionFormats.begin(), listIterator->listConnectionFormats.end(), sinkData.listConnectionFormats.begin())
            && std::equal(listIterator->listMainSoundProperties.begin(), listIterator->listMainSoundProperties.end(), sinkData.listMainSoundProperties.begin(), equalMainSoundProperty) && std::equal(listIterator->listSoundProperties.begin(), listIterator->listSoundProperties.end(), sinkData.listSoundProperties.begin(), equalSoundProperty);
}

bool CommonFunctions::compareGateway(std::vector<am_Gateway_s>::iterator listIterator, const am_Gateway_s& gatewayData)
{
    return (listIterator->name.compare(gatewayData.name) == 0) && (listIterator->sinkID == gatewayData.sinkID) && (listIterator->sourceID == gatewayData.sourceID) && (listIterator->controlDomainID == gatewayData.controlDomainID) && (listIterator->domainSinkID == gatewayData.domainSinkID) && (listIterator->domainSourceID == gatewayData.domainSourceID) && std::equal(listIterator->convertionMatrix.begin(), listIterator->convertionMatrix.end(), gatewayData.convertionMatrix.begin()) && std::equal(listIterator->listSourceFormats.begin(), listIterator->listSourceFormats.end(), gatewayData.listSourceFormats.begin()) && std::equal(listIterator->listSinkFormats.begin(), listIterator->listSinkFormats.end(), gatewayData.listSinkFormats.begin());
}

bool CommonFunctions::compareGateway1(const am_Gateway_s gateway1, const am_Gateway_s gatewayData)
{
    return (gateway1.name.compare(gatewayData.name) == 0) && (gateway1.sinkID == gatewayData.sinkID) && (gateway1.sourceID == gatewayData.sourceID) && (gateway1.controlDomainID == gatewayData.controlDomainID) && (gateway1.domainSinkID == gatewayData.domainSinkID) && (gateway1.domainSourceID == gatewayData.domainSourceID) && std::equal(gateway1.convertionMatrix.begin(), gateway1.convertionMatrix.end(), gatewayData.convertionMatrix.begin()) && std::equal(gateway1.listSourceFormats.begin(), gateway1.listSourceFormats.end(), gatewayData.listSourceFormats.begin()) && std::equal(gateway1.listSinkFormats.begin(), gateway1.listSinkFormats.end(), gatewayData.listSinkFormats.begin());
}

bool CommonFunctions::compareSinkMainSink(std::vector<am_SinkType_s>::iterator listIterator, const std::vector<am_Sink_s>& sinkList)
{
    std::vector<am_Sink_s>::const_iterator sinkListIterator = sinkList.begin();
    for (; sinkListIterator < sinkList.end(); ++sinkListIterator)
    {
        if (listIterator->sinkID == sinkListIterator->sinkID)
        {
            return (listIterator->name.compare(sinkListIterator->name) == 0) && (listIterator->availability.availability == sinkListIterator->available.availability) && (listIterator->availability.availabilityReason == sinkListIterator->available.availabilityReason) && (listIterator->muteState == sinkListIterator->muteState) && (listIterator->volume == sinkListIterator->mainVolume) && (listIterator->sinkClassID == sinkListIterator->sinkClassID);
        }
    }
    return false;
}

bool CommonFunctions::compareSinkMainSource(std::vector<am_SourceType_s>::iterator listIterator, const std::vector<am_Source_s>& sourceList)
{
    std::vector<am_Source_s>::const_iterator sinkListIterator = sourceList.begin();
    for (; sinkListIterator < sourceList.end(); ++sinkListIterator)
    {
        if (listIterator->sourceID == sinkListIterator->sourceID)
        {
            return (listIterator->name.compare(sinkListIterator->name) == 0) && (listIterator->availability.availability == sinkListIterator->available.availability) && (listIterator->availability.availabilityReason == sinkListIterator->available.availabilityReason) && (listIterator->sourceClassID == sinkListIterator->sourceClassID);
        }
    }
    return false;
}

bool CommonFunctions::compareRoute(am_Route_s a, am_Route_s b)
{
    bool retVal = true;
    std::vector<am_RoutingElement_s>::iterator itA = a.route.begin(), itB = b.route.begin();
    retVal &= a.sourceID == b.sourceID;
    retVal &= a.sinkID == b.sinkID;
    for (; itA != a.route.end(); ++itA)
    {
        retVal &= itA->sinkID == itB->sinkID;
        retVal &= itA->sourceID == itB->sourceID;
        retVal &= itA->connectionFormat == itB->connectionFormat;
        retVal &= itA->domainID == itB->domainID;
        itB++;
    }
    return retVal;
}

std::vector<am_ConnectionFormat_e> CommonFunctions::getStandardConnectionFormatList()
{
    std::vector<am_ConnectionFormat_e> list;
    list.push_back(CF_GENIVI_ANALOG);
    list.push_back(CF_GENIVI_STEREO);
    return list;
}

std::vector<am_SoundProperty_s> CommonFunctions::getStandardSoundPropertyList()
{
    std::vector<am_SoundProperty_s> soundPropertyList;
    am_SoundProperty_s soundProperty;
    soundProperty.type = SP_EXAMPLE_BASS;
    soundProperty.value = 23;
    soundPropertyList.push_back(soundProperty);
    soundProperty.type = SP_EXAMPLE_MID;
    soundProperty.value = 2;
    soundPropertyList.push_back(soundProperty);
    return soundPropertyList;
}

std::vector<am_MainSoundProperty_s> CommonFunctions::getStandardMainSoundPropertyList()
{
    std::vector<am_MainSoundProperty_s> mainSoundPropertyList;
    am_MainSoundProperty_s mainSoundProperty;
    mainSoundProperty.type = MSP_UNKNOWN;
    mainSoundProperty.value = 23;
    mainSoundPropertyList.push_back(mainSoundProperty);
    mainSoundProperty.type = MSP_UNKNOWN;
    mainSoundProperty.value = 3;
    mainSoundPropertyList.push_back(mainSoundProperty);
    return mainSoundPropertyList;
}

void CommonFunctions::createSink(am_Sink_s& sink) const
{
    sink.name = "AnySink";
    sink.domainID = 4;
    sink.available.availability = A_AVAILABLE;
    sink.available.availabilityReason = AR_GENIVI_NEWMEDIA;
    sink.sinkClassID = 1;
    sink.listConnectionFormats = getStandardConnectionFormatList();
    sink.listSoundProperties = getStandardSoundPropertyList();
    sink.listMainSoundProperties = getStandardMainSoundPropertyList();
    sink.mainVolume = 12;
    sink.muteState = MS_UNMUTED;
    sink.visible = true;
    sink.volume = 23;
    sink.sinkID = 0;
}

void CommonFunctions::createSource(am_Source_s& source) const
{
    source.name = "AnySource";
    source.domainID = 4;
    source.available.availability = A_AVAILABLE;
    source.available.availabilityReason = AR_GENIVI_NEWMEDIA;
    source.sourceClassID = 1;
    source.listConnectionFormats = getStandardConnectionFormatList();
    source.listSoundProperties = getStandardSoundPropertyList();
    source.listMainSoundProperties = getStandardMainSoundPropertyList();
    source.interruptState = IS_OFF;
    source.visible = true;
    source.volume = 23;
    source.sourceID = 0;
    source.sourceState = SS_ON;
}

void CommonFunctions::createDomain(am_Domain_s & domain) const
{

    domain.domainID = 0;
    domain.name = "AnyDomain";
    domain.nodename = "AnyNode";
    domain.busname = "AnyBusname";
    domain.complete = true;
    domain.early = true;
    domain.state = DS_CONTROLLED;
}

void CommonFunctions::createGateway(am_Gateway_s & gateway)
{
    gateway.name = "AnyGateway";
    gateway.sinkID = 1;
    gateway.sourceID = 2;
    gateway.controlDomainID = 1;
    gateway.domainSinkID = 3;
    gateway.domainSourceID = 4;
    gateway.convertionMatrix = getStandardConvertionMatrix();
    gateway.listSourceFormats = getStandardConnectionFormatList();
    gateway.listSinkFormats = getStandardConnectionFormatList();
    gateway.gatewayID = 0;

}

void CommonFunctions::createConnection(am_Connection_s & connection) const
{
    connection.connectionID = 0;
    connection.sinkID = 1;
    connection.sourceID = 2;
    connection.delay = -1;
    connection.connectionFormat = CF_GENIVI_ANALOG;
}

std::vector<bool> CommonFunctions::getStandardConvertionMatrix()
{
    std::vector<bool> convMatrix;
    convMatrix.push_back(true);
    convMatrix.push_back(false);
    convMatrix.push_back(true);
    convMatrix.push_back(false);
    convMatrix.push_back(true);
    convMatrix.push_back(true);
    return convMatrix;
}

void CommonFunctions::connectionList2RoutingList(std::vector<am_RoutingElement_s> & routingList, const std::vector<am_Connection_s>& connectionList)
{
    am_RoutingElement_s routingElement;
    std::vector<am_Connection_s>::const_iterator cIterator = connectionList.begin();
    for (; cIterator < connectionList.end(); ++cIterator)
    {
        routingElement.sinkID = cIterator->sinkID;
        routingElement.sourceID = cIterator->sourceID;
        routingElement.connectionFormat = cIterator->connectionFormat;
        routingElement.domainID = 4; //todo: make this test read out the real value
        routingList.push_back(routingElement);
    }
}

