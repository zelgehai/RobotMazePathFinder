#include "../inc/OPT3101.h"

// edited by Valvano and Valvano 12/22/2019
// hardware
// GND    ground
// VIN    3.3V
// SDA    OPT3101 pin 14 <> P6.4 I2C data SDA_S
// SCL    OPT3101 pin 13 <- P6.5 I2C clock SCL_S
// GP1    OPT3101 pin 11 -> P6.2/AUXR/GP1 input from the DATA_RDY
// GP2    OPT3101 pin 12 -> P10.1 Not used
// RST_MS OPT3101 pin 17 <- P6.3/AUXL/nRST_MS output low to reset the OPT3101
#define I2C_ADDRESS 0x58

// (speed of light) / (2 * 10 MHz * 0x10000) = 0.22872349395 mm
// Valvano removed floating point, converted to binary fixed point
#define MM_PER_PHASE_COUNT 0.22872349395
#define BinFixMM_PER_PHASE_COUNT 14990 // divided by 65536
#define BinFix 16
// We will do 128 sub-frames per frame.
#define SUB_FRAME_COUNT 128

// Time constant of the crosstalk filter.  This is set according to equation 6,
// section 4.2.1, of sbau310.pdf, and depends on SUB_FRAME_COUNT above.
#define XTALK_FILT_TIME_CONST 3

// Assuming SUB_VD_CLK_CNT has not been changed, each sub-frame takes 0.25 ms,
// and each frame will take 32 ms.  The datasheet documentation of
// ILLUM_XTALK_CALIB and INT_XTALK_CALIB says we should wait at least
// 5 << XTALK_FILT_TIME_CONST frames to get good crosstalk readings.
// (5 << 3) * 128 * (0.25 ms) = 1280
#define CROSSTALK_SETTLING_TIME_MS 1280

static uint32_t reg08, reg09;

uint32_t OPT3101_ReadRegister(uint8_t address)
{
    uint8_t buffer[3];
    EUSCI_B1_I2C_Send_Multiple_Bytes(I2C_ADDRESS, &address, 1);
    EUSCI_B1_I2C_Receive_Multiple_Bytes(I2C_ADDRESS, buffer, 3);
    return buffer[0] + ((uint32_t)buffer[1] << 8) + ((uint32_t)buffer[2] << 16);
}

void OPT3101_WriteRegister(uint8_t address, uint32_t data)
{
    uint8_t buffer[] = {
    address,
    data & 0xFF,
    data >> 8 & 0xFF,
    data >> 16 & 0xFF
    };

    EUSCI_B1_I2C_Send_Multiple_Bytes(I2C_ADDRESS, buffer, sizeof(buffer));
}

void OPT3101_Init(void)
{
    // Drive P6.3/AUXL/nRST_MS low to reset the OPT3101, then drive it high.
    P6->OUT &= ~0x08;
    P6->DIR |= 0x08;
    Clock_Delay1ms(1);
    P6->OUT |= 0x08;
    Clock_Delay1ms(1);

    // Make P6.2/AUXR be an input for the DATA_RDY signal.
    P6->DIR &= ~0x04;

    // Set up P6.2/AUXR to detect low-to-high transitions.
    P6->IES &= ~0x04;

    // Clear the P6.2/AUXR interrupt flag.
    P6->IFG &= ~0x04;

    // Wait until INIT_LOAD_DONE gets set to 1.
    while(!(OPT3101_ReadRegister(0x03) & 0x100))
    {
        Clock_Delay1ms(1);
    }
}

//uint32_t Reg2a;
//uint32_t Reg27;
void OPT3101_Setup(void)
{
    // Set the overload flag observation window (TG_OVL_WINDOW_START).
    // This choice comes from the OPT3101 Configurator Tool version 0.8.0.
    OPT3101_WriteRegister(0x89, 7000);

    // Enable the temperature sensor.
    // This choice comes from the OPT3101 Configurator Tool version 0.8.0.
    uint32_t reg6e = OPT3101_ReadRegister(0x6e);
    reg6e |= 0x80000;  // EN_TEMP_CONV = 1
    OPT3101_WriteRegister(0x6e, reg6e);

    // Turn on clip mode for frequency-correction to phase.
    // This choice comes from the OPT3101 Configurator Tool version 0.8.0.
    uint32_t reg50 = OPT3101_ReadRegister(0x50);
    reg50 |= 1;
    OPT3101_WriteRegister(0x50, reg50);

    // Set NUM_SUB_FRAMES to SUB_FRAME_COUNT - 1.
    // Set NUM_AVG_SUB_FRAMES to the same value so we are averaging together
    // every sub-frame.
    OPT3101_WriteRegister(0x9f,
    (uint32_t)(SUB_FRAME_COUNT - 1) << 12 | (SUB_FRAME_COUNT - 1));

// test different modulation frequency
  // By default, SUB_VD_CLK_CNT is 9,999 for 10,000 TD clocks in a sub-
  // frame.  Changing this should change the modulation frequency.  Total
  // time of one sub-frame is 25 nsec * (SUB_VD_CLK_CNT + 1).
/*  uint32_t reg80 = OPT3101_ReadRegister(0x80);
    reg80 = (reg80 & ~0x0001FFFE) | (uint32_t)((60000 - 1) << 1);
    OPT3101_WriteRegister(0x80, reg80);*/
// end of test different modulation frequency

  // Set XTALK_FILT_TIME_CONST to the corresponding value.
    uint32_t reg2e = OPT3101_ReadRegister(0x2e);
    reg2e = (reg2e & ~0xF00000) | (uint32_t)XTALK_FILT_TIME_CONST << 20;
    OPT3101_WriteRegister(0x2e, reg2e);

    // Set up the GPIO1 pin (which is connected to P6.2/AUXR) to be
    // a data-ready signal.
    uint32_t reg78 = OPT3101_ReadRegister(0x78);
    uint32_t reg0b = OPT3101_ReadRegister(0x0b);
    reg78 |= 0x1000;                   // GPIO1_OBUF_EN = 1
    reg78 = (reg78 & ~0x1C0) | 0x080;  // GPO1_MUX_SEL = 2 (DIG_GPO_0)
    reg0b = (reg0b & ~0xF) | 9;        // DIG_GPO_SEL0 = 9 (DATA_RDY)
    OPT3101_WriteRegister(0x78, reg78);
    OPT3101_WriteRegister(0x0b, reg0b);


    uint32_t reg2a = OPT3101_ReadRegister(0x2a);
    reg2a |= 0x8000;  // EN_ADAPTIVE_HDR = 1 : Adaptive HDR
    reg2a |= 1;       // EN_TX_SWITCH = 1    : Automatic channel-switching.
    OPT3101_WriteRegister(0x2a, reg2a);
    //    Reg2a = OPT3101_ReadRegister(0x2a); // debugging


  // Settings from the OPT3101 Configurator Tool 0.8.0 for monoshot mode.
  // No deep sleep, using the minimal startup delay allowed by the tool.
    uint32_t reg27 = OPT3101_ReadRegister(0x27);
    reg27 |= 3;                             // MONOSHOT_MODE = 3
    reg27 = (reg27 & 0xFFFF03) | (1 << 2);  // MONOSHOT_NUMFRAME = 1
    OPT3101_WriteRegister(0x27, reg27);
//    Reg27 = OPT3101_ReadRegister(0x27); // debugging

    uint32_t reg76 = OPT3101_ReadRegister(0x76);
    reg76 |= 0x001;  // DIS_GLB_PD_REFSYS = 1
    reg76 |= 0x020;  // DIS_GLB_PD_AMB_DAC = 1
    reg76 |= 0x100;  // DIS_GLB_PD_OSC = 1
    OPT3101_WriteRegister(0x76, reg76);

    uint32_t reg26 = OPT3101_ReadRegister(0x26);
    reg26 = (reg26 & 0x0003FF) | (95 << 10);  // POWERUP_DELAY = 95
    OPT3101_WriteRegister(0x26, reg26);
}

void OPT3101_CalibrateInternalCrosstalk(void)
{
    // Clear TG_EN because the OPT3101 datasheet says EN_SEQUENCER should only be
    // changed while TG_EN is 0.
    uint32_t reg80 = OPT3101_ReadRegister(0x80);
    reg80 &= ~1;  // TG_EN = 0
    OPT3101_WriteRegister(0x80, reg80);

    uint32_t orig_reg2a = OPT3101_ReadRegister(0x2a);
    uint32_t orig_reg2e = OPT3101_ReadRegister(0x2e);

    uint32_t reg2e = orig_reg2e;
    reg2e &= ~(1 << 6);      // USE_XTALK_REG_INT = 0
    reg2e |= 1 << 5;         // USE_XTALK_FILT_INT = 1 : Select filtered IQ.
    reg2e = reg2e & ~0xE00;  // IQ_READ_DATA_SEL = 0
    OPT3101_WriteRegister(0x2e, reg2e);

    uint32_t reg2a = orig_reg2a;
    reg2a &= ~(1 << 15);     // EN_ADAPTIVE_HDR = 0
    OPT3101_WriteRegister(0x2a, reg2a);

    uint32_t reg14 = OPT3101_ReadRegister(0x14);
    reg14 &= ~(1 << 16);     // EN_SEQUENCER = 0
    reg14 &= ~(1 << 17);     // EN_PROCESSOR_VALUES = 0
    OPT3101_WriteRegister(0x14, reg14);

    reg80 |= 1;  // TG_EN = 1
    OPT3101_WriteRegister(0x80, reg80);

    reg2e |= 1 << 4;  // INT_XTALK_CALIB = 1 : Start the calibration.
    OPT3101_WriteRegister(0x2e, reg2e);

    Clock_Delay1ms(CROSSTALK_SETTLING_TIME_MS);

    // The internal crosstalk values could be read from registers 0x3b and 0x3c
    // at this point.

    reg80 &= ~1;  // TG_EN = 0
    OPT3101_WriteRegister(0x80, reg80);

    OPT3101_WriteRegister(0x2a, orig_reg2a);
    OPT3101_WriteRegister(0x2e, orig_reg2e);

    reg80 |= 1;   // TG_EN = 1
    OPT3101_WriteRegister(0x80, reg80);
}

void OPT3101_StartMeasurement(void)
{
    // Set MONOSHOT_BIT to 1 to trigger a new measurement.
    // Assumption: The other bits in register 0 should be 0.
    OPT3101_WriteRegister(0x00, 0x800000);
}

// ch is 0,1,2 for channel
void OPT3101_StartMeasurementChannel(uint32_t ch)
{
    if(ch <= 2)
    {
    uint32_t reg2a = OPT3101_ReadRegister(0x2a);
    reg2a |= 0x8000;  // EN_ADAPTIVE_HDR = 1 : Adaptive HDR
    reg2a &= ~0x07;   // EN_TX_SWITCH = 0    : Manual channel-switching.
    reg2a |= ch<<1;   // SEL_TX_CH bits 2:1 is 0,1,2
    OPT3101_WriteRegister(0x2a, reg2a);  // Set MONOSHOT_BIT to 1 to trigger a new measurement.
    }
    // Assumption: The other bits in register 0 should be 0.
    OPT3101_WriteRegister(0x00, 0x800000);
}

void OPT3101_ReadMeasurement(void)
{
    reg08 = OPT3101_ReadRegister(0x08);
    reg09 = OPT3101_ReadRegister(0x09);
}

bool OPT3101_MeasurementError(void)
{
    if (!((reg08 >> 20) & 0x0001))
    {
        // FRAME_STATUS is 0: The frame is invalid.
        return true;
    }

    if (reg09 >> 18 & 3)
    {
        // PHASE_OVER_FLOW_F2 or SIG_OVL_FLAG is 1.
        return true;
    }

    return false;
}

uint32_t OPT3101_GetTxChannel(void)
{
    return (reg08 >> 18) & 0x0003;  // TX_CHANNEL
}

uint32_t OPT3101_GetAmplitude(void)
{
    return reg09 & 0xFFFF;  // AMP_OUT
}
//
uint32_t OPT3101_GetPhase(void)
{
    return reg08 & 0xFFFF;  // PHASE_OUT
}

uint32_t OPT3101_GetDistanceMillimeters(void)
{
    // return OPT3101_GetPhase() * MM_PER_PHASE_COUNT; // removed floating point
    return (OPT3101_GetPhase() * BinFixMM_PER_PHASE_COUNT)>>BinFix;
}

// Returns true if we have a new distance measurement ready to be read, false otherwise.
bool OPT3101_CheckDistanceSensor(void)
{
    if (!(P6->IFG & (1 << 2)))
    {
        // Data is not ready yet.
        return false;
    }
    return true;
}
uint32_t ChannelCount[3]; // debugging monitor

uint32_t OPT3101_GetMeasurement(uint32_t distances[3], uint32_t amplitudes[3])
{
    uint32_t channel,distance,amplitude;
    OPT3101_ReadMeasurement();

    distance  = OPT3101_GetDistanceMillimeters();
    amplitude = OPT3101_GetAmplitude();
    channel   = OPT3101_GetTxChannel();

    if (OPT3101_MeasurementError())
    {
        // Something went wrong getting the measurement.
        distance = 65535;
    }

    else if (amplitude < 150)
    {
        // Low amplitude: ignore the distance.
        distance = 65534;
    }

    else if (distance > 10000)
    {
        // The distance measurement probably underflowed and wrapped around
        // to a really big number (because of imperfect phase offset
        // calibration), so report 65533.
        distance = 65533;
    }

    // Clear the pin-change interrupt flag.
    P6->IFG &= ~(1 << 2);
    if (channel <= 2)
    {
        ChannelCount[channel]++;
        distances[channel]  = distance;
        amplitudes[channel] = amplitude;
    }
    return channel;
}

uint32_t *PTxChan;
uint32_t *Pdistances;
uint32_t *Pamplitudes;

void OPT3101_ArmInterrupts(uint32_t *pTxChan, uint32_t distances[3], uint32_t amplitudes[3])
{
    PTxChan = pTxChan;
    Pdistances = distances;
    Pamplitudes = amplitudes;
    // Make P6.2/AUXR be an input for the DATA_RDY signal.
    P6->DIR &= ~0x04;
    // Set up P6.2/AUXR to detect low-to-high transitions.
    P6->IES &= ~0x04;
    // Clear the P6.2/AUXR interrupt flag.
    P6->IFG &= ~0x04;
    P6->IE = 0x04;  // arm interrupt only on P6.2
    NVIC->IP[40] = 0x40; // priority 2
    NVIC->ISER[1] = 0x00000100;  // enable interrupt 40 in NVIC
}

uint32_t ISRTime; // bus cycle time (1us)
uint32_t ISRLast;    // last time (20.83ns)
uint32_t ISRPeriod;

// *PTxChan set to 0,1,2 when measurement done
void PORT6_IRQHandler(void)
{
    *PTxChan = OPT3101_GetMeasurement(Pdistances,Pamplitudes);
    P6->IFG = 0x00;            // clear all flags
}
