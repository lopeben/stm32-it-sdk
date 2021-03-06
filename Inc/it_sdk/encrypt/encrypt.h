/* ==========================================================
 * encrypt.h - Encryption headers
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 02 dec. 2018
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
 * 
 *
 * ==========================================================
 */

#ifndef IT_SDK_ENCRYPT_H_
#define IT_SDK_ENCRYPT_H_

#define ITSDK_ENCRYPT_MAX_FRAME_SIZE	12		// max size for a frame to be encrypted (related to buffer size)

void itsdk_encrypt_cifferKey(uint8_t * key, int len);
void itsdk_encrypt_unCifferKey(uint8_t * key, int len);

// uint64_t ciffer/unciffer function
#define itsdk_encrypt_cifferKey64(v) ( \
		(uint64_t)(v) ^ (  ((uint64_t)ITSDK_PROTECT_KEY  | ((uint64_t)ITSDK_PROTECT_KEY << 32) ) ) \
)

#define itsdk_encrypt_unCifferKey64(v) itsdk_encrypt_cifferKey64(v)


void itsdk_aes_crt_encrypt_128B(
		uint8_t	* clearData,			// Data to be encrypted
		uint8_t * encryptedData,		// Can be the same as clearData
		uint8_t   dataLen,				// Size of data to be encrypted
		uint32_t  deviceId,				// 32b device ID
		uint16_t  seqId,				// 16b sequenceId (incremented for each of the frame)
		uint8_t   nonce,				// 8b  value you can update dynamically from backend
		uint32_t  sharedKey,			// 24b hardcoded value (hidden with ITSDK_PROTECT_KEY)
		uint8_t * masterKey				// 128B key used for encryption (hidden with ITSDK_PROTECT_KEY)
);

void itsdk_speck_encrypt(
		uint8_t	* clearData,			// Data to be encrypted
		uint8_t * encryptedData,		// Can be the same as clearData
		uint8_t   dataLen,				// Size of data to be encrypted
		uint64_t  masterKey				// 64B key used for encryption (hidden with ITSDK_PROTECT_KEY)
);

#endif /* IT_SDK_ENCRYPT_H_ */
