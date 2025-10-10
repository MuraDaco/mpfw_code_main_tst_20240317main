
---

Sunday, 9th June 2023


- come eseguire i collegamenti considerando un solo cavo per la gestione del [tx/rx rs-485 enable]
    - condizioni: devono essere l'uno l'opposto dell'altro, quando uno è alto l'altro deve essere basso

       |
    o--+
    s  |
    o--+
       |

- cosa sono gli "interrupt pending"?


- funzioni per la gestione della trasmissione rs-485
    - impostare il clock della periferica
    - abilitare il clock della periferica
    - impostare la priorità dell'interrupt
    - abilitare l'interrupt
    - init
        - impostazione dei parametri di comunicazione
            - parity
            - stop bit
            - bit length
            - baud rate
        - abilitare il modulo
            - CLOCK_AttachClk (fsl_clock.h/c)
                - rsDbgConsole::Init
            - CLOCK_EnableClock (fsl_clock.h)
                - FLEXCOMM_Init (fsl_flexcomm.h/c)
                    - USART_Init
                        - USART_RTOS_Init
            - RESET_PeripheralReset (fsl_reset.h/c)
                - FLEXCOMM_Init (fsl_flexcomm.h/c)
                    - USART_Init
                        - USART_RTOS_Init
            - NVIC_SetPriority (core_cm4.h)
                - rsDbgConsole::InitFreeRTOS
            - FLEXCOMM_SetIRQHandler (fsl_flexcomm.h/c)
                - USART_TransferCreateHandle
                    - USART_RTOS_Init
            - FLEXCOMM0_DriverIRQHandler (fsl_flexcomm.h/c)
                - FLEXCOMM0_IRQHandler (startup_lpc54608.cpp)
            - EnableIRQ (fsl_common.h) 
                - USART_TransferCreateHandle
                    - USART_RTOS_Init

            - NVIC->IP
                - __NVIC_SetPriority
                    - NVIC_SetPriority
            - NVIC->ISER
                - __NVIC_EnableIRQ
                    - NVIC_EnableIRQ
                        - EnableIRQ
            - SYSCON->PRESETCTRL / SYSCON->PRESETCTRLSET / SYSCON->PRESETCTRLCLR
                - RESET_ClearPeripheralReset
                - RESET_SetPeripheralReset
                    - RESET_PeripheralReset
            - SYSCON->AHBCLKCTRLSET
                - CLOCK_EnableClock
            - SYSCON->ASYNCAPBCTRL
                - CLOCK_AttachClk
            - SYSCON->MAINCLKSELA
                - CLOCK_AttachClk




    - send_buffer
    - interrupt_tx handler
        - body of the packet
        - last byte send
    - timer to manage rs485-en handler



---

Saturday, 8th June 2023

- impostare un tasto per l'invio di un pacchetto

- pensare in termini di 
    - "processo" o "flusso di processo"
    - e di "thread"
    - un processo durante il suo flusso può saltare da un thread ad un altro oppure avviare contemporaneamente più thread
    - ci saranno poi dei 
        - processi specifici tipicamnete associati alle risorse
        - processi associati alle applicazioni


---

Friday, 7th June 2023


- analisi della trasmissione
    - cosa signifia nella trasmissione: procedura "blocking" e "no-blocking"?
        - quando si inseriscono i dati nel FIFO bisogna stare attenti che il FIFO non vada in overflow
            - il fifo può contenere al massimo 16 byte, naturalmente si può usare un fifo con un solo byte
        - quindi quando si trasferisce un buffer si avrà un interrupt all'incirca ogni 1ms (se il baud rate è di 9600, perchè ci vuole 1ms per inviare un byte
    - processo di gestione della trasmissione 
        - init
            - CFG
                ->ENABLE
                ->DATALEN
                ->PARITYSEL
                ->STOPLEN
                ->OETA
                ->OESEL
            - INTENSET
            - INTENCLR
            - INTSTATUS
            - BRG (Baud rate)
            - -------------------------
            - configurare i parametri generali validi per tx & rx
                - datalen, paruty, stopbit
            - configurare la TX
                - TXLVLENA & TXLVL
            - enable uart peripheral
        - send
            - FIFOCFG
                ->ENABLETX
                ->EMPTYTX
            - FIFOSTAT
                ->TXEMPTY
                    - Transmit FIFO empty. When 1, the transmit FIFO is empty. The peripheral may still be processing the last piece of data.
                ->TXNOTFULL
                ->TXLVL
            - FIFOTRIG
                ->TXLVLENA
                ->TXLVL
            - FIFOINTENSET / FIFOINTENCLR
                ->TXLVL
            - FIFOINTSTAT
                ->TXLVL
            - FIFOWR / FIFORD
                ->TXDATA / RXDATA
                    - Transmit data to / Receive data from the FIFO.
            - -------------------------
            - write data in FIFO
                - FIFOWR->TXDATA
            - abilitare l'interrupt
                - ENABLETX, EMPTYTX
            - gestione nel loop-main
            - gestione nell'interrupt
            - gestione dell'ultimo carattere
        

- stat->TXIDLE
    - Transmitter Idle. When 0, indicates that the transmitter is currently in the process of sending data. When 1, indicate that the transmitter is not currently in the process of sending data.
- stat->TXDISSTAT
    - Transmitter Disabled Status flag. When 1, this bit indicates that the USART transmitter is fully idle after being disabled via the TXDIS bit in the CFG register (TXDIS = 1).            
- intenset->TXIDLEEN
    - When 1, enables an interrupt when the transmitter becomes idle (TXIDLE = 1).
- intenset->TXDISEN
    - When 1, enables an interrupt when the transmitter is fully disabled as indicated by the TXDISINT flag in STAT. See description of the TXDISINT bit for details.
- intstat->TXDISINT
    - Transmitter Disabled Interrupt flag.
- intstat->TXIDLE
    - Transmitter Idle status


- analisi della ricezione


- configurazione della board ed in particolare dei pin
    - sono funzioni di inizializzazione
        - il modulo board deve conoscere/vedere tutti i moduli che gestiscono i pin della board
    - la board è specifica per la piattaforma
    - se è la sequenza delle funzioni di init di ciascun modulo forse si può usare direttamente il modulo krInit e la sua tabella
    - forse il modulo "board" ha senso solo nel caso ci siamo elementi che sono comuni a più moduli
    - oppure potrebbe essere il punto più opportuno dove definire gli interrupt, quindi quei file che adesso si trovano nel modulo main_app
        - 

- la struttura che devono avere i dati per la gestione del buffer sono
    - flag -> newdata=false / buffer_full=false
        - questa flag fa da flip-flop tra l'interrup e la funzione chiamata nel main-loop
        - if (newdata == false)


    - pointer buffer

// ***** start - come gestire il modulo relativo al protocollo tipo mod-bus

- elementi in comune
    - il preambolo
        - id del dispositivo sorgenete del pacchetto
        - id del dipositivo ricevente il pacchetto
        - lunghezza del frame comprensivo del checksum che saranno 1, 2 o 4 byte
    - corpo del messaggio
    - checksum

- problemi da risolvere
    - quando il pacchetto viene interrotto prima di essere completato la macchina a stati si viene a trovare in uno stato di stallo perche attende la fine del pacchetto che però non arriverà
        - per evitare questa situazione una possibilità è quella di mettere un timer di timeout scaduto il quale il buffer viene resettato
        - il timer deve essere fatto partire dal ricevimento dell'ultimo byte, 
            - tra due byte successivi si può mettere un intervallo massimo di 1ms, quindi il timeout sarà di 1ms e comunuqe si potrebbe parametrizzarlo assegandogli un parametro
    - quando si deve comunicare con un altro dispositivo è bene stabilire delle tempistiche, eventualmente impostarle come parametri,
        - tempo di attesa prima di **RICEVERE** una risposta prima di considerare il messaggio senza risposta
            - questo tempo è anche leagto all'abilitazione/disabilitazione del modulo RS-485
        - timeout della risposta non è necessaria un assoluta precisione
        - tempo di abilitazione del canale di ricezione su RS485 necessità di un alta precisione per ottimizzare i tempi
            - per gestire rs485_rx_enable ci sono due possibilità
                - manualmente gestendo un interrupt timer e l'interrupt di trasmissione relativo all'evento dell'invio dell'ultimo byte
                    - vediamo gli interrupt di trasmissione
                - automaticamante impostando OETA/OESEL del modulo USART
        - tempo di attesa prima di **INVIARE** una risposta
            - si deve dare il tempo al dispositivo che ha trasmesso il messaggio di impostarsi per la ricezione (**Rs485 enable**)


- come gestire le varie fasi
    - l'analisi del corpo del pacchetto deve essere fatta nel main-loop
    - l'analisi del preambolo del pacchetto deve essere fatta in interrupt
        - è necessario per ottenere la lunghezza del pacchetto

- dai pacchetti ricevuti sul canale seriale/rs485 si ottengono
    - informazioni
    - comandi
        - eventi con parametro

- modalità
    - slave
        - ci sarà un modulo che controlla gli eventi/messaggi provenienti dalla seriale e poi li gestisce
    - master
        - ci sarà un modulo che invia comandi sulla seriale per ottenere informazioni per poi gestirle



// ***** end   - come gestire il modulo relativo al protocollo tipo mod-bus


---

Thursday, 6th June 2023

/***** START - usart data buffer management ****************/



- interrupt
    - si carica il buffer
        - ogni volta che si attiva l'interrupt
            - check rxbuffer status
                - if rxbuffer is not full
                    - add byte to buffer
                        - check status buffer & in case set it to "FULL"

- CFG
    ->ENABLE
    ->OETA
    ->OESEL

- CTL
    ->

- STAT
    ->

- INTENSET
    ->STARTEN

INTSTAT
    ->START

- FIFOSTAT
    ->RXNOTEMPTY = 1 ==> si può leggere
    ->RXLVL

- FIFOTRIG
    ->RXLVLENA = 1 ==> abilita l'interrupt
    ->RXLVL

- FIFORD
    ->RXDATA
- FIFORDNOPOP

- FIFOINTENSET
- FIFOINTENCLR

- main loop
    - si controlla lo stato del buffer
        - every main loop
            - CheckRxBuffer()
                - if buffer is full
                    - manage the buffer
                    - reset buffer
                        - set status buffer to "not full"

- FIFOCFG
    - ENABLERX
    - EMPTYRX


/***** END - usart data buffer management ****************/

/***** START - INTERRUPT UART **********/

// impostazione della priorità dell'interrupt


// impostazione dell'handler
    FLEXCOMM_SetIRQHandler(base, handler.flexcomm_handler, handle);

// abilitazione dell'inettrupt
    /* Enable interrupt in NVIC. */
    (void)EnableIRQ(s_usartIRQ[instance]);

//-//-//-

status_t USART_TransferCreateHandle(USART_Type *base,
                                    usart_handle_t *handle,
                                    usart_transfer_callback_t callback,
                                    void *userData)
{
    /* Check 'base' */
    assert(!((NULL == base) || (NULL == handle)));

    uint32_t instance = 0;
    usart_to_flexcomm_t handler;
    handler.usart_master_handler = USART_TransferHandleIRQ;

    if ((NULL == base) || (NULL == handle))
    {
        return kStatus_InvalidArgument;
    }

    instance = USART_GetInstance(base);

    (void)memset(handle, 0, sizeof(*handle));
    /* Set the TX/RX state. */
    handle->rxState = (uint8_t)kUSART_RxIdle;
    handle->txState = (uint8_t)kUSART_TxIdle;
    /* Set the callback and user data. */
    handle->callback    = callback;
    handle->userData    = userData;
    handle->rxWatermark = (uint8_t)USART_FIFOTRIG_RXLVL_GET(base);
    handle->txWatermark = (uint8_t)USART_FIFOTRIG_TXLVL_GET(base);

    FLEXCOMM_SetIRQHandler(base, handler.flexcomm_handler, handle);

    /* Enable interrupt in NVIC. */
    (void)EnableIRQ(s_usartIRQ[instance]);

    return kStatus_Success;
}

//-//-

    static inline status_t EnableIRQ(IRQn_Type interrupt)
    {
        if (NotAvail_IRQn == interrupt)
        {
            return kStatus_Fail;
        }

#if defined(FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS) && (FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS > 0)
        if (interrupt >= FSL_FEATURE_NUMBER_OF_LEVEL1_INT_VECTORS)
        {
            return kStatus_Fail;
        }
#endif

#if defined(__GIC_PRIO_BITS)
        GIC_EnableIRQ(interrupt);
#else
        NVIC_EnableIRQ(interrupt);
#endif
        return kStatus_Success;
    }


//-//-

  #define NVIC_EnableIRQ              __NVIC_EnableIRQ
  #define NVIC_GetEnableIRQ           __NVIC_GetEnableIRQ

//-//-


/**
  \brief   Enable Interrupt
  \details Enables a device specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}

/***** END - INTERRUPT UART **********/

/***************/

struct rtos_usart_config    rsDbgConsole::usart_config= {
        .baudrate    = 115200,
        .parity      = kUSART_ParityDisabled,
        .stopbits    = kUSART_OneStopBit,
        .srcclk      = BOARD_DEBUG_UART_CLK_FREQ;
        .base        = DEMO_USART;
        .buffer      = rsDbgConsole::background_buffer,
        .buffer_size = sizeof(rsDbgConsole::background_buffer),
    };

void rsDbgConsole::InitFreeRTOS             (void)      {

    NVIC_SetPriority(DEMO_USART_IRQn, USART_NVIC_PRIO);

    if (0 > USART_RTOS_Init(&handle, &t_handle, &usart_config))
    {
        vTaskSuspend(NULL);
    }
    
}

//-//-//-

int USART_RTOS_Init(usart_rtos_handle_t *handle, usart_handle_t *t_handle, const struct rtos_usart_config *cfg)
{
    usart_config_t defcfg;
    ...
    ...
    USART_GetDefaultConfig(&defcfg);

    defcfg.baudRate_Bps = cfg->baudrate;
    defcfg.parityMode   = cfg->parity;
    defcfg.enableTx     = true;
    defcfg.enableRx     = true;

    USART_Init(handle->base, &defcfg, cfg->srcclk);


//-//-//-

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "LPCXPRESSO54608"

#define BOARD_EXTCLKINRATE (0)

/*! @brief The UART to use for debug messages. */
/* TODO: rename UART to USART */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) USART0
#define BOARD_DEBUG_UART_INSTANCE 0U
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetFlexCommClkFreq(0)


//-//-//-

uint32_t CLOCK_GetFlexCommClkFreq(uint32_t id)
{
    uint32_t freq = 0U;

    switch (SYSCON->FCLKSEL[id])
    {
        case 0U:
            freq = CLOCK_GetFro12MFreq();
            break;
        case 1U:
            freq = CLOCK_GetFroHfFreq() / ((SYSCON->FROHFCLKDIV & SYSCON_FROHFCLKDIV_DIV_MASK) + 1U);
            break;
        case 2U:
            freq = CLOCK_GetPllOutFreq();
            break;
        case 3U:
            freq = CLOCK_GetI2SMClkFreq();
            break;
        case 4U:
            freq = CLOCK_GetFrgClkFreq();
            break;

        default:
            freq = 0U;
            break;
    }

    return freq;
}

/* Get FRO 12M Clk */
/*! brief	Return Frequency of FRO 12MHz
 *  return	Frequency of FRO 12MHz
 */
uint32_t CLOCK_GetFro12MFreq(void)
{
    return (0U != (SYSCON->PDRUNCFG[0] & SYSCON_PDRUNCFG_PDEN_FRO_MASK)) ? 0U : 12000000U;
}

//-//-//-

status_t USART_Init(USART_Type *base, const usart_config_t *config, uint32_t srcClock_Hz)
{
    int result;

    /* check arguments */
    assert(!((NULL == base) || (NULL == config) || (0U == srcClock_Hz)));
    if ((NULL == base) || (NULL == config) || (0U == srcClock_Hz))
    {
        return kStatus_InvalidArgument;
    }

    /* initialize flexcomm to USART mode */
    result = FLEXCOMM_Init(base, FLEXCOMM_PERIPH_USART);

//-//-//-

status_t USART_TransferCreateHandle(USART_Type *base,
                                    usart_handle_t *handle,
                                    usart_transfer_callback_t callback,
                                    void *userData)
{
    /* Check 'base' */
    assert(!((NULL == base) || (NULL == handle)));

    uint32_t instance = 0;
    usart_to_flexcomm_t handler;
    handler.usart_master_handler = USART_TransferHandleIRQ;

    if ((NULL == base) || (NULL == handle))
    {
        return kStatus_InvalidArgument;
    }

    instance = USART_GetInstance(base);

    (void)memset(handle, 0, sizeof(*handle));
    /* Set the TX/RX state. */
    handle->rxState = (uint8_t)kUSART_RxIdle;
    handle->txState = (uint8_t)kUSART_TxIdle;
    /* Set the callback and user data. */
    handle->callback    = callback;
    handle->userData    = userData;
    handle->rxWatermark = (uint8_t)USART_FIFOTRIG_RXLVL_GET(base);
    handle->txWatermark = (uint8_t)USART_FIFOTRIG_TXLVL_GET(base);

    FLEXCOMM_SetIRQHandler(base, handler.flexcomm_handler, handle);

    /* Enable interrupt in NVIC. */
    (void)EnableIRQ(s_usartIRQ[instance]);

    return kStatus_Success;
}


//-//-//-

/*! brief Initializes FLEXCOMM and selects peripheral mode according to the second parameter. */
status_t FLEXCOMM_Init(void *base, FLEXCOMM_PERIPH_T periph)
{
    uint32_t idx = FLEXCOMM_GetInstance(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the peripheral clock */
    CLOCK_EnableClock(s_flexcommClocks[idx]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if !(defined(FSL_FEATURE_FLEXCOMM_HAS_NO_RESET) && FSL_FEATURE_FLEXCOMM_HAS_NO_RESET)
    /* Reset the FLEXCOMM module */
    RESET_PeripheralReset(s_flexcommResets[idx]);
#endif

    /* Set the FLEXCOMM to given peripheral */
    return FLEXCOMM_SetPeriph((FLEXCOMM_Type *)base, periph, 0);
}

//-//-//-

/* Get the index corresponding to the FLEXCOMM */
/*! brief Returns instance number for FLEXCOMM module with given base address. */
uint32_t FLEXCOMM_GetInstance(void *base)
{
    uint32_t i;
    pvoid_to_u32_t BaseAddr;
    BaseAddr.pvoid = base;

    for (i = 0U; i < (uint32_t)FSL_FEATURE_SOC_FLEXCOMM_COUNT; i++)
    {
        if (BaseAddr.u32 == s_flexcommBaseAddrs[i])
        {
            break;
        }
    }

    assert(i < FSL_FEATURE_SOC_FLEXCOMM_COUNT);
    return i;
}

//-//-//-
- compare 
    - c
    - CLOCK_AttachClk

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Clock Selection for IP */
/**
 * brief	Configure the clock selection muxes.
 * param	connection	: Clock to be configured.
 * return	Nothing
 */
void CLOCK_AttachClk(clock_attach_id_t connection)
{
    uint8_t mux;
    uint8_t sel;
    uint16_t item;
    uint32_t tmp32 = (uint32_t)connection;
    uint32_t i;
    volatile uint32_t *pClkSel;

    pClkSel = &(SYSCON->MAINCLKSELA);

    if (kNONE_to_NONE != connection)
    {
        for (i = 0U; i < 2U; i++)
        {
            if (tmp32 == 0U)
            {
                break;
            }
            item = (uint16_t)GET_ID_ITEM(tmp32);
            if (0U != item)
            {
                mux = GET_ID_ITEM_MUX(item);
                sel = GET_ID_ITEM_SEL(item);
                if (mux == CM_ASYNCAPB)
                {
                    SYSCON->ASYNCAPBCTRL          = SYSCON_ASYNCAPBCTRL_ENABLE(1);
                    ASYNC_SYSCON->ASYNCAPBCLKSELA = sel;
                }
                else
                {
                    ((volatile uint32_t *)pClkSel)[mux] = sel;
                }
            }
            tmp32 = GET_ID_NEXT_ITEM(tmp32); /*!<  pick up next descriptor */
        }
    }
}

//-//-//-

static inline void CLOCK_EnableClock(clock_ip_name_t clk)
{
    uint32_t index = CLK_GATE_ABSTRACT_REG_OFFSET(clk);
    if (index < 3U)
    {
        SYSCON->AHBCLKCTRLSET[index] = (1UL << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
    else
    {
        SYSCON->ASYNCAPBCTRL             = SYSCON_ASYNCAPBCTRL_ENABLE(1);
        ASYNC_SYSCON->ASYNCAPBCLKCTRLSET = (1UL << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
    }
}

//-//-//-

/* ----------------------------------------------------------------------------
   -- ASYNC_SYSCON Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ASYNC_SYSCON_Peripheral_Access_Layer ASYNC_SYSCON Peripheral Access Layer
 * @{
 */

/** ASYNC_SYSCON - Register Layout Typedef */
typedef struct {
  __IO uint32_t ASYNCPRESETCTRL;                   /**< Async peripheral reset control, offset: 0x0 */
  __O  uint32_t ASYNCPRESETCTRLSET;                /**< Set bits in ASYNCPRESETCTRL, offset: 0x4 */
  __O  uint32_t ASYNCPRESETCTRLCLR;                /**< Clear bits in ASYNCPRESETCTRL, offset: 0x8 */
       uint8_t RESERVED_0[4];
  __IO uint32_t ASYNCAPBCLKCTRL;                   /**< Async peripheral clock control, offset: 0x10 */
  __O  uint32_t ASYNCAPBCLKCTRLSET;                /**< Set bits in ASYNCAPBCLKCTRL, offset: 0x14 */
  __O  uint32_t ASYNCAPBCLKCTRLCLR;                /**< Clear bits in ASYNCAPBCLKCTRL, offset: 0x18 */
       uint8_t RESERVED_1[4];
  __IO uint32_t ASYNCAPBCLKSELA;                   /**< Async APB clock source select A, offset: 0x20 */
} ASYNC_SYSCON_Type;




/////

/*! @brief FLEX USART configuration structure */
struct rtos_usart_config
{
    USART_Type *base;                /*!< USART base address */
    uint32_t srcclk;                 /*!< USART source clock in Hz*/
    uint32_t baudrate;               /*!< Desired communication speed */
    usart_parity_mode_t parity;      /*!< Parity setting */
    usart_stop_bit_count_t stopbits; /*!< Number of stop bits to use */
    uint8_t *buffer;                 /*!< Buffer for background reception */
    uint32_t buffer_size;            /*!< Size of buffer for background reception */
};

/** USART - Register Layout Typedef */
typedef struct {
  __IO uint32_t CFG;                               /**< USART Configuration register. Basic USART configuration settings that typically are not changed during operation., offset: 0x0 */
  __IO uint32_t CTL;                               /**< USART Control register. USART control settings that are more likely to change during operation., offset: 0x4 */
  __IO uint32_t STAT;                              /**< USART Status register. The complete status value can be read here. Writing ones clears some bits in the register. Some bits can be cleared by writing a 1 to them., offset: 0x8 */
  __IO uint32_t INTENSET;                          /**< Interrupt Enable read and Set register for USART (not FIFO) status. Contains individual interrupt enable bits for each potential USART interrupt. A complete value may be read from this register. Writing a 1 to any implemented bit position causes that bit to be set., offset: 0xC */
  __O  uint32_t INTENCLR;                          /**< Interrupt Enable Clear register. Allows clearing any combination of bits in the INTENSET register. Writing a 1 to any implemented bit position causes the corresponding bit to be cleared., offset: 0x10 */
       uint8_t RESERVED_0[12];
  __IO uint32_t BRG;                               /**< Baud Rate Generator register. 16-bit integer baud rate divisor value., offset: 0x20 */
  __I  uint32_t INTSTAT;                           /**< Interrupt status register. Reflects interrupts that are currently enabled., offset: 0x24 */
  __IO uint32_t OSR;                               /**< Oversample selection register for asynchronous communication., offset: 0x28 */
  __IO uint32_t ADDR;                              /**< Address register for automatic address matching., offset: 0x2C */
       uint8_t RESERVED_1[3536];
  __IO uint32_t FIFOCFG;                           /**< FIFO configuration and enable register., offset: 0xE00 */
  __IO uint32_t FIFOSTAT;                          /**< FIFO status register., offset: 0xE04 */
  __IO uint32_t FIFOTRIG;                          /**< FIFO trigger settings for interrupt and DMA request., offset: 0xE08 */
       uint8_t RESERVED_2[4];
  __IO uint32_t FIFOINTENSET;                      /**< FIFO interrupt enable set (enable) and read register., offset: 0xE10 */
  __IO uint32_t FIFOINTENCLR;                      /**< FIFO interrupt enable clear (disable) and read register., offset: 0xE14 */
  __I  uint32_t FIFOINTSTAT;                       /**< FIFO interrupt status register., offset: 0xE18 */
       uint8_t RESERVED_3[4];
  __IO uint32_t FIFOWR;                            /**< FIFO write data., offset: 0xE20 */
       uint8_t RESERVED_4[12];
  __I  uint32_t FIFORD;                            /**< FIFO read data., offset: 0xE30 */
       uint8_t RESERVED_5[12];
  __I  uint32_t FIFORDNOPOP;                       /**< FIFO data read with no FIFO pop., offset: 0xE40 */
       uint8_t RESERVED_6[440];
  __I  uint32_t ID;                                /**< Peripheral identification register., offset: 0xFFC */
} USART_Type;

/* USART - Peripheral instance base addresses */
/** Peripheral USART0 base address */
#define USART0_BASE                              (0x40086000u)
/** Peripheral USART0 base pointer */
#define USART0                                   ((USART_Type *)USART0_BASE)

/***************/


/*************Code for DbgConsole Init, Deinit, Printf, Scanf *******************************/

/*! @brief The UART to use for debug messages. */
/* TODO: rename UART to USART */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) USART0
#define BOARD_DEBUG_UART_INSTANCE 0U
#define BOARD_DEBUG_UART_CLK_FREQ CLOCK_GetFlexCommClkFreq(0)
#define BOARD_DEBUG_UART_CLK_ATTACH kFRO12M_to_FLEXCOMM0
#define BOARD_DEBUG_UART_RST kFC0_RST_SHIFT_RSTn
#define BOARD_DEBUG_UART_CLKSRC kCLOCK_Flexcomm0
#define BOARD_UART_IRQ_HANDLER FLEXCOMM0_IRQHandler
#define BOARD_UART_IRQ FLEXCOMM0_IRQn

    // result = DbgConsole_Init(   BOARD_DEBUG_UART_INSTANCE, 
    //                             BOARD_DEBUG_UART_BAUDRATE, 
    //                             BOARD_DEBUG_UART_TYPE,
    //                             BOARD_DEBUG_UART_CLK_FREQ
    //                         );

#if ((SDK_DEBUGCONSOLE == DEBUGCONSOLE_REDIRECT_TO_SDK) || defined(SDK_DEBUGCONSOLE_UART))
/* See fsl_debug_console.h for documentation of this function. */
status_t DbgConsole_Init(uint8_t instance, uint32_t baudRate, serial_port_type_t device, uint32_t clkSrcFreq)
{
    serial_manager_config_t serialConfig;
    status_t status = (status_t)kStatus_SerialManager_Error;

#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))
    serial_port_uart_config_t uartConfig = {
        .instance     = instance,
        .clockRate    = clkSrcFreq,
        .baudRate     = baudRate,
        .parityMode   = kSerialManager_UartParityDisabled,
        .stopBitCount = kSerialManager_UartOneStopBit,
        .enableRx     = 1,
        .enableTx     = 1,
    };
#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC) && (SERIAL_PORT_TYPE_USBCDC > 0U))
    serial_port_usb_cdc_config_t usbCdcConfig = {
        .controllerIndex = (serial_port_usb_cdc_controller_index_t)instance,
    };
#endif

#if (defined(SERIAL_PORT_TYPE_SWO) && (SERIAL_PORT_TYPE_SWO > 0U))
    serial_port_swo_config_t swoConfig = {
        .clockRate = clkSrcFreq,
        .baudRate  = baudRate,
        .port      = instance,
        .protocol  = kSerialManager_SwoProtocolNrz,
    };
#endif

/////

serial_manager_status_t SerialManager_Init(serial_handle_t serialHandle, serial_manager_config_t *config)
{
    serial_manager_handle_t *handle;
    serial_manager_status_t status = kStatus_SerialManager_Error;

#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))
        case kSerialPort_Uart:
            status = Serial_UartInit(((serial_handle_t)&handle->lowLevelhandleBuffer[0]), config->portConfig);

//.//.//.//
serial_manager_status_t Serial_UartInit(serial_handle_t serialHandle, void *serialConfig)
{
    serial_uart_state_t *serialUartHandle;
    serial_port_uart_config_t *uartConfig;
    hal_uart_config_t config;
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    hal_uart_transfer_t transfer;
#endif
#endif

    assert(serialConfig);
    assert(serialHandle);
    assert(SERIAL_PORT_UART_HANDLE_SIZE >= sizeof(serial_uart_state_t));

    uartConfig       = (serial_port_uart_config_t *)serialConfig;
    serialUartHandle = (serial_uart_state_t *)serialHandle;
//.//.//.//

hal_uart_status_t HAL_UartInit(hal_uart_handle_t handle, hal_uart_config_t *config)
{
    hal_uart_state_t *uartHandle;
    usart_config_t usartConfig;
    status_t status;
    assert(handle);
    assert(config);
    assert(config->instance < (sizeof(s_UsartAdapterBase) / sizeof(USART_Type *)));
    assert(s_UsartAdapterBase[config->instance]);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static USART_Type *const s_UsartAdapterBase[] = USART_BASE_PTRS;


//.//.//.//

hal_uart_status_t HAL_UartInit(hal_uart_handle_t handle, hal_uart_config_t *config)
{
    hal_uart_state_t *uartHandle;
    usart_config_t usartConfig;
    status_t status;
    assert(handle);
    assert(config);
    assert(config->instance < (sizeof(s_UsartAdapterBase) / sizeof(USART_Type *)));
    assert(s_UsartAdapterBase[config->instance]);
    ...
    ...
    usartConfig.enableRx    = config->enableRx;
    usartConfig.enableTx    = config->enableTx;
    usartConfig.txWatermark = kUSART_TxFifo0;
    usartConfig.rxWatermark = kUSART_RxFifo1;

    status = USART_Init(s_UsartAdapterBase[config->instance], &usartConfig, config->srcClock_Hz);

//.//.//.//


/////
/*! @brief serial port uart config struct*/
typedef struct _serial_port_uart_config
{
    uint32_t clockRate;                             /*!< clock rate  */
    uint32_t baudRate;                              /*!< baud rate  */
    serial_port_uart_parity_mode_t parityMode;      /*!< Parity mode, disabled (default), even, odd */
    serial_port_uart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
    uint8_t instance;                               /*!< Instance (0 - UART0, 1 - UART1, ...), detail information
                                                         please refer to the SOC corresponding RM. */
    uint8_t enableRx;                               /*!< Enable RX */
    uint8_t enableTx;                               /*!< Enable TX */
} serial_port_uart_config_t;


/*************Code for DbgConsole Init, Deinit, Printf, Scanf *******************************/

---

Wednesday, 5th June 2023

- gestione della seriale
    - migiorare il modulo fw2_wrapper_resources
    - l'applicazione

- gestione dei file di configurazione della board
    - ci sono parametri che non dipendono dall'applicazione ed altri si
        - come ad esempio la libreria del freeRTOS
    - sarebbe da pensare ad un modulo board o comunque una classe nel modulo wrapper_core
        - qui si potrebbero impostare anche gli interrupt
    - in alternativa, specie per la gestione degli interrupt, la locazione di questi file può essere il modulo main_app


---

Friday, 30th June 2023

- analisi della procedura di sostituzione del SysTick con un altro interrupt timer
    - le funzioni che vengono cambiate/"sovrascritte" sono
        - HAL_InitTick(uint32_t TickPriority)
            - 
        - HAL_SuspendTick(void)
        - HAL_ResumeTick(void)
    - le funzioni di cui sopra gestiscono i registri relativi all'interrupt timer
    - la funzione che implementa l'handler dell'interrupt è la seguente
        - stm_20230504
            - stm32f7xx_it.c
                - TIM6_DAC_IRQHandler
                    - HAL_TIM_IRQHandler

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}


__STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}


#define __HAL_TIM_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->DIER |= (__INTERRUPT__))

---

Thursday, 29th June 2023


***************

***************

***************

Finished building target: tst_20230522.elf
 
arm-none-eabi-size   tst_20230522.elf 
   text	   data	    bss	    dec	    hex	filename
  24780	     88	   2592	  27460	   6b44	tst_20230522.elf
Finished building: default.size.stdout
 
arm-none-eabi-objdump -h -S  tst_20230522.elf  > "tst_20230522.list"
Finished building: tst_20230522.list
 

10:32:18 Build Finished. 0 errors, 0 warnings. (took 2s.251ms)

***************

work@umtiefs-MBP mpfw_code_apps_tst_20221206 % git merge main
Updating c7c7af1..3d7faa6
Fast-forward
 src/tb/kr/krThread/v_02/krThreadTbl.cpp | 4 ++--
 1 file changed, 2 insertions(+), 2 deletions(-)
work@umtiefs-MBP mpfw_code_apps_tst_20221206 % 


work@umtiefs-MBP mpfw_code_main_tst_20230522 % git merge main                                   
Updating efe8367..a38f9b8
Fast-forward
 .gitignore                                               |   1 -
 doc/note_20230522_build.md                               | 248 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++--------------------------
 src/stm32f769_disco/Inc/freertos/FreeRTOSConfig.h        |  11 +---
 src/stm32f769_disco/Src/freertos/freertos.c              |  57 +++++++++++++++-----
 src/stm32f769_disco/Src/freertos/{port.c => port.c_not}  |   0
 src/stm32f769_disco/Src/interrupt/stm32f7xx_it.c         |  16 +++---
 src/stm32f769_disco/Src/main.cpp                         |   1 +
 wsp/cmake/project/stm32f769_disco/set_cpp_files.cmake    |   2 +-
 wsp/cube/workspace/stm32f769_disco/.metadata/version.ini |   2 +-
 9 files changed, 231 insertions(+), 107 deletions(-)
 rename src/stm32f769_disco/Src/freertos/{port.c => port.c_not} (100%)
work@umtiefs-MBP mpfw_code_main_tst_20230522 % 

---

Wednesday, 28th June 2023

- il problema compare quando si compilano le tabelle dei timer e quindi si usa la funzione delay del freertor


- risoluzione del problema
    - bisogna capire bene come vengono gestiti
        - SysTick
        - Timer
    - nei progetti che usano il FreeRTOS
        - stm_202230504
            - stm
                - qui nel file FreeRTOSConfig.h è ATTIVA la define "#define xPortSysTickHandler SysTick_Handler"
        - FreeRTOS_ThreadCreation
            - stm
                - qui nel file FreeRTOSConfig.h è COMMENTATA la define "#define xPortSysTickHandler SysTick_Handler"
        - tst_20221229
            - nxp
                - qui nel file FreeRTOSConfig.h è ATTIVA la define "#define xPortSysTickHandler SysTick_Handler"
    - la "define xPortSysTickHandler" definisce l'handler del "SysTick"
    - da capire dove e come FreeRTOS attiva l'interrupt SysTick
        - SysTick_CTRL_TICKINT abilita l'interrupt del SysTick

- define not-commented
    - arm-none-eabi-size   stm_20230504.elf 
    -    text	   data	    bss	    dec	    hex	filename
    -   34140	    344	  39800	  74284	  1222c	stm_20230504.elf
    - Finished building: default.size.stdout

- define commented
    - arm-none-eabi-size   stm_20230504.elf 
    -    text	   data	    bss	    dec	    hex	filename
    -   34092	    344	  39800	  74236	  121fc	stm_20230504.elf
    - Finished building: default.size.stdout
- main
    - osKernelStart
    - krThreadManager.cpp
        - vTaskStartScheduler
            - xPortStartScheduler
                - vPortSetupTimerInterrupt
                    - /* Configure SysTick to interrupt at the requested rate. */
                      portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
                      portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );

- FreeRTOS_ThreadCreation ["#define xPortSysTickHandler" COMMENTED]
    - stm32f7xx_it.c
        - SysTick_Handler
            - osSystickHandler
                - xPortSysTickHandler

- tst_20230504 ["#define xPortSysTickHandler" NOT COMMENTED]
    - stm32f7xx_it.c
        - SysTick_Handler

- tst_20230522 ["#define xPortSysTickHandler" NOT COMMENTED]
    - stm32f7xx_it.c
        - SysTick_Handler


---

Tuesday, 27th June 2023

- risoluzione del problema
    - l'idea che mi sono fatto è la seguente
        - per usare il FreeRTOS si deve configurare il SysTick timer, per questa cosa c'è un file sorgente di template
        - qualora non venga definito esplicitamente il SysTick timer viene preso uno di default 

- gestione interrupt
    - è necessario impostare sempre la priorità di un interrupt
    - stabilire quali sono gli interrupt usati dal freertos
        - sembrerebbe
            - SVCall
            - PendSV

- FreeRTOS_ThreadCreation project
    - gerarchia delle chiamate
        - void SysTick_Handler(void)
            - osSystickHandler()
                - xPortSysTickHandler()


void xPortSysTickHandler( void )
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	portENABLE_INTERRUPTS();
}


---

Monday, 26th June 2023

- esclude directory from search
    - main/mpfw_code_main_stm_20230420, main/mpfw_code_main_stm_20230424, main/mpfw_code_main_stm_20230501, libs/mpfw_code_libs_sdk_stm32f769_disco/src/v_00
- scopo
    - far girare i moduli fw2_wrapper_core, fw2_wrapper_resources sulla piattaforma stm32f769_disco
        - la main_app "stm_20230504" implementata un applicazione "nulla" in cui viene inizializzato il freertos ed in cui viene implementato il modulo "fw2_wrapper_core"
        - la main_app "stm_20230505" implementata un applicazione che gestisce la seriale in cui viene inizializzato il freertos
        - le main_app "stm_20230516" e "stm_20230520" implementano il modulo "fw2_wrapper_resources"
        - il passo successivo è quello di inglobare il modulo app "tst_20221206"
        - questo passo è compiuto con la main_app "tst_20230522"
            - ci sono problemi però con la gestione del modulo "fw2_wrapper_core"
                - sembra che lo scheduler del freertos parta subito o comunque il timer che lo gestisce è attivato prima di impostare/inizializzare i threads

- funzioni da analizzare
    - MPU_Config();
        - the same
        - HAL_MPU_Disable();
        - HAL_MPU_ConfigRegion(&MPU_InitStruct);
        - HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    - CPU_CACHE_Enable();
    - HAL_Init();
        - #if (ART_ACCELERATOR_ENABLE != 0)
            -   __HAL_FLASH_ART_ENABLE();
        - #if (PREFETCH_ENABLE != 0U)
            -  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
        - HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
        - HAL_InitTick(TICK_INT_PRIORITY);
        - HAL_MspInit();
            - questa funzione viene eseguita solo dalla main_app "stm_20230504" nel file "stm32f7xx_hal_msp.c" presente nel proprio modulo
                -  __HAL_RCC_PWR_CLK_ENABLE();
                -  __HAL_RCC_SYSCFG_CLK_ENABLE();
                -  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
                    - __NVIC_PRIO_BITS definita di default per la scheda di sviluppo
                        - nxp la definisce mentre stm no
    - SystemClock_Config();
    - InitUART();
        - the same

- cosa sono
    - SVCall & PendSV: system service call via SWI instruction
        - vedere "pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf"

- sdk_uart_prv.c
    - SystemClock_Config
        - ret = HAL_PWREx_EnableOverDrive(); [stm32f7xx_hal_pwr_ex.c]
            - __HAL_RCC_PWR_CLK_ENABLE
        - ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
- InitUART();
    - HAL_GPIO_Init
        - __HAL_RCC_SYSCFG_CLK_ENABLE

- BSP_PB_Init() [funzio che abilita gli interrupt]

- stm_20230504
    - stm32f7xx_hal_timebase_tim.c
        - HAL_InitTick

---

Friday, 26th May 2023

- sono rimasto che ho testato la main_app stm_20230504 (quella con linizializzazione di quasi tutti i moduli hal) mettendo un ciclo while infinito prima della definizione e poi creazione del thread 
    - ho avviato e non si è bloccato o comunque il systick non ha dato errore
        - l'erroere rammento è dovuto al fatto che risulta una lista non vuota mentre lo è
            - xTaskIncrementTick
            - task.c: riga 2702 -> 				if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
        - si potrebbe provare a mettere un breackpoint nel systick per vedere se vi entra 
