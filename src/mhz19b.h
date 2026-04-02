#include <stdint.h>


typedef struct tag_mhz19b_data
{
    uint co2_value;
    uint co2_counter;
    uint cs_error_counter;
    uint cmd_error_counter;


} MHZ19B;


void mhz19b_init();
bool mhz19b_test();
void mhz19b_read();
MHZ19B* mhz19b_data();
void mhz19b_protocolreset();


