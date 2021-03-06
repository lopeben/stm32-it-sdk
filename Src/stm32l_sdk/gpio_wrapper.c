/* ==========================================================
 * uart_wrapper.c - wrapper function for uarts
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 12 sept. 2018
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2018 Disk91
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU LESSER General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------
 * Serial1 is LPUART
 * Serial2 is USART2
 * Debuh is non affected
 *
 * ==========================================================
 */
#include <string.h>
#include <stdbool.h>
#include <it_sdk/config.h>
#if ITSDK_PLATFORM == __PLATFORM_STM32L0x1 || ITSDK_PLATFORM == __PLATFORM_STM32L0x3

#include <it_sdk/itsdk.h>
#include <it_sdk/wrappers.h>
#include "stm32l0xx_hal.h"


/**
 * Convert the bankId used as a generic Id to the right GPIO structure
 */
GPIO_TypeDef * getPortFromBankId(uint8_t bankId) {
	switch ( bankId ) {
	case __BANK_A: return GPIOA;
	case __BANK_B: return GPIOB;
	case __BANK_C: return GPIOC;
	case __BANK_D: return GPIOD;
	case __BANK_H: return GPIOH;
	default:
		itsdk_error_handler(__FILE__, __LINE__);
	}
	return NULL;
}


/**
 * Convert a GPIO bank/pin into the corresponding ExtI line
 */
IRQn_Type getIrqFromBankPin(uint8_t bankId, uint16_t id) {

	if ( id <= 1 ) {
		return EXTI0_1_IRQn;
	} else if ( id <= 3 ) {
		return EXTI2_3_IRQn;
	} else {
		return EXTI4_15_IRQn;
	}
}




void gpio_configure(uint8_t bank, uint16_t id, itsdk_gpio_type_t type ) {
	GPIO_InitTypeDef GPIO_InitStruct;

	switch ( bank ) {
	case __BANK_A:
		  __GPIOA_CLK_ENABLE();
		  break;
	case __BANK_B:
		  __GPIOB_CLK_ENABLE();
		  break;
	case __BANK_C:
		  __GPIOC_CLK_ENABLE();
		  break;
	case __BANK_D:
		  __GPIOD_CLK_ENABLE();
		  break;
	case __BANK_H:
		  __GPIOH_CLK_ENABLE();
		  break;
	}

	GPIO_InitStruct.Pin = id;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	switch (type) {

	case GPIO_OUTPUT_PP:
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_OUTPUT_PULLUP:
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		break;

	case GPIO_OUTPUT_PULLDOWN:
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		break;

	case GPIO_OUTPUT_OD:
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_INPUT:
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_INPUT_PULLUP:
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		break;

	case GPIO_INPUT_PULLDOWN:
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		break;

	case GPIO_INTERRUPT_RISING:
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_INTERRUPT_RISING_PULLDWN:
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		break;

	case GPIO_INTERRUPT_FALLING:
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_INTERRUPT_FALLING_PULLUP:
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
		break;

	case GPIO_INTERRUPT_ANY:
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_ANALOG:
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;

	case GPIO_OFF:
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;
	}

	HAL_GPIO_Init(getPortFromBankId(bank), &GPIO_InitStruct);

}

void gpio_set(uint8_t bank, uint16_t id) {
	HAL_GPIO_WritePin(getPortFromBankId(bank), id,GPIO_PIN_SET);
}

void gpio_reset(uint8_t bank, uint16_t id) {
	HAL_GPIO_WritePin(getPortFromBankId(bank), id,GPIO_PIN_RESET);
}

void gpio_change(uint8_t bank, uint16_t id, uint8_t val) {
	HAL_GPIO_WritePin(getPortFromBankId(bank), id, val);
}

void gpio_toggle(uint8_t bank, uint16_t id) {
	HAL_GPIO_TogglePin(getPortFromBankId(bank), id);
}

uint8_t gpio_read(uint8_t bank, uint16_t id) {
	return HAL_GPIO_ReadPin(getPortFromBankId(bank), id);
}


void gpio_interruptEnable(uint8_t bank, uint16_t id) {
	HAL_NVIC_EnableIRQ(getIrqFromBankPin(bank,id));
}

void gpio_interruptDisable(uint8_t bank, uint16_t id) {
	HAL_NVIC_DisableIRQ(getIrqFromBankPin(bank,id));
}

void gpio_interruptPriority(uint8_t bank, uint16_t id, uint8_t nPreemption, uint8_t nSubpriority) {
	HAL_NVIC_SetPriority(getIrqFromBankPin(bank,id), nPreemption, nSubpriority);
}

void gpio_interruptClear(uint8_t bank, uint16_t id) {
	__HAL_GPIO_EXTI_CLEAR_IT(id);
}


/**
 * RCT Interrupt handler allowing to chain different function
 */
gpio_irq_chain_t __gpio_irq_chain = { NULL, NULL };
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	gpio_irq_chain_t * c = &__gpio_irq_chain;
	while ( c != NULL ) {
		void (*p)(uint16_t p) = c->irq_func;
		if ( p != NULL ) {
			p(GPIO_Pin);
		}
		c = c->next;
	}
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}

/**
 * Add an action to the chain, the action **must be** static
 */
void gpio_registerIrqAction(gpio_irq_chain_t * chain) {
	gpio_irq_chain_t * c = &__gpio_irq_chain;
	while ( c->next != NULL && c->irq_func != chain->irq_func ) {
	  c = c->next;
	}
	if ( c->irq_func != chain->irq_func ) {
		// the Action is not already existing
		c->next=chain;
		chain->next = NULL;
	}
}

/**
 * Remove an action to the chain, the action **must be** static
 */
void gpio_removeIrqAction(gpio_irq_chain_t * chain) {
	gpio_irq_chain_t * c = &__gpio_irq_chain;
	while ( c != NULL && c->next != chain ) {
	  c = c->next;
	}
	if ( c != NULL ) {
		c->next = c->next->next;
	}
}

/**
 * Search for an existing action
 */
bool gpio_existAction(gpio_irq_chain_t * chain) {
	gpio_irq_chain_t * c = &__gpio_irq_chain;
	while ( c != NULL && c->next != chain ) {
	  c = c->next;
	}
	if ( c != NULL ) {
		return true;
	}
	return false;
}


#endif
