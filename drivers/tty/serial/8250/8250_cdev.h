/* SPDX-License-Identifier: GPL-2.0 */

#ifndef UART_CDEV_H
#define UART_CDEV_H

#include <linux/types.h>

struct uart_8250_port;

int uart_cdev_init(void);
void uart_cdev_exit(void);
int uart_cdev_register(struct uart_8250_port *p);
void uart_cdev_unregister(struct uart_8250_port *p);

#endif /* UART_CDEV_H */