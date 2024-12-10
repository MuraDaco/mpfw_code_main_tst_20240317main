//  *******************************************************************************
//  
//  mpfw / fw2 - Multi Platform FirmWare FrameWork 
//      library that contains the "main" entry point and, 
//      eventualy, application code that is platform dependent
//  Copyright (C) (2023) Marco Dau
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  You can contact me by the following email address
//  marco <d o t> ing <d o t> dau <a t> gmail <d o t> com
//  
//  *******************************************************************************
/*
 * main.c
 *
 *  Created on: Aug, 10 2023
 *      Author: Marco Dau
 */

#include <iomanip>

// system include
#include <thread>
#include <pthread.h>
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

// module include
#include "main.h"

// lib include
#include <krInitStaticManager.h>
using namespace fw2::core::core;

#include <krThreadManager.h>
using namespace fw2::wrapper::core;

#include <sdk_usart.h>
#include <rsSerial.h>
#include <uyProtocol2.h>
using namespace fw2::wrapper::resources;

// tst include
#include <tt_MainManager.h>
#include <mn/tt_mn_svSyncMain.h>

// tst/tb include
#include <app/krThreadTbl.h>
#include <app/krTimerTbl.h>
#include <app/svStateMachineTbl.h>
using namespace mpfw::apps;

void thread_service_rsSerial() {

	while(1){if(!sdk_usart::loop()) break;}

}


void set_device_addr    (char p_param)  {
    char l_param_1 = p_param;
    // if(l_param_1.compare("Master")) uyProtocol2::Set_Master();
    if(
        ('1' <= l_param_1) && 
        ('9' >= l_param_1)
    ) {
        uyProtocol2::AddsSet((uint8_t) l_param_1);
        std::cout << "Set device address: " << l_param_1 << " - end" << std::endl;
    } 
}

void set_master_slave    (string p_param)  {
    if(0 == p_param.compare("Master")) {
        std::cout << "Master is seleced - Input: " << p_param << " - end" << std::endl;
        uyProtocol2::Set_Master();
    } else {
        if(0 == p_param.compare("Slave")) {
            std::cout << "Slave is seleced - Input: " << p_param << " - end" << std::endl;
            uyProtocol2::Set_Slave();
        } else {
            std::cout << "Parameter is not ricognized - Input: " << p_param << " - end" << std::endl;
        }
    }
}


int main(int argc, char *argv[])
{

    std::cout << "argc == " << argc << '\n';
 
    for(int ndx{}; ndx != argc; ++ndx) {
        std::cout << "argv[" << ndx << "] == " << quoted(argv[ndx]) << '\n';
    }
    std::cout << "argv[" << argc << "] == "
              << static_cast<void*>(argv[argc]) << '\n';
    string l_param("xxx");

    if(argc == 2)   {
        set_device_addr((char) argv[1][0]);
    }

    if(argc == 3)   {
        set_device_addr((char) argv[1][0]);
        set_master_slave(argv[2]);

    }

    if(argc == 4)   {
        set_device_addr((char) argv[1][0]);
        set_master_slave(argv[2]);
        l_param = argv[3];
    }

    std::thread thread_service;
	sdk_usart::start(l_param);
    thread_service = std::thread(thread_service_rsSerial);

    rsSerial::Init();

    // ***************************************************************************
    krInitStaticManager::LoopOnUnits();     
    rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    rsSerial::Tx("START  - Main"); rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    rsSerial::Tx("START  - Setup Timer Service"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    rsSerial::Tx("START  - Setup State Machine Service"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);

    // ***************************************************************************
    rsSerial::Tx("START  - Threads definition and then run them"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    krThreadManager::InitThenRun();
    
    // do {
    //     sleep_for(10ms);
    // } while (tt_mn_svSyncMain::CheckRaceConditionCounter(10));
    

    // ***************************************************************************
    krThreadManager::Exit();
    rsSerial::Tx("EXIT  - from Main Loop"); rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);
    rsSerial::Tx(rsSerial::endl);

    //// ***************************************************************************
    //rsSerial::Tx("SHOW  - Tests results"); rsSerial::Tx(rsSerial::endl);
    //rsSerial::Tx(rsSerial::endl);
    //tt_MainManager::LoopOnUnits();
    //rsSerial::Tx(rsSerial::endl);

	sdk_usart::end_0();
    thread_service.join();
	sdk_usart::end();

}

