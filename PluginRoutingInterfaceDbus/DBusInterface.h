/**
 *
 * Copyright (C) 2011, BMW AG
 *
 * PluginDBus
 *
 * \file DBusInterface.h
 *
 * \date 20.05.2011
 * \author Christian Müller (christian.ei.mueller@bmw.de)
 *
 * \section License
 * GNU Lesser General Public License, version 2.1, with special exception (GENIVI clause)
 * Copyright (C) 2011, BMW AG – Christian Müller  Christian.ei.mueller@bmw.de
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License, version 2.1, as published by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License, version 2.1, for more details.
 * You should have received a copy of the GNU Lesser General Public License, version 2.1, along with this program; if not, see <http://www.gnu.org/licenses/lgpl-2.1.html>.
 * Note that the copyright holders assume that the GNU Lesser General Public License, version 2.1, may also be applicable to programs even in cases in which the program is not a library in the technical sense.
 * Linking AudioManager statically or dynamically with other modules is making a combined work based on AudioManager. You may license such other modules under the GNU Lesser General Public License, version 2.1. If you do not want to license your linked modules under the GNU Lesser General Public License, version 2.1, you may use the program under the following exception.
 * As a special exception, the copyright holders of AudioManager give you permission to combine AudioManager with software programs or libraries that are released under any license unless such a combination is not permitted by the license of such a software program or library. You may copy and distribute such a system following the terms of the GNU Lesser General Public License, version 2.1, including this special exception, for AudioManager and the licenses of the other code concerned.
 * Note that people who make modified versions of AudioManager are not obligated to grant this special exception for their modified versions; it is their choice whether to do so. The GNU Lesser General Public License, version 2.1, gives permission to release a modified version without this exception; this exception also makes it possible to release a modified version which carries forward this exception.
 */

#ifndef DBUSINTERFACE_H_
#define DBUSINTERFACE_H_

#include "DBusInterfaceAdaptor.h"
#include "routinginterface.h"

class DBusInterface : public QObject {
	Q_OBJECT
public:
	DBusInterface(QObject *parent = 0);
	void setReceiverInterface(RoutingReceiveInterface* r_interface);
	void emitSystemReady();

public Q_SLOTS: // METHODS
	int peekDomain(const QString &name);
    int registerDomain(const QString &name, const QString &node, bool earlymode);
    int registerGateway(const QString &name, const QString &sink, const QString &source, const QString &domainSource, const QString &domainSink, const QString &controlDomain);
    int registerSink(const QString &name, const QString &sinkclass, const QString &domain);
    int registerSource(const QString &name, const QString &audioclass, const QString &domain);
Q_SIGNALS: // SIGNALS
    void signal_systemReady();

private:
	RoutingReceiveInterface* audiomanager;
};

#endif /* DBUSINTERFACE_H_ */
