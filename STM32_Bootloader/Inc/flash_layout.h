/*
 * flash_layout.h
 *
 *  Created on: Apr 16, 2026
 *      Author: Rubin Khadka
 */

#ifndef INC_FLASH_LAYOUT_H_
#define INC_FLASH_LAYOUT_H_

/* Bootloader: 8KB */
#define BL_START_ADDR         0x08000000
#define BL_SIZE               (8 * 1024)

/* Application Header: 1KB */
#define APP_HEADER_ADDR       0x08002000
#define APP_HEADER_SIZE       (1 * 1024)

/* Application: 55KB */
#define APP_START_ADDR        0x08002400
#define APP_MAX_SIZE          (55 * 1024)

#endif /* INC_FLASH_LAYOUT_H_ */
