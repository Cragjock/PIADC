// ===================================
// ===================================

#ifndef __ads1015_h_
#define __ads1015_h_

#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
#include <fcntl.h>
#include <time.h>
#include <byteswap.h>

#define LBYTE(LB) (LB & 0x0FF)
#define HBYTE(HB) ((HB & 0xFF00)>>8)

#define I2C_SLAVE_ADDRESS 0x48    // device slave address, hard coded

#define Pointer_Reg 0x03
#define Convert_Reg 0x00
#define Config_Reg  0x01
#define LO_Thresh_Reg 0x02
#define HI_Thresh_Reg 0x03

/// Config Register define and bit location
#define CR_OS 15
#define CR_MUX2 14
#define CR_MUX1 13
#define CR_MUX0 12
#define CR_PGA2 11
#define CR_PGA1 10
#define CR_PGA0 9
#define CR_MODE 8
#define CR_DR2 7
#define CR_DR1 6
#define CR_DR0 5
#define CR_COMP_MODE 4
#define CR_COMP_POL 3
#define CR_COMP_LAT 2
#define CR_COMP_Q1 1
#define CR_COMP_Q2 0

/// input MUX: differential
#define mux_diff_1 0x0000      /// Ain_p=Ain0 & Ain_n=Ain1 default
#define mux_diff_2 0x1000      /// Ain_p=Ain0 & Ain_n=Ain3
#define mux_diff_3 0x2000      /// Ain_p=Ain1 & Ain_n=Ain3
#define mux_diff_4 0x3000      /// Ain_p=Ain2 & Ain_n=Ain3
/// input MUX: single ended
#define mux_single_1 0x4000    /// Ain_p=Ain0 & Ain_n=gnd
#define mux_single_2 0x5000    /// Ain_p=Ain1 & Ain_n=gnd
#define mux_single_3 0x6000    /// Ain_p=Ain2 & Ain_n=gnd
#define mux_single_4 0x7000    /// Ain_p=Ain3 & Ain_n=gnd
/// Programmable gain amplifier
#define PGA_6144 0x0000
#define PGA_4096 0x0200
#define PGA_2048 0x0400 /// default
#define PGA_1024 0x0600
#define PGA_0512 0x0800
#define PGA_0256 0x0A00
#define PGA_0256 0x0C00
#define PGA_0256 0x0E00
/// Sampling data rates
#define DR_128sps 0x0000
#define DR_250sps 0x0020
#define DR_490sps 0x0040
#define DR_920sps 0x0060
#define DR_1600sps 0x0080  /// default
#define DR_2400sps 0x00A0
#define DR_3300sps 0x00C0
#define DR_3300sps 0x00E0

#define MODE_CONTINUOUS 0x0
#define MODE_SINGLE_SHOT 0x0100

#define COMP_MODE_HYSTERESIS 0x0 /// default
#define COMP_MODE_WINDOW 0x10
#define COMP_LATCH_OFF 0x0 /// default
#define COMP_LATCH_ON 0x4
#define COMP_QUE_ASSERT_1 0x0
#define COMP_QUE_ASSERT_2 0x1
#define COMP_QUE_ASSERT_4 0x2
#define COMP_QUE_DISABLE 0x3 /// default

#define SIGN_MASK 0x8000     // bit 12 test

#define MODE1           0x00
#define MODE2           0x01

char buf[10];
typedef unsigned short int UINT;
typedef short int SINT;
typedef unsigned char BYTE;     // for 8 bit items
typedef unsigned int WORD;      // for 16 bit items
char input[5];
extern int file;
int choice;


typedef struct _myADS1015
{
    int i2c_address;
    UINT PGA;            // set to 2 but can have 8 channel to select from
    int opMode;         // set to continuous
    UINT Mux;            // differential input, INP=IN0, INN=IN1, but can have 8 channel to select from
    UINT data_rate;
} myADS1015;

enum _menuitems
{
    Initialize=0x31,        // set up operation, 0x31 for ASCII
    normalMode,             // continuous single
    set_one_channel,        // one shot
    set_all_channel,        // continuous differential
    allLED_off,
    allLED_on,
    rate_up_down,
    checkMODE_reg,
    good_bye
} menuitems, test1;

int postmenu();
UINT read_config_reg(int);
int ADS1015_Init(const char* devname);
int read_convert_register(int);
int ADS1015_op_init(int file);
int I2C_Open(int bus, int addr);
void I2C_Close(int filep);
int16_t myI2C_read_swap(int file, uint8_t command);
int myI2C_write_swap(int file, uint8_t command_reg, uint16_t data);

#endif //
//==============================================
// =============================================






