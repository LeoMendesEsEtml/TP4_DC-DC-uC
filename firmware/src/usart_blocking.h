#ifndef USART_BLOCKING_H
#define USART_BLOCKING_H

#include <stdint.h>

void USART_Blocking_Initialize(unsigned int baud);
void USART_Blocking_WriteChar(char c);
void USART_Blocking_WriteString(const char *str);
char USART_Blocking_ReadChar(void);

#endif // USART_BLOCKING_H
