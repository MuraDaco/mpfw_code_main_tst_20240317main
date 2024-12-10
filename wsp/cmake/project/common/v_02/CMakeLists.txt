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
trace_execution_cmakelists()

enable_language( ASM)


# step 1 - 1st level path definition
set(CODE_DIR        ../../../../../../.. )
include(01_set_code_build_dirs.cmake )
include(${CODE_DIR}/${CMAKE_MODULE_VER_RPATH}/set_apps_libs_dirs.cmake )

# step 2 - 2nd level path definition of depended modules
include(02_include_dependencies.cmake )

# step 3 - set cpp sources files
include(../../${WP_PLATFORM_STR}/set_cpp_files.cmake )

# step 4 - post build parameter (copy executable file into development environment build directory )
include(../../${WP_PLATFORM_STR}/set_dev_environment_build_dir.cmake )



## ******************************************************************
## __________________________________________________________________
## TST/APP CPP FILES Definitions

set(EXECUTABLE ${PROJECT_NAME}.out )

add_executable(${EXECUTABLE} 
    ${CODE_APP_SOURCE_MAIN}
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${CODE_DIR_APP_SOURCE_MAIN_INCLUDE}
)

target_link_libraries(${EXECUTABLE} PRIVATE 
    #${APP_NAME}_lib
    #fw2_wp_rs_lib
    #fw2_wp_kr_lib
    #fw2_lib
    ${LIB_SDK_PLATFORM}
    #fw2_wp_rs_lib
)

include(${LIBS_SDK_PLATFORM_CMAKE_PUBLIC_DIR}/toolchain_compile_option.cmake )
include(${LIBS_SDK_PLATFORM_CMAKE_PUBLIC_DIR}/toolchain_link_option.cmake )

message(STATUS "---- end   section of <module_main_${MAIN_NAME}> module ")
