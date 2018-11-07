/* ==========================================================
 * s2lp_spi.c - S2LP (STm SubGhz transceiver) driver for sigfox
 *              SPI interface
 * Project : IngeniousThings SDK
 * ----------------------------------------------------------
 * Created on: 04 nov. 2018
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2018  IngeniousThings and Disk91
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
 *
 * Some peaces of that code directly comes from ST Libraries
 * and identified with << COPYRIGHT(c) 2018 STMicroelectronics >>
 *
 * ==========================================================
 */

#include <it_sdk/config.h>
#if ITSDK_SIGFOX_LIB == __SIGFOX_S2LP

#include <it_sdk/itsdk.h>
#include <it_sdk/sigfox/sigfox.h>
#include <it_sdk/wrappers.h>

#if ITSDK_PLATFORM == __PLATFORM_STM32L0x3 || ITSDK_PLATFORM == __PLATFORM_STM32L0x1
#include <stm32l_sdk/spi/spi.h>
#else
#error NOT A SUPPORTED PLATFORM FOR S2LP
#endif

#include <drivers/s2lp/s2lp_spi.h>


/**
 * SPI Chip select set to low/High
 */
void s2lp_spi_setCsLow() {
	gpio_reset(ITSDK_S2LP_CS_BANK,ITSDK_S2LP_CS_PIN);
}

void s2lp_spi_setCsHigh() {
	gpio_set(ITSDK_S2LP_CS_BANK,ITSDK_S2LP_CS_PIN);
}

/**
 * Access the S2LP registers
 */
S2LP_SPI_StatusBytes s2lp_spi_accessRegisters(
		__SPI_HANDLER_TYPE * spi,
		uint8_t  cRegAddress,
        uint8_t  cNbBytes,
        uint8_t* pcBuffer,
		S2LP_ACCESS	 access					// true for read, false for
) {
  uint8_t v=0;
  uint8_t r=0;
  uint16_t status = 0x0000;

  // When accessing the SPI interface, the two status bytes of the MC_STATE
  // registers are sent to the MISO pin.
  S2LP_SPI_StatusBytes *pStatus=(S2LP_SPI_StatusBytes *)&status;

  // Disable S2LP Interrupt
  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
  gpio_interruptDisable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

  s2lp_spi_setCsLow();
  itsdk_delayMs(S2LP_SPI_DELAY);

  // Write the aHeader bytes and read the S2-LP status bytes
  v = (access==S2LP_READ_REGISTER)?READ_HEADER:WRITE_HEADER;
  spi_readRegister(
		  spi,
          (uint8_t *)&v,
          (uint8_t *)&(r),
		  1
  );
  status = r << 8;

  // Write the aHeader bytes and read the S2-LP status bytes
  v = cRegAddress;
  spi_readRegister(
		  spi,
          (uint8_t *)&v,
          (uint8_t *)&(r),
		  1
  );
  status+=r;

  // Writes the registers according to the number of bytes
  v=0xFF;
  for (int index = 0; index < cNbBytes; index++) {
	  if ( access==S2LP_READ_REGISTER ) {
		  spi_readRegister(
		 	  spi,
		 	  (uint8_t *)&v,
		 	  (uint8_t *)&(pcBuffer)[index],
		 	  1
		  );
	  } else {
		  spi_write_byte(spi,pcBuffer[index]);
	  }
  }

  // To be sure to don't rise the Chip Select before the end of last sending
  spi_wait4TransactionEnd(spi);

  // Puts the SPI chip select high to end the transaction
  s2lp_spi_setCsHigh();

  // Re-enable S2LP Interrupt
  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
  gpio_interruptEnable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

  return *pStatus;

}


// Largely inspired by STMicroelectronics source code << COPYRIGHT(c) 2018 STMicroelectronics >>




/**
 * Write a S2LP register and get results
 */

S2LP_SPI_StatusBytes s2lp_spi_writeRegisters(
		__SPI_HANDLER_TYPE * spi,
		uint8_t cRegAddress,
        uint8_t cNbBytes,
        uint8_t* pcBuffer
) {
	return s2lp_spi_accessRegisters(spi,cRegAddress,cNbBytes,pcBuffer,S2LP_WRITE_REGISTER);
	/*
  uint8_t v=0;
  uint8_t r=0;
  uint16_t status = 0x0000;
  S2LP_SPI_StatusBytes *pStatus=(S2LP_SPI_StatusBytes *)&status;

  // Disable S2LP Interrupt
  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
  gpio_interruptDisable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

  s2lp_spi_setCsLow();
  itsdk_delayMs(S2LP_SPI_DELAY);

  // Write the aHeader bytes and read the S2-LP status bytes
  v = WRITE_HEADER;
  spi_readRegister(
		  spi,
          (uint8_t *)&v,
          (uint8_t *)&(r),
		  1
  );
  status = r << 8;

  // Write the aHeader bytes and read the S2-LP status bytes
  v = cRegAddress;
  spi_readRegister(
		  spi,
          (uint8_t *)&v,
          (uint8_t *)&(r),
		  1
  );
  status+=r;

  // Writes the registers according to the number of bytes
  for (int index = 0; index < cNbBytes; index++) {
	  spi_write_byte(spi,pcBuffer[index]);
  }

  // To be sure to don't rise the Chip Select before the end of last sending
  spi_wait4TransactionEnd(spi);

  // Puts the SPI chip select high to end the transaction
  s2lp_spi_setCsHigh();

  // Re-enable S2LP Interrupt
  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
  gpio_interruptEnable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

  return *pStatus;
  */

}

S2LP_SPI_StatusBytes s2lp_spi_readRegisters(
		__SPI_HANDLER_TYPE * spi,
		uint8_t cRegAddress,
        uint8_t cNbBytes,
        uint8_t* pcBuffer
) {
	return s2lp_spi_accessRegisters(spi,cRegAddress,cNbBytes,pcBuffer,S2LP_READ_REGISTER);
	/*
	  uint8_t v=0;
	  uint8_t r=0;
	  uint16_t status = 0x0000;
	  S2LP_SPI_StatusBytes *pStatus=(S2LP_SPI_StatusBytes *)&status;

	  // Disable S2LP Interrupt
	  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
	  gpio_interruptDisable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

	  s2lp_spi_setCsLow();
	  itsdk_delayMs(S2LP_SPI_DELAY);

	  // Write the aHeader bytes and read the S2-LP status bytes
	  v = READ_HEADER;
	  spi_readRegister(
			  spi,
	          (uint8_t *)&v,
	          (uint8_t *)&(r),
			  1
	  );
	  status = r << 8;

	  // Write the aHeader bytes and read the S2-LP status bytes
	  v = cRegAddress;
	  spi_readRegister(
			  spi,
	          (uint8_t *)&v,
	          (uint8_t *)&(r),
			  1
	  );
	  status+=r;

	  // Read the registers according to the number of bytes
	  uint8_t dummy = 0xFF;
	  for (int index = 0; index < cNbBytes; index++) {
		  spi_readRegister(
				  spi,
				  (uint8_t *)&dummy,
				  (uint8_t *)&(pcBuffer)[index],
				  1
		  );
	  }

	  // To be sure to don't rise the Chip Select before the end of last sending
	  spi_wait4TransactionEnd(spi);

	  // Puts the SPI chip select high to end the transaction
	  s2lp_spi_setCsHigh();

	  // Re-enable S2LP Interrupt
	  gpio_interruptPriority(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN, 4, 4);
	  gpio_interruptEnable(ITSDK_S2LP_INT_BANK, ITSDK_S2LP_INT_PIN);

	  return *pStatus;
	  */

}

// End of >> COPYRIGHT(c) 2018 STMicroelectronics <<






#endif // ITSDK_SIGFOX_LIB test
