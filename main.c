#include"ads1015.h"

const char * devName = "/dev/i2c-1";
int file;


int main()
{
    file = ADS1015_Init(devName);   // set the device and i2C items
do
{
    test1 = postmenu();
    switch (test1)
        {
            case Initialize:
                printf("\nyou selected: ADS1015_op_init\n");
                ADS1015_op_init(file);
                break;
            case normalMode :
                printf("\nyou selected: normalMode\n");
                break;
            case set_one_channel:
                printf("\nyou selected: read conversion\n");
                read_register(file);
                break;
            case set_all_channel:
                printf("\nyou selected: read config register\n");
                config_reg_write(file);
                break;
            case allLED_off:
                printf("\nyou selected: allLED_off\n");
                break;
            case allLED_on:
                printf("\nyou selected: allLED_on\n");
                break;
            case rate_up_down:
                printf("\nyou selected: rate_up_down\n");
                break;
            case checkMODE_reg:
                printf("\nyou selected: checkMODE_reg\n");
                break;
            //case '9':
            //    printf("\nyou selected: 9");
            //    break;
            default:
                printf("\nyou selected: default\n");
                break;
        }

}
    while(test1 != good_bye);

    close(file);
    return 0;
}
