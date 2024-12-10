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
trace_execution()

##################################################################################
## _______________________________________________________________________________
## RELATIVE PATH Definitions

##################################################################################
## Relative path from build folder where the executable file (.out) is created  ##
##################################################################################

# after build process the executable file (.out) is copied into "stm cube" build folder to be available to upload to micro via st-link
# this variable is used in "tool_cmd.cmake" file
set(PLATFORM_ENVIRONMENT_BUILD_DIR    ${WSP_DIR__FROM_BUILD}/cube/project/${WP_PLATFORM_STR}/${MAIN_NAME}/${CMAKE_BUILD_TYPE}      )
#  for example:
#  - main/mpfw_code_main_stm_20230424/wsp/cube/project/stm32f769_disco/stm_20230424/Debug

end_include()
