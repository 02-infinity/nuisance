# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NUISANCE.
#
#    NUISANCE is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NUISANCE is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

if(NOT DEFINED ENV{NUWRO})

  cmessage(FATAL_ERROR "Environment variable NUWRO is not defined. "
    "This must be set to point to a prebuilt NuWro instance.")

endif()

set(NUWRO $ENV{NUWRO})

if(DEFINED USE_EXP AND USE_EXP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNW_READHISTFROMINP")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NUWRO_ENABLED__ -D__NUWRO_REWEIGHT_ENABLED__ ")

set(RWENGINE_INCLUDE_DIRECTORIES ${RWENGINE_INCLUDE_DIRECTORIES} ${NUWRO}/src ${NUWRO}/src/reweight ${NUWRO}/build/src)

set(RWENGINE_LINKER_FLAGS "${RWENGINE_LINKER_FLAGS} -L${NUWRO}/build/${CMAKE_SYSTEM_NAME}/lib -lreweight -levent")

set(NEED_PYTHIA6 TRUE)
set(NEED_ROOTPYTHIA6 TRUE)
