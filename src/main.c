/*!
    \file    main.c
    \brief   led spark with systick
    
    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "exmc_norflash.h"
//#include "gd32f450z_eval.h"


#define BUFFER_SIZE               0x100
#define WRITE_READ_ADDR           0xfff200

nor_id_struct nor_id;
uint32_t writereadstatus = 0;
uint16_t writebuffer[BUFFER_SIZE];
uint16_t readbuffer[BUFFER_SIZE];
nor_status_struct status;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{

    systick_config();

    exmc_norflash_init(EXMC_BANK0_NORSRAM_REGION1);
    /* read nor flash id and printf */
    nor_read_id(&nor_id);

    nor_return_to_read_mode();

    /* erase the nor flash block to be written data */
    status = nor_eraseblock(WRITE_READ_ADDR);


    /* fill writeBuffer with the specified value */
    nor_fill_buffer(writebuffer, BUFFER_SIZE, 0x0006);

    /* write data to nor flash, WRITE_READ_ADDR: the starting address of the write data */
    status = nor_write_buffer(writebuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    if(NOR_SUCCESS == status) {
        printf("\r\nwrite data to nor flash block successfully!");
    } else {
        printf("\r\nwrite data to nor flash block failure!");

        /* failure, light on LED3 */
        gd_eval_led_on(LED3);
        while(1);
    }

    /* read data from nor flash, WRITE_READ_ADDR: the starting address of the read data*/
    nor_readbuffer(readbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    /* read and write data comparison for equality */
    writereadstatus = 0;
    for(index = 0x00; index < BUFFER_SIZE; index++) {
        if(readbuffer[index] != writebuffer[index]) {
            writereadstatus++;
            break;
        }
    }
    while (1){
    }
}
