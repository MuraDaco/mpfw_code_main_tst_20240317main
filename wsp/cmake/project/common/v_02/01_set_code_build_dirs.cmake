#   *******************************************************************************
#   
#   mpfw / fw2 - Multi Platform FirmWare FrameWork 
#       library that contains the "main" entry point and, 
#       eventualy, application code that is platform dependent
#   Copyright (C) (2023) Marco Dau
#   
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published
#   by the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#   
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.
#   
#   You can contact me by the following email address
#   marco <d o t> ing <d o t> dau <a t> gmail <d o t> com
#   
#   *******************************************************************************
##################################################################################
## _______________________________________________________________________________
## RELATIVE PATH Definitions

##################################################################################
## Relative path from CMakeLists.txt project folder                             ##
##################################################################################
trace_execution()

##################################################################################
## Relative path from build folder where the executable file (.out) is created  ##
##################################################################################
# linker needs the following definitions to set library path

# root folder: relative path of "code" directory (project root folder) respect project build folder
# it is necessary when a library obj file has to be linked
# it is necessary to define the library obj path respect the build directory (the folder where the executable file will be created) to give to linker
# it is directly used in path_pub_config.cmake file of all sub-project that have a library obj file (library already compiled and without sources)
set(CODE_DIR__FROM_BUILD        ../../../../../../../../../../../../../.. )

# wsp folder: relative path of "wsp" app directory from project build folder
# it is necessary to define the "cube" executable path respect the build directory (the folder where the executable file will be created) to give to post command of copy 
# it is used in the copy post command to copy the executable file to build directory of "stm cube" project, here the mentioned executable file can be upload to device 
set(WSP_DIR__FROM_BUILD     ../../../../../../../../../../.. )



end_include()
