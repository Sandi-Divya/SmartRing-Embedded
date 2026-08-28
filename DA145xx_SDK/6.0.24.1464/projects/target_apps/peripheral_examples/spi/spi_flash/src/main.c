/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SPI flash example
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"
#include "spi.h"
#include "spi_flash.h"

uint8_t rd_data[512];
uint8_t wr_data[512];
uint8_t dev_id;

/**
 ****************************************************************************************
 * @brief SPI Flash test function
 ****************************************************************************************
 */
void spi_flash_test(void);

/**
 ****************************************************************************************
 * @brief Main routine of the SPI flash example
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    spi_flash_test();
    while(1);
}

/**
 ****************************************************************************************
 * @brief SPI and SPI Flash Initialization function
 ****************************************************************************************
 */
static void spi_flash_peripheral_init(void)
{
    // Release the SPI flash memory from power down
    spi_flash_release_from_power_down();

    // Try to auto-detect the SPI flash memory
    spi_flash_auto_detect(&dev_id);

    // Disable the SPI flash memory protection (unprotect all sectors)
    spi_flash_configure_memory_protection(SPI_FLASH_MEM_PROT_NONE);
}

static void print_memory_protection_test(uint32_t *actual_size, unsigned char buf[][2], uint32_t protection_control_field)
{
    // 1. Unprotected memory
    printf_string(UART, "\n\r1) Unprotected memory test\n\r");
    printf_string(UART, "\tPerforming a full erase...\n\r");
    spi_flash_chip_erase_forced();  // the whole memory array gets unprotected in this function

    printf_string(UART, "\n\r2) Retrieving the two bytes at addresses 0x00000 and 0x10000");
    printf_string(UART, "\n\r   Reading [0x00000] = 0x");
    spi_flash_read_data(rd_data, 0, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, " and [0x10000] = 0x");
    spi_flash_read_data(rd_data, 0x10000, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, "\n\r  (must be [0x00000] = 0xFF and [0x10000] = 0xFF, as the memory has been cleared)");

    printf_string(UART, "\n\r3) Writing [0x00000]<- 0xE0 and [0x10000]<- 0x0E to the unprotected memory");
    spi_flash_write_data(&buf[0][0], 0, 1, actual_size);
    spi_flash_write_data(&buf[0][1], 0x10000, 1, actual_size);
    printf_string(UART, "\n\r   Reading [0x00000] = 0x");
    spi_flash_read_data(rd_data, 0, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, " and [0x10000] = 0x");
    spi_flash_read_data(rd_data, 0x10000, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, "\n\r  (must be [0x00000] = 0xE0 and [0x10000] = 0x0E)");

    // 2. Full protected memory
    printf_string(UART, "\n\r4) Enabling memory protection for the whole memory array.");

    spi_flash_configure_memory_protection(protection_control_field);
    printf_string(UART, "\n\r5) Writing [0x00000]<- 0xD0 and [0x10000]<- 0x0D to the fully protected memory");
    spi_flash_write_data(&buf[1][0], 0, 1, actual_size);
    spi_flash_write_data(&buf[1][1], 0x10000, 1, actual_size);
    printf_string(UART, "\n\r   Reading [0x00000] = 0x");
    spi_flash_read_data(rd_data, 0, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, " and [0x10000] = 0x");
    spi_flash_read_data(rd_data, 0x10000, 1, actual_size);
    printf_byte(UART, rd_data[0]);
    printf_string(UART, "\n\r  (must be [0x00000] = 0xE0 and [0x10000] = 0x0E), the old values)");

    printf_string(UART, "\n\r6) Unprotecting the whole memory array and doing a full erase");
    spi_flash_chip_erase_forced();  // the whole memory array gets unprotected in this function
}

/**
 ****************************************************************************************
 * @brief SPI Flash protection features test
 ****************************************************************************************
 */
static void spi_protection_features_test(void)
{
    unsigned char buf[4][2] = {{0xE0, 0x0E}, {0xD0, 0x0D}, {0xB0, 0x0B}, {0x70, 0x07}};
    uint32_t actual_size;
    printf_string(UART, "\n\r\n\r");
    printf_string(UART, "\n\rSPI flash memory protection features demo.\n\r");

    char *name[25];                         // SPI flash memory name
    uint32_t protection_control_field;      // SPI flash memory protect

    switch(dev_id)
    {
        case W25x10CL_DEV_INDEX:
        {
            *name = "W25x10CL";
            printf_string(UART, name[0]);
            protection_control_field = W25x10CL_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case W25x20CL_DEV_INDEX:
        {
            *name = "W25x20CL";
            printf_string(UART, name[0]);
            protection_control_field = W25x20CL_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25xy011_DEV_INDEX:
        {
            *name = "AT25xy011";
            printf_string(UART, name[0]);
            protection_control_field = AT25xy011_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case P25Q10U_DEV_INDEX:
        {
            *name = "P25Q10U";
            printf_string(UART, name[0]);
            protection_control_field = P25Q10U_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case GD25WD20_DEV_INDEX:
        {
            *name = "GD25WD20";
            printf_string(UART, name[0]);
            protection_control_field = GD25WD20_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25xy021A_DEV_INDEX:
        {
            *name = "AT25xy021A";
            printf_string(UART, name[0]);
            protection_control_field = AT25xy021A_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25EU0021A_DEV_INDEX:
        {
            *name = "AT25EU0021A";
            printf_string(UART, name[0]);
            protection_control_field = AT25EU0021A_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25XE041B_DEV_INDEX:
        {
            *name = "AT25XE041B";
            printf_string(UART, name[0]);
            protection_control_field = AT25XE041B_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25FF041A_DEV_INDEX:
        {
            *name = "AT25FF041A";
            printf_string(UART, name[0]);
            protection_control_field = AT25FF041A_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25SF041B_DEV_INDEX:
        {
            *name = "AT25SF041B";
            printf_string(UART, name[0]);
            protection_control_field = AT25SF041B_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25XE041D_DEV_INDEX:
        {
            *name = "AT25XE041D";
            printf_string(UART, name[0]);
            protection_control_field = AT25XE041D_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25XE081D_DEV_INDEX:
        {
            *name = "AT25XE081D";
            printf_string(UART, name[0]);
            protection_control_field = AT25XE081D_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case MX25R8035F_DEV_INDEX:
        {
            *name = "MX25R8035F";
            printf_string(UART, name[0]);
            protection_control_field = MX25R8035F_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT45DB081E_DEV_INDEX:
        {
            *name = "AT45DB081E";
            printf_string(UART, name[0]);
            protection_control_field = AT45DB081E_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25xy512C_DEV_INDEX:
        {
            *name = "AT25xy512C";
            printf_string(UART, name[0]);
            protection_control_field = AT25xy512C_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25EU0011A_DEV_INDEX:
        {
            *name = "AT25EU0011A";
            printf_string(UART, name[0]);
            protection_control_field = AT25EU0011A_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }
        case AT25FF081_DEV_INDEX:
        {
            *name = "AT25FF081";
            printf_string(UART, name[0]);
            protection_control_field = AT25FF081_MEM_PROT_ALL;
            print_memory_protection_test(&actual_size, buf, protection_control_field);
            break;
        }

        default:
        {
            printf_string(UART, "\n\r Not known SPI flash memory device detected");
        }
    }
}

/**
 ****************************************************************************************
 * @brief SPI Flash power mode for Macronix test
 ****************************************************************************************
 */
static void print_MX25R_power_mode(void)
{
    uint32_t power_mode;
    // Get the power mode
    spi_flash_get_power_mode(&power_mode);
    printf_string(UART, "\n\r\n\rMX25R Power Mode: ");
    if ((power_mode & SPI_FLASH_PM_HPM)== SPI_FLASH_PM_HPM)
        printf_string(UART, " High Performance Mode ");
    else
        printf_string(UART, " Low Power Mode ");
}

/**
 ****************************************************************************************
 * @brief SPI Flash Read/Write test
 ****************************************************************************************
 */
static void spi_read_write_test(void)
{
    uint32_t write_size = 512; // set this variable to the desired read size
    uint32_t read_size = 256;  // set this variable to the desired read size
    uint32_t actual_size;
    uint32_t i;

    printf_string(UART, "\r\n\r\n\r\nSPI Flash Erase/Program/Read test...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);

    // Program Page (256 bytes)
    printf_string(UART, "\n\r\n\rPerforming Program Page...");
    spi_flash_page_program(wr_data, 0, 256);
    printf_string(UART, " Page programmed.");

    // Read SPI Flash first 256 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 256 bytes...\r\n");
    spi_flash_read_data(rd_data, 0, read_size, &actual_size);
    // Display Results
    for (i = 0; i < read_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);

    printf_string(UART, "\r\n\r\nPerforming Sector Erase...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);
    printf_string(UART, "Sector erased.");

    // Write data example (512 bytes)
    printf_string(UART, "\r\n\r\nPerforming 512 byte write...");
    spi_flash_write_data(wr_data, 0, 512, &actual_size);
    printf_string(UART, "Data written.");

    // Read SPI Flash first 512 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 512 bytes...");
    spi_flash_read_data(rd_data, 0, write_size, &actual_size);
    // Display Results
    for (i = 0; i < write_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);
}

/**
 ****************************************************************************************
 * @brief SPI Flash Read/Write test
 ****************************************************************************************
 */
static void spi_read_write_test_buffer(void)
{
    uint32_t write_size = 512; // set this variable to the desired read size
    uint32_t read_size = 256;  // set this variable to the desired read size
    uint32_t actual_size;
    uint32_t i;

    printf_string(UART, "\r\n\r\n\r\nSPI Flash Erase/Program/Read Buffer test...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);

    // Program Page (256 bytes)
    printf_string(UART, "\n\r\n\rPerforming Program Page...");
    spi_flash_page_program_buffer(wr_data, 0, 256);
    printf_string(UART, " Page programmed.");

    // Read SPI Flash first 256 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 256 bytes...\r\n");
    spi_flash_read_data_buffer(rd_data, 0, read_size, &actual_size);
    // Display Results
    for (i = 0; i < read_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);

    printf_string(UART, "\r\n\r\nPerforming Sector Erase...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);
    printf_string(UART, "Sector erased.");

    // Write data example (512 bytes)
    printf_string(UART, "\r\n\r\nPerforming 512 byte write...");
    spi_flash_write_data_buffer(wr_data, 0, 512, &actual_size);
    printf_string(UART, "Data written.");

    // Read SPI Flash first 512 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 512 bytes...");
    spi_flash_read_data_buffer(rd_data, 0, write_size, &actual_size);
    // Display Results
    for (i = 0; i < write_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);
}

#if defined (CFG_SPI_DMA_SUPPORT)
static void spi_read_write_test_dma(void)
{
    uint32_t write_size = 512; // set this variable to the desired read size
    uint32_t read_size = 256;  // set this variable to the desired read size
    uint32_t actual_size;
    uint32_t i;

    printf_string(UART, "\r\n\r\n\r\nSPI Flash Erase/Program/Read DMA test...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);

    // Program Page (256 bytes)
    printf_string(UART, "\n\r\n\rPerforming Program Page...");
    spi_flash_page_program_dma(wr_data, 0, 256);
    printf_string(UART, " Page programmed.");

    // Read SPI Flash first 256 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 256 bytes...\r\n");
    spi_flash_read_data_dma(rd_data, 0, read_size, &actual_size);
    // Display Results
    for (i = 0; i < read_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);

    printf_string(UART, "\r\n\r\nPerforming Sector Erase...");
    spi_flash_block_erase(0, SPI_FLASH_OP_SE);
    printf_string(UART, "Sector erased.");

    // Write data example (512 bytes)
    printf_string(UART, "\r\n\r\nPerforming 512 byte write...");
    spi_flash_write_data_dma(wr_data, 0, 512, &actual_size);
    printf_string(UART, "Data written.");

    // Read SPI Flash first 512 bytes
    printf_string(UART, "\r\n\r\nReading SPI Flash first 512 bytes...");
    spi_flash_read_data_dma(rd_data, 0, write_size, &actual_size);
    // Display Results
    for (i = 0; i < write_size; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\r\nBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);
}
#endif

void spi_flash_test(void)
{
    uint16_t man_dev_id;
    uint64_t unique_id;
    uint32_t jedec_id;
    uint32_t actual_size;
    uint32_t i;

    uint16_t read_size = 256;  // set this variable to the desired read size
    wr_data[0] = 0;

    for (i = 1; i < 512; i++)
    {
        wr_data[i] = wr_data[i-1] + 1;
    }

    printf_string(UART, "\n\r******************");
    printf_string(UART, "\n\r* SPI FLASH TEST *");
    printf_string(UART, "\n\r******************\n\r");

    // Enable FLASH and SPI
    spi_flash_peripheral_init();
    // spi_flash_chip_erase();
    // Read SPI Flash Manufacturer/Device ID
    spi_flash_read_rems_id(&man_dev_id);

    spi_cs_low();

    spi_cs_high();

    // Erase flash
    spi_flash_chip_erase_forced();

    // Read SPI Flash first 256 bytes
    printf_string(UART, "\n\r\n\rReading SPI Flash first 256 bytes...");
    spi_flash_read_data(rd_data, 0, read_size, &actual_size);
    // Display Results
    for (i = 0; i < read_size ; i++)
    {
        printf_byte(UART, rd_data[i]);
        printf_string(UART, " ");
    }
    printf_string(UART, "\n\r\n\rBytes Read: 0x");
    printf_byte(UART, (actual_size >> 8) & 0xFF);
    printf_byte(UART, (actual_size) & 0xFF);
    printf_string(UART, "\n\r");

    // Read SPI Flash JEDEC ID
    spi_flash_read_jedec_id(&jedec_id);
    printf_string(UART, "\n\rSPI flash JEDEC ID is ");
    printf_byte(UART, (jedec_id >> 16) & 0xFF);
    printf_byte(UART, (jedec_id >> 8) & 0xFF);
    printf_byte(UART, (jedec_id) & 0xFF);

    switch (dev_id)
    {
        case W25x10CL_DEV_INDEX:
            printf_string(UART, "\n\rYou are using W25X10 (1-MBit) SPI flash device.");
        break;

        case W25x20CL_DEV_INDEX:
            printf_string(UART, "\n\rYou are using W25X20 (2-MBit) SPI flash device.");
        break;

        case AT25xy011_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25DF011 or AT25DN011 or AT25XE011 (1-MBit) SPI flash device.");
        break;

        case MX25V1006E_DEV_INDEX:
            printf_string(UART, "\n\rYou are using MX25V1006E 1-Mbit SPI flash device.");
        break;

        case MX25R1035F_DEV_INDEX:
            printf_string(UART, "\n\rYou are using MX25R1035F 1-Mbit SPI flash device.");
        break;

        case MX25R2035F_DEV_INDEX:
            printf_string(UART, "\n\rYou are using MX25R2035F 2-Mbit SPI flash device.");
        break;

        case P25Q10U_DEV_INDEX:
            printf_string(UART, "\n\rYou are using P25Q10U 1-Mbit SPI flash device.");
        break;

        case GD25WD20_DEV_INDEX:
            printf_string(UART, "\n\rYou are using GD25WD20 2-Mbit SPI flash device.");
        break;

        case AT25xy021A_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25DF021A or AT25XE021A 2-Mbit SPI flash device.");
        break;

        case AT25EU0021A_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25EU0021A 2-Mbit SPI flash device.");
        break;

        case AT25XE041B_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25XE041B 4-Mbit SPI flash device.");
        break;

        case AT25FF041A_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25FF041A 4-Mbit SPI flash device.");
        break;

        case AT25SF041B_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25SF041B 4-Mbit SPI flash device.");
        break;

        case AT25XE041D_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25XE041D 4-Mbit SPI flash device.");
        break;

        case AT25XE081D_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25XE081D 8-Mbit SPI flash device.");
        break;

        case AT25xy512C_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25xy512C 512-Kbit SPI flash device.");
        break;

        case AT25EU0011A_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25EU0011A 1-Mbit SPI flash device.");
        break;

        case AT25FF081_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT25FF081 8-Mbit SPI flash device.");
        break;
#if defined (__FPGA__)
        case MX25R8035F_DEV_INDEX:
            printf_string(UART, "\n\rYou are using MX25R8035F 8-Mbit SPI flash device.");
        break;

        case AT45DB081E_DEV_INDEX:
            printf_string(UART, "\n\rYou are using AT45DB081E 8-Mbit SPI flash device.");
        break;
#endif // __FPGA__
        default:
            printf_string(UART, "\n\rNo SPI flash device detected");
        break;
    }

    if ((dev_id == W25x10CL_DEV_INDEX)    ||
        (dev_id == W25x20CL_DEV_INDEX)    ||
        (dev_id == P25Q10U_DEV_INDEX)     ||
        (dev_id == GD25WD20_DEV_INDEX)    ||
        (dev_id == AT25xy021A_DEV_INDEX)  ||
        (dev_id == AT25EU0021A_DEV_INDEX) ||
        (dev_id == AT25XE041B_DEV_INDEX)  ||
        (dev_id == AT25FF041A_DEV_INDEX)  ||
        (dev_id == AT25SF041B_DEV_INDEX)  ||
        (dev_id == AT25XE041D_DEV_INDEX)  ||
        (dev_id == AT25XE081D_DEV_INDEX)  ||
        (dev_id == MX25R8035F_DEV_INDEX)  ||
        (dev_id == AT45DB081E_DEV_INDEX)  ||
        (dev_id == AT25xy512C_DEV_INDEX)  ||
        (dev_id == AT25EU0011A_DEV_INDEX))
    {
        // Read SPI Flash Manufacturer/Device ID
        spi_flash_read_rems_id(&man_dev_id);
        printf_string(UART, "\n\r\n\rSPI flash Manufacturer/Device ID is ");
        printf_byte(UART, (man_dev_id >> 8) & 0xFF);
        printf_byte(UART, (man_dev_id) & 0xFF);

        spi_flash_read_unique_id(&unique_id);
        printf_string(UART, "\n\r\n\rSPI flash Unique ID Number is ");
        printf_byte(UART, ((unique_id >> 32) >> 24) & 0xFF);
        printf_byte(UART, ((unique_id >> 32) >>16) & 0xFF);
        printf_byte(UART, ((unique_id >> 32) >>8) & 0xFF);
        printf_byte(UART, (unique_id >> 32) & 0xFF);
        printf_byte(UART, (unique_id >> 24) & 0xFF);
        printf_byte(UART, (unique_id >> 16) & 0xFF);
        printf_byte(UART, (unique_id >> 8) & 0xFF);
        printf_byte(UART, (unique_id) & 0xFF);
    }

    if ((dev_id == MX25R1035F_DEV_INDEX) ||
        (dev_id == MX25R2035F_DEV_INDEX))
    {
        //---------------- MX25R Low Power Mode Tests -------------------------------

        spi_flash_set_power_mode(SPI_FLASH_PM_LPM);

        print_MX25R_power_mode();

        spi_read_write_test();
        spi_read_write_test_buffer();
#if defined (CFG_SPI_DMA_SUPPORT)
        spi_read_write_test_dma();
#endif

        //---------------- MX25R High Performance Mode Tests -------------------------------

        spi_flash_set_power_mode(SPI_FLASH_PM_HPM);

        print_MX25R_power_mode();

        spi_read_write_test();
        spi_read_write_test_buffer();
#if defined (CFG_SPI_DMA_SUPPORT)
        spi_read_write_test_dma();
#endif
    }
    else
    {
        spi_read_write_test();
        spi_read_write_test_buffer();
#if defined (CFG_SPI_DMA_SUPPORT)
        spi_read_write_test_dma();
#endif

        // SPI FLASH memory protection features
        spi_protection_features_test();
    }
    printf_string(UART, "\n\rEnd of test\n\r");
}
