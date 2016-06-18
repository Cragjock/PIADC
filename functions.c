#include "ads1015.h"

char* mux_type[]=
{
    "Ain_p=Ain0 & Ain_n=Ain1",
    "Ain_p=Ain0 & Ain_n=Ain3",
    "Ain_p=Ain1 & Ain_n=Ain3",
    "Ain_p=Ain2 & Ain_n=Ain3",
    "Ain_p=Ain0 & Ain_n=gnd",
    "Ain_p=Ain1 & Ain_n=gnd",
    "Ain_p=Ain2 & Ain_n=gnd",
    "Ain_p=Ain3 & Ain_n=gnd"
};

/*** Samples per second ***/
unsigned int data_rates[8] = {128, 250, 490, 920, 1600, 2400, 3300, 3300};

/*** PGA fullscale voltages in mV ***/
unsigned int fullscale[8] = {6144, 4096, 2048, 1024, 512, 256, 256, 256 };

char* c_mode[]={"Continuous convert", "Single-shot convert"};

enum channel {MUX0, MUX1, MUX2, MUX3, MUX4, MUX5, MUX6, MUX7};
enum D_R {DR128SPS, DR250SPS, DR490SPS, DR920SPS, DR1600SPS, DR2400SPS, DR3300SPS, DR3301SPS};
enum FScale {mv6144, mv4096, mv2048, mv1024, mv512, mv256};

//myADS1015 ADC = {0x48, mv2048, 1, MUX4, DR250SPS};
myADS1015 ADC = {0x48, mv4096, 0, MUX0, DR250SPS};

myADS1015 arrADC[4]=  {
                    {0x48, mv4096, 0, MUX0, DR250SPS},
                    {0x48, mv2048, 1, MUX1, DR250SPS},
                    {0x48, mv1024, 1, MUX4, DR1600SPS},
                    {0x48, 0, 0, 0, 0},
                };


//myADS1015 ADC = {0x48, mv2048, 1, MUX4, DR250SPS};

const char * i2cdev[2] = {"/dev/ic2-0","/dev/i2c-1"};
static int slave_address = 0x48;
uint16_t init_config_reg = 0;


/************************
    POST MENU
*************************/
int postmenu()
{
        printf("Select a menu item:\n\
            1: ADS1015_op_init\n\
            2: Normal mode\n\
            3: read_config\n\
            4: read conversion reg\n\
            5: All LED off\n\
            6: All LED on\n\
            7: rate up/down\n\
            8: check MODE reg\n\
            9: quit\n\
           Your choice is: ");

    //choice = getchar();
    //choice = gets_s(input, 5);
    fgets(input, 5, stdin);
    choice=input[0];

    //char nl =getchar();
    //int yes=fflush(stdin); works on Windows but not Linux

    printf("\nyou selected: ");
    putchar(choice);

    return choice;
}

UINT read_config_reg(int file)
{
    UINT result = 67;   //BS number to start
    UINT resultswap = 17;

    result=i2c_smbus_read_word_data(file, Config_Reg);
    printf("read config register: %x \n", result);

    resultswap=myI2C_read_swap(file, Config_Reg);
    printf("read swapped config register: %x \n", resultswap);

    myADS1015 current_CR;
    current_CR.data_rate = (result & 0x0e0) >> CR_DR0;
    current_CR.Mux = (result & 0x7000) >> CR_MUX0;    current_CR.PGA = (result &0x0E00 ) >> CR_PGA0;

    arrADC[3].data_rate = (resultswap & 0x0e0) >> CR_DR0;
    arrADC[3].Mux = (resultswap & 0x7000) >> CR_MUX0;
    arrADC[3].PGA = (resultswap &0x0E00 ) >> CR_PGA0;

    printf("current config reg is: %x\ndata rate is: %x\nmux is: %x\nPGA is: %x\n",
                    resultswap,
                    arrADC[3].data_rate,
                    arrADC[3].Mux,
                    arrADC[3].PGA);

    printf("Mux number is: %s \n", mux_type[arrADC[3].Mux]);
    printf("PGA setting is: %d \n", fullscale[arrADC[3].PGA]);
    printf("Data rate is: %d \n", data_rates[arrADC[3].data_rate]);

    return resultswap;
}

/**************************************
    Initialize bus and ADC
**************************************/
int ADS1015_Init(const char* devname)
{
    file= I2C_Open(1, slave_address);
    //file = open(devname, O_RDWR);

    init_config_reg = mux_diff_1 | PGA_4096 | DR_250sps | MODE_CONTINUOUS | COMP_QUE_DISABLE;
    /// should be 0x0223
    myI2C_write_swap(file, Config_Reg, init_config_reg);

    return file;
}

/*************/
int ADS1015_op_init(int file)   // maybe not needed
{
    //unsigned int i = 1;
    //char *c = (char*)&i;
    //if (*c)
    //   printf("Little endian\n");
    //else
    //   printf("Big endian\n");;

    UINT result = 0x1234; // BS number
    UINT res = 0x4823;
    UINT result1 = 0x5678;

    //result=i2c_smbus_read_word_data(file, Config_Reg);
    result = myI2C_read_swap(file, Config_Reg);
    printf("the op intit config register is x%x: \n", result);

    result1 = (result & 0x1000); // check if still converting or not
    if((result & 0x1000) == 0x1000)             // if (x & MASK) = MASK, ok
        printf("Not doing a conversion\n");
    else
        printf("conversion in process\n");

    result1 = ((result & 0x7000)>> 12);
    printf("Mux number is :%s \n", mux_type[result1]);       // which MUX type

    result1 = ((result >>5) & 0x0007);
    printf("Data rate is :%d \n", data_rates[result1]);        //data rate
    result1 = ((result >>9) & 0x0007);
    printf("PGA setting is :%d \n\n\n\n", fullscale[result1]);

	UINT config = res;
	config &= 0x001f;                   // strip for COMP items
	config |= (0 << 15) | (0 << 8);     // set for continuous
	config |= (ADC.Mux & 0x0007) << 12;
	config |= (ADC.PGA & 0x0007) << 9;
	config |= (ADC.data_rate & 0x0007) << 5;

    result1 = ((config & 0x7000)>> 12);
    printf("Mux number is :%s \n", mux_type[result1]);       // which MUX type

    result1 = (config >>5)& 0x0007;
    printf("Data rate is: %d SPS\n", data_rates[result1]);        //data rate

    result1 = (config >>9) & 0x0007;
    printf("PGA setting is: %d mv\n", fullscale[result1]);

    result1 = (config >>8)& 0x0001;
    printf("Current mode is: %s\n", c_mode[result1]);

    //=== get the byte order correct ====
    buf[2]=HBYTE(config);
    buf[3]=LBYTE(config);
    result=(buf[3]<<8) | buf[2];    //x = lobyte << 8 | hibyte;
    printf("byte order correct config reg: 0x%x\n", result);

    result=i2c_smbus_write_word_data(file, Config_Reg, result);
    //result=myI2C_write_swap(file, Config_Reg, result);

    //result=i2c_smbus_write_word_data(file, Config_Reg,0x4302);
    //result=i2c_smbus_write_word_data(file, Config_Reg,config);

    //result=i2c_smbus_read_word_data(file, Config_Reg);
    result=read_config_reg(file);
    //result = myI2C_read_swap(file, Config_Reg);

    //uint16_t sdc = bswap_16(result); // grt the right order to display
    printf("the op intit config register is x%x: \n", result);

    return 0;
}


/**< KEEP THIS ************** */
int read_convert_register(int file)
{

do
{
    //SINT result = 23; // BS number
    int16_t result = 0x1234; // due to __s32 i2c_smbus_read_word_data(int file, __u8 command)
    int16_t result_sw = 0x7893;
    uint16_t result1= 0x8312; // BS data

    //result = i2c_smbus_read_word_data(file, Convert_Reg);       // read the data
    result = myI2C_read_swap(file, Convert_Reg);                // read the data
    printf("raw convert register count is: 0x%x\n", result);

    /************************************
        conversion is:
        (conversion register count)/16 (right shift 4 bits)
        then times the gain, this gives volt count times 1000
        then divide 1000 to set decimal place correctly
    ************************************/




    // check if negative
    if((result & SIGN_MASK) == SIGN_MASK)
        result = -(~(result)+1);

    result = (result>>4)*2;         // assumes gain is 2
    printf("===================");
    printf("the voltage (x1) is: %2.3f\n", (float)result/1000);

    float PCAcount = ((float)result/1000*80)+320;
    printf("Test equation for servo count: %2.3i\n", (int)PCAcount);

    //result=i2c_smbus_read_word_data(file, Config_Reg);
    result = myI2C_read_swap(file, Config_Reg);
    printf("the config register is: x%x\n", result);

    sleep(2);

}
while(1);
    return 0;
}

/*******************************************************************/
int I2C_Open(int bus, int addr)
{
    int file;
    file = open(i2cdev[bus], O_RDWR);
    if (file == -1)
        {
            perror(i2cdev[bus]);
            int errsv = errno;
            return -1;
        }

    if (ioctl(file, I2C_SLAVE, addr) < 0)
    {
        perror("Failed to acquire bus access and/or talk to slave");
        //exit(1);
        return -1;
    }
    return file;
}

void I2C_Close(int filep)
{
    close(filep);
}

/*********************************************************/
int myI2C_write_data(int file, uint8_t command_reg, uint8_t data)
{
    int res = i2c_smbus_write_byte_data(file, command_reg, data);
    /** S Addr Wr [A] Comm [A] Data [A] P **/
    if (res<0)
    {
        printf("result i2c write error");
        return -1;
    }
    return 0;
}
/***********************************************************/
int32_t myI2C_read_data(int file, uint8_t command)
{
        int32_t res = i2c_smbus_read_byte_data(file, command);
        if (res < 0)
        {
            printf("Read error");
            return -1;
        }
        return res;      // return the read data
}

int myI2C_write_swap(int file, uint8_t command_reg, uint16_t data)
{
    /** use the byte swap header **/
    uint16_t data_swap = bswap_16(data);

    printf("write swap: data in: %x, data swapped: %x\n", data, data_swap);

    int res = i2c_smbus_write_word_data(file, command_reg, data_swap);
    /** S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P **/
    if (res<0)
    {
        printf("result i2c write error");
        return -1;
    }
    return 0;
}
/***********************************************************/
int16_t myI2C_read_swap(int file, uint8_t command)
{
        int16_t res = i2c_smbus_read_word_data(file, command);
        /** S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P **/
        if (res == -1)
        {
            printf("Read error");
            return -1;
        }

        /** use the byte swap header **/
        uint16_t res_swap = bswap_16(res);
        printf("read swap: data in: %x, data swapped: %x\n", res, res_swap);

        return res_swap;      // return the read data
}
