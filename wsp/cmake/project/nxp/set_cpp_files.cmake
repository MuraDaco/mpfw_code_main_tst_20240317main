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
## ******************************************************************
## __________________________________________________________________
## TST/APP CPP FILES Definitions
trace_execution()

set(CODE_NXP_FILES_TST_C_MN_AP
    ${MAIN_XXX_SRC_DIR}/ds3231.c
    ${MAIN_XXX_SRC_DIR}/ds3231_low_level.c
    ${MAIN_XXX_SRC_DIR}/freertos.c
    ${MAIN_XXX_SRC_DIR}/semihost_hardfault.c
)

set(CODE_NXP_FILES_TST_CPP_MN_AP
    ${MAIN_XXX_SRC_DIR}/LPC54608_tst_freertos.cpp
    ${MAIN_XXX_SRC_DIR}/cpp_config.cpp
)

set(CODE_APP_TB_COMMON
    ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krThread/v_${TB_KR_THREAD_VER}/krThreadTbl.cpp
    ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krInit/v_${TB_KR_INIT_VER}/krInitStaticTbl.cpp
    ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krTimer/v_${TB_KR_TIMER_VER}/krTimerTbl.cpp
    ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/sv/svStateMachine/v_${TB_SV_STATE_MACHINE_VER}/svStateMachineTbl.cpp
)

set(CODE_APP_SOURCE_MAIN
    ${CODE_NXP_FILES_TST_C_MN_AP}
    ${CODE_NXP_FILES_TST_CPP_MN_AP}
    ${CODE_APP_TB_COMMON}
)

end_include()

