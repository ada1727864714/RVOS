#include <stddef.h>
#include "../include/types.h"

/* clang-format off */
/* Register address offsets */
#define UARTHS_REG_TXFIFO (0x00)
#define UARTHS_REG_RXFIFO (0x04)
#define UARTHS_REG_TXCTRL (0x08)
#define UARTHS_REG_RXCTRL (0x0c)
#define UARTHS_REG_IE     (0x10)
#define UARTHS_REG_IP     (0x14)
#define UARTHS_REG_DIV    (0x18)

/* TXCTRL register */
#define UARTHS_TXEN       (0x01)
#define UARTHS_TXWM(x)    (((x) & 0xffff) << 16)

/* RXCTRL register */
#define UARTHS_RXEN       (0x01)
#define UARTHS_RXWM(x)    (((x) & 0xffff) << 16)

/* IP register */
#define UARTHS_IP_TXWM    (0x01)
#define UARTHS_IP_RXWM    (0x02)
/* clang-format on */

typedef struct _uarths_txdata
{
    /* Bits [7:0] is data */
    uint32_t data : 8;
    /* Bits [30:8] is 0 */
    uint32_t zero : 23;
    /* Bit 31 is full status */
    uint32_t full : 1;
} __attribute__((packed, aligned(4))) uarths_txdata_t;

typedef struct _uarths_rxdata
{
    /* Bits [7:0] is data */
    uint32_t data : 8;
    /* Bits [30:8] is 0 */
    uint32_t zero : 23;
    /* Bit 31 is empty status */
    uint32_t empty : 1;
} __attribute__((packed, aligned(4))) uarths_rxdata_t;

typedef struct _uarths_txctrl
{
    /* Bit 0 is txen, controls whether the Tx channel is active. */
    uint32_t txen : 1;
    /* Bit 1 is nstop, 0 for one stop bit and 1 for two stop bits */
    uint32_t nstop : 1;
    /* Bits [15:2] is reserved */
    uint32_t resv0 : 14;
    /* Bits [18:16] is threshold of interrupt triggers */
    uint32_t txcnt : 3;
    /* Bits [31:19] is reserved */
    uint32_t resv1 : 13;
} __attribute__((packed, aligned(4))) uarths_txctrl_t;

typedef struct _uarths_rxctrl
{
    /* Bit 0 is txen, controls whether the Tx channel is active. */
    uint32_t rxen : 1;
    /* Bits [15:1] is reserved */
    uint32_t resv0 : 15;
    /* Bits [18:16] is threshold of interrupt triggers */
    uint32_t rxcnt : 3;
    /* Bits [31:19] is reserved */
    uint32_t resv1 : 13;
} __attribute__((packed, aligned(4))) uarths_rxctrl_t;

typedef struct _uarths_ip
{
    /* Bit 0 is txwm, raised less than txcnt */
    uint32_t txwm : 1;
    /* Bit 1 is txwm, raised greater than rxcnt */
    uint32_t rxwm : 1;
    /* Bits [31:2] is 0 */
    uint32_t zero : 30;
} __attribute__((packed, aligned(4))) uarths_ip_t;

typedef struct _uarths_ie
{
    /* Bit 0 is txwm, raised less than txcnt */
    uint32_t txwm : 1;
    /* Bit 1 is txwm, raised greater than rxcnt */
    uint32_t rxwm : 1;
    /* Bits [31:2] is 0 */
    uint32_t zero : 30;
} __attribute__((packed, aligned(4))) uarths_ie_t;

typedef struct _uarths_div
{
    /* Bits [31:2] is baud rate divisor register */
    uint32_t div : 16;
    /* Bits [31:16] is 0 */
    uint32_t zero : 16;
} __attribute__((packed, aligned(4))) uarths_div_t;

typedef struct _uarths
{
    /* Address offset 0x00 */
    uarths_txdata_t txdata;
    /* Address offset 0x04 */
    uarths_rxdata_t rxdata;
    /* Address offset 0x08 */
    uarths_txctrl_t txctrl;
    /* Address offset 0x0c */
    uarths_rxctrl_t rxctrl;
    /* Address offset 0x10 */
    uarths_ie_t ie;
    /* Address offset 0x14 */
    uarths_ip_t ip;
    /* Address offset 0x18 */
    uarths_div_t div;
} __attribute__((packed, aligned(4))) uarths_t;

typedef enum _uarths_interrupt_mode
{
    UARTHS_SEND = 1,
    UARTHS_RECEIVE = 2,
    UARTHS_SEND_RECEIVE = 3,
} uarths_interrupt_mode_t;

typedef enum _uarths_stopbit
{
    UARTHS_STOP_1,
    UARTHS_STOP_2
} uarths_stopbit_t;