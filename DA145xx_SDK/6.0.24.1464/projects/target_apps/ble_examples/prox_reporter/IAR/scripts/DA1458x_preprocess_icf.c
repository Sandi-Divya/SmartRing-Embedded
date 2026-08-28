#include "da14585_config_basic.h"
#include "da14585_config_advanced.h"
#include "da1458x_scatter_config.h"

#if !defined(CFG_RET_DATA_SIZE)
    #error "CFG_RET_DATA_SIZE is not defined!"
#endif

#if !defined(CFG_RET_DATA_UNINIT_SIZE)
    #error "CFG_RET_DATA_UNINIT_SIZE is not defined!"
#endif

/********************************************************************************************
 * Memory area where retained data will be stored.
 ********************************************************************************************/
#define RET_MEM_SIZE        (CFG_RET_DATA_UNINIT_SIZE + CFG_RET_DATA_SIZE + RET_HEAP_SIZE)
/* Retained data base address */
#define RET_MEM_BASE_ADDR    ALIGN4_HI(__SCT_BLE_BASE - RET_MEM_SIZE)

/********************************************************************************************
 * Free area resides between the Exchange memory and ROM data.
 *******************************************************************************************/
/* Free area base address */
#define FREE_AREA_BASE_ADDR     ALIGN4_HI(__SCT_BLE_BASE + __SCT_EM_BLE_END)
/* Free area size */
#define FREE_AREA_SIZE          (ROM_DATA_BASE_ADDR - FREE_AREA_BASE_ADDR)

#if defined(CFG_CODE_LOCATION_OTP) && defined(CFG_CODE_LOCATION_EXT)
    #error "Only one of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#elif defined(CFG_CODE_LOCATION_OTP)
    #define CODE_LOCATION_OTP   1
    #define CODE_LOCATION_EXT   0
#elif defined(CFG_CODE_LOCATION_EXT)
    #define CODE_LOCATION_OTP   0
    #define CODE_LOCATION_EXT   1
#else
    #error "One of CFG_CODE_LOCATION_OTP and CFG_CODE_LOCATION_EXT must be defined!"
#endif

#if defined (CFG_TRNG)
    #define TRNG_BUFFER_AREA_SZ CFG_TRNG
#else
    #define TRNG_BUFFER_AREA_SZ 0
#endif

/* OTP memory size = 64K*/
#define OTP_MEM_SIZE            (64 * 1024)

/* OTP header section size = 64 * 64-bit words = 512bytes*/
#define OTP_HEADER_SIZE         ((64 * 64) / 8)

/* Useful OTP memory size:
 1. The 512 bytes of the OTP header start at 63.5K offset in OTP
    memory.
 2. There is a limitation in the location of the TRNG buffer (max size is 1K).
    The TRNG buffer must be always placed before the 64K limit in RAM. */
#define OTP_MEM_USEFUL_SIZE     (OTP_MEM_SIZE - OTP_HEADER_SIZE - TRNG_BUFFER_AREA_SZ)

#if CODE_LOCATION_OTP
    /* Base address of code (RAM base address + interrupt vector table size + patch table size) */
    #define CODE_AREA_BASE      (0x07fc0000 + 0xC0 + 80)

    /* Max needs in RAM per application - excluding the retained data, the interrupt vector table and the patch table */
    #define CODE_AREA_MAX_SIZE  (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

    /* Useful memory area for OTP code */
    #define CODE_AREA_SIZE      (OTP_MEM_USEFUL_SIZE - (0xC0 + 80))
#elif CODE_LOCATION_EXT
    /* Base address of code (RAM base address + interrupt vector table size + patch table size + TRNG buffer) */
    #define CODE_AREA_BASE      ALIGN4_HI(0x07fc0000 + 0xC0 + 80 + TRNG_BUFFER_AREA_SZ)

    /* Max needs in RAM per application - excluding the retained data, the interrupt vector table, the patch table and the TRNG buffer */
    #define CODE_AREA_MAX_SIZE  (RET_MEM_BASE_ADDR - CODE_AREA_BASE)

    /* Same as max size */
    #define CODE_AREA_SIZE      CODE_AREA_MAX_SIZE
#endif


/* uVision supports AND as an operator,
 * IAR does not, it supports the bitwise AND "&"
 */
#define AND &

int code_area_base      = CODE_AREA_BASE;
int code_area_size      = CODE_AREA_SIZE;
int code_location_otp   = CODE_LOCATION_OTP;
int code_location_ext   = CODE_LOCATION_EXT;
int ret_mem_base_addr   = RET_MEM_BASE_ADDR;
int cfg_ret_data_uninit = CFG_RET_DATA_UNINIT_SIZE;
int cfg_ret_data_size   = CFG_RET_DATA_SIZE;
int ret_heap_size       = RET_HEAP_SIZE;
int free_area_base_addr = FREE_AREA_BASE_ADDR;
int ret_mem_size        = RET_MEM_SIZE;
int rom_data_base_addr  = ROM_DATA_BASE_ADDR;
int free_area_size      = FREE_AREA_SIZE;
int trng_buffer_area_sz = TRNG_BUFFER_AREA_SZ;
int cfg_max_tx_packet_length = CFG_MAX_TX_PACKET_LENGTH;

#if (CFG_MAX_TX_PACKET_LENGTH > 27)
int free_area_at_tx_cntl_buffer_base_addr  = FREE_AREA_AT_TX_CNTL_BUFFER_BASE_ADDR;
int free_area_at_tx_adv_1_buffer_base_addr = FREE_AREA_AT_TX_ADV_1_BUFFER_BASE_ADDR;
int free_area_at_tx_adv_2_buffer_base_addr = FREE_AREA_AT_TX_ADV_2_BUFFER_BASE_ADDR;
int free_area_at_tx_adv_3_buffer_base_addr = FREE_AREA_AT_TX_ADV_3_BUFFER_BASE_ADDR;
int free_area_at_tx_cntl_buffer_size   = FREE_AREA_AT_TX_CNTL_BUFFER_SIZE;
int free_area_at_tx_adv_1_buffer_size  = FREE_AREA_AT_TX_ADV_1_BUFFER_SIZE;
int free_area_at_tx_adv_2_buffer_size  = FREE_AREA_AT_TX_ADV_2_BUFFER_SIZE;
int free_area_at_tx_adv_3_buffer_size  = FREE_AREA_AT_TX_ADV_3_BUFFER_SIZE;
#endif
