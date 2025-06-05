#include "usart_blocking.h"
#include "system_config.h"
#include <xc.h>

void USART_Blocking_Initialize(unsigned int baud)
{
    U1MODE = 0;
    U1STA = 0;
    U1BRG = (SYS_CLK_FREQ / (16 * baud)) - 1;
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1MODEbits.ON = 1;
}

void USART_Blocking_WriteChar(char c)
{
    while (U1STAbits.UTXBF);
    U1TXREG = c;
    while (!U1STAbits.TRMT);
}

void USART_Blocking_WriteString(const char *str)
{
    while (*str)
    {
        USART_Blocking_WriteChar(*str++);
    }
}

char USART_Blocking_ReadChar(void)
{
    while (!U1STAbits.URXDA);
    return U1RXREG;
}
