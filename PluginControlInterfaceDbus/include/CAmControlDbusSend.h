/**
 *  Copyright (c) copyright 2011-2012 Aricent® Group  and its licensors
 *  Copyright (c) 2012 BMW
 *
 *  \author Sampreeth Ramavana
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

#ifndef _CAMCONTROLDBUSSEND_H_
#define _CAMCONTROLDBUSSEND_H_

#include <dbus/dbus.h>
#include <string>
#include <vector>
#include "audiomanagertypes.h"

namespace am
{

class CAmControlDbusSend
{
public:
    CAmControlDbusSend(DBusConnection* conn, std::string bus_name, std::string  path, std::string  interface, std::string  method);
    virtual ~CAmControlDbusSend();
    void append(std::string string);
    void append(uint16_t integer);
    void append(int16_t integer);
    void append(std::vector<am_SoundProperty_s> listSoundProperties);
    void append(am_SoundProperty_s soundProperty);
    am_Error_e send();

private:
    DBusMessage* mpDbusMessage;
    DBusConnection* mpDbusConnection;
    DBusMessageIter mDbusMessageIter;
    DBusError mDBusError;
};
}

#endif /* _CAMCONTROLDBUSSEND_H_ */
