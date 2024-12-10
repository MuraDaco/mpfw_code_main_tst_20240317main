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



# set(CODE_APP_TB_COMMON
#     ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krThread/v_${TB_KR_THREAD_VER}/krThreadTbl.cpp
#     ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krInit/v_${TB_KR_INIT_VER}/krInitStaticTbl.cpp
#     ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/kr/krTimer/v_${TB_KR_TIMER_VER}/krTimerTbl.cpp
#     ${MAIN_XXX_SRC_COMMON_DIR}/tb/tb/sv/svStateMachine/v_${TB_SV_STATE_MACHINE_VER}/svStateMachineTbl.cpp
# )


set(CODE_SW4STM32_DIR
    ${MAIN_XXX_SRC_DIR}/SW4STM32/startup_stm32f769xx.s
    ${MAIN_XXX_SRC_DIR}/SW4STM32/syscalls.c
)

set(CODE_SRC_DIR
    ${MAIN_XXX_SRC_DIR}/Src/audio_loopback.c
    ${MAIN_XXX_SRC_DIR}/Src/audio_play.c
    #${MAIN_XXX_SRC_DIR}/Src/camera.c
    ${MAIN_XXX_SRC_DIR}/Src/lcd.c
    ${MAIN_XXX_SRC_DIR}/Src/log.c
    ${MAIN_XXX_SRC_DIR}/Src/main.c
    ${MAIN_XXX_SRC_DIR}/Src/qspi.c
    ${MAIN_XXX_SRC_DIR}/Src/sd.c
    ${MAIN_XXX_SRC_DIR}/Src/sd_diskio.c
    ${MAIN_XXX_SRC_DIR}/Src/sd_FatFS.c
    ${MAIN_XXX_SRC_DIR}/Src/sdram.c
    ${MAIN_XXX_SRC_DIR}/Src/stm32f7xx_it.c
    ${MAIN_XXX_SRC_DIR}/Src/system_stm32f7xx.c
    ${MAIN_XXX_SRC_DIR}/Src/touchscreen.c
    ${MAIN_XXX_SRC_DIR}/Src/usbh.c
    ${MAIN_XXX_SRC_DIR}/Src/usbh_conf.c
    ${MAIN_XXX_SRC_DIR}/Src/usbh_diskio_dma.c
    ${MAIN_XXX_SRC_DIR}/Src/file_operations.c
    ${MAIN_XXX_SRC_DIR}/Src/file_operations_SD_FatFS.c
    ${MAIN_XXX_SRC_DIR}/Src/explorer.c
    ${MAIN_XXX_SRC_DIR}/Src/explorer_SD_FatFS.c
    ${MAIN_XXX_SRC_DIR}/Src/menu.c

    ${MAIN_XXX_SRC_DIR}/Src/uyGlo.c
    ${MAIN_XXX_SRC_DIR}/Src/uyRTC.c
    ${MAIN_XXX_SRC_DIR}/Src/radar_sensor.c
    ${MAIN_XXX_SRC_DIR}/Src/radar_sensor_log.c
    ${MAIN_XXX_SRC_DIR}/Src/hwI2C.c
    ${MAIN_XXX_SRC_DIR}/Src/hwDS3231.c
)




set(CODE_APP_SOURCE_MAIN
    ${CODE_SW4STM32_DIR}
    ${CODE_SRC_DIR}
)

end_include()
