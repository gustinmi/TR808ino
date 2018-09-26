/*
 * IncFile1.h
 *
 * Created: 12.4.2018 14:39:56
 *  Author: gustin
 */ 

#include <stdio.h>

void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
