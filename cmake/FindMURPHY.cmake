############################################################################
#
# Copyright 2014 Intel
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#		http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
############################################################################

FIND_PATH(MURPHY_INCLUDE_DIR murphy/common.h
/usr/include
)

FIND_LIBRARY(MURPHY_LIBRARY
NAMES murphy-common
PATHS /lib
)

FIND_LIBRARY(DOMCTL_LIBRARY
NAMES murphy-domain-controller
PATHS /lib
)

FIND_LIBRARY(MURPHY_DBUS_LIBRARY
NAMES murphy-dbus-libdbus
PATHS /lib
)

SET( MURPHY_FOUND "NO" )

IF(MURPHY_LIBRARY)
    IF(DOMCTL_LIBRARY)
        IF(MURPHY_DBUS_LIBRARY)
          SET( MURPHY_FOUND "YES" )
          SET( MURPHY_LIBRARY "${DOMCTL_LIBRARY}" "${MURPHY_LIBRARY}" "${MURPHY_DBUS_LIBRARY}")
          message(STATUS "Found MURPHY libs: ${MURPHY_LIBRARY}")
          message(STATUS "Found MURPHY include: ${MURPHY_INCLUDE_DIR}")
      ENDIF(MURPHY_DBUS_LIBRARY)
  ENDIF(DOMCTL_LIBRARY)
ENDIF(MURPHY_LIBRARY)

MARK_AS_ADVANCED(
  MURPHY_INCLUDE_DIR
  MURPHY_LIBRARY
)
