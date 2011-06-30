/**
 * Copyright (C) 2011, BMW AG
 *
 * PlayerGUI
 *
 * \file player.h
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
 *
 * This Tool serves as test HMI interface in order to test the Genivi AudioManager. It is not intended to be production SW.
 *
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <gst/gst.h>
#include <glib.h>
#include <QString>
#include <QObject>
#include <phonon/phonon>
#include <QtGui/QWidget>
#include "qstandarditemmodel.h"
#include "ui_playergui.h"
#include "DBusSend.h"
#include "DBusTypes.h"

using namespace Phonon;

class player : public QObject {

Q_OBJECT

public:
	typedef enum e_playerType {
		simple,
		navigation,
		ta
	} playerType;

	player(const QString& file,Ui::playerguiClass ui,playerType type,QString appName);
	virtual ~player();

	void play(void);
	void stop(void);

public slots:
	void slot_numberOfSinksChanged();

private:
	MediaObject *music;
	QString m_play_file;
	DBusSend* sender;
	playerType m_type;
	QString m_appName;
	int m_interruptID;
	QList<SinkType> m_sinks;
	Ui_playerguiClass m_ui;
};

#endif /* PLAYER_H_ */
