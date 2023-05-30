/*
 * logger.c
 *
 *  Created on: Nov 7, 2022
 *      Author: Valentin
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "logger.h"
#include "main.h"
#include "iks01a3_env_sensors.h"

#define LOG_UART_TIMEOUT 100

UART_HandleTypeDef *log_huart;

static char log_buffer[LOG_BUFFER_SIZE];

void log_init(void *huart)
{
	log_huart = (UART_HandleTypeDef *)huart;
}

void log_transmit(uint8_t *data, uint16_t data_len)
{
	HAL_UART_Transmit(log_huart, data, data_len, LOG_UART_TIMEOUT);
}

void log_printf(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(log_buffer, LOG_BUFFER_SIZE, format, argptr);
	va_end(argptr);
	log_transmit((uint8_t*)log_buffer, strlen(log_buffer));
}














