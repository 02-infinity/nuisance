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

##################################  NEUT  ######################################
if(USE_NEUT)
  include(${CMAKE_SOURCE_DIR}/cmake/NEUTSetup.cmake)
  cmessage(STATUS "Using NEUT Reweight engine.")
endif()
#################################  NuWro  ######################################
if(USE_NuWro)
  include(${CMAKE_SOURCE_DIR}/cmake/NuWroSetup.cmake)
  cmessage(STATUS "Using NuWro Reweight engine.")
endif()
##################################  GENIE  #####################################
if(USE_GENIE)
  include(${CMAKE_SOURCE_DIR}/cmake/GENIESetup.cmake)
  cmessage(STATUS "Using GENIE Reweight engine.")
endif()

if(DEFINED BUILD_GEVGEN AND BUILD_GEVGEN)
	   cmessage(STATUS "Building custom gevgen.")
else()
set(BUILD_GEVGEN 0)
endif()

##################################  NIWG  ######################################
if(USE_NIWG)
  include(${CMAKE_SOURCE_DIR}/cmake/NIWGSetup.cmake)
  cmessage(STATUS "Using NIWG Reweight engine.")
endif()
##################################  T2K   ######################################
if(USE_T2K)
  include(${CMAKE_SOURCE_DIR}/cmake/T2KSetup.cmake)
  cmessage(STATUS "Using T2K Reweight engine.")
endif()

################################################################################

cmessage(STATUS "Reweight engine include directories: ${RWENGINE_INCLUDE_DIRECTORIES}")

if(NEED_ROOTEVEGEN)
  cmessage(STATUS "Require ROOT eve generation libraries")
  LIST(REVERSE ROOT_LIBS)
  LIST(APPEND ROOT_LIBS
    Gui
    Ged
    Geom
    TreePlayer
    EG
    Eve)
  LIST(REVERSE ROOT_LIBS)
endif()

if(NEED_ROOTPYTHIA6)
  cmessage(STATUS "Require ROOT Pythia6 libraries")
  LIST(APPEND ROOT_LIBS
    EGPythia6
    Pythia6)
endif()
