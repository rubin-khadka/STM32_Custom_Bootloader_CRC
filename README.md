# STM32 Custom Bootloader with CRC Verification

[![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html)
[![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)](http://st.com/en/development-tools/stm32cubeide.html)
[![CRC32](https://img.shields.io/badge/CRC-32-darkgreen)](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)

## Project Overview

This project implements a custom bootloader for STM32 microcontrollers that performs CRC (Cyclic Redundancy Check) verification on the application firmware before execution. The bootloader ensures firmware integrity, preventing the system from running corrupted or incomplete applications.

This project is programmed without using any libraries - completely bare metal. This approach allows us to keep the bootloader size to only 8KB, which is essential for memory-constrained devices like the STM32F103C8T6 (64KB total flash).

The project follows this flow chart:

<img width="408" height="637" alt="project_flowchart" src="https://github.com/user-attachments/assets/4f4b6684-b40d-48e1-bc0c-1494eb9748ae" />

The application is a simplified version of the [Multi Sensor Cloud Logger Project](https://github.com/rubin-khadka/STM32_MicroSD_Cloud_Logger). It uses three sensors:
- DHT11 - Temperature & Humidity
- MPU6050 - Accelerometer & Gyroscope
- DS3231 - Real Time Clock

A 16x2 LCD display shows sensor values, and a push button cycles through different display modes.

## Video Demonstrations

### Hardware Demo

https://github.com/user-attachments/assets/48d1f5ff-b504-4e79-8dd5-a6804ab16511

When the device is powered on, the green LED on PA4 lights up to indicate that the bootloader has verified the CRC and is now jumping to the application.

The application is a simple sensor dashboard on a 16x2 LCD. Pressing the button cycles through different display modes:
- Temperature & Humidity
- Time & Date
- MPU6050 Accelerometer data (scaled)
- MPU6050 Gyroscope data (scaled)

### Hercules Uart Debug

https://github.com/user-attachments/assets/4feb2be0-2729-406f-ae84-f6187003e120

This demo shows the bootloader loading, performing the CRC check, and after successful verification, jumping to the application to start the program.

### Programming with STM32CubeProgrammer

https://github.com/user-attachments/assets/56057cd3-0352-40e7-9ee2-32e5088c5972

This demo shows how to flash both the bootloader and application using STM32CubeProgrammer.

- Using ELF files - The linker script already defines the memory locations for both bootloader (`0x08000000`) and application (`0x08002400`). When using ELF files, the programmer automatically places the code in the correct addresses.
- Using BIN files - When using raw binary files, you must manually specify the download address for each firmware:
    - Bootloader → 0x08000000
    - Application → 0x08002400

## Memory Layout

The STM32 flash memory is partitioned to separate the bootloader from the application firmware:

| Start Address | Size | Section | Description |
|---------------|------|---------|-------------|
| 0x08000000 | 8KB | Bootloader | Custom bootloader code |
| 0x08002000 | 1KB | Application Header	| Contains CRC and application metadata |
| 0x08002400 | 55KB | Application | Main application firmware |

```
Flash Memory Map (STM32F103C8 - 64KB total)
┌─────────────────────────────────────────────────────────────┐
│ 0x08000000                                                  │
│    ┌─────────────────────────────────────────────────────┐  │
│    │  Bootloader                     (8KB)               │  │
│    │  0x08000000 - 0x08001FFF                            │  │
│    └─────────────────────────────────────────────────────┘  │
│                                                             │
│ 0x08002000                                                  │
│    ┌─────────────────────────────────────────────────────┐  │
│    │  Application Header              (1KB)              │  │
│    │  0x08002000 - 0x080023FF                            │  │
│    └─────────────────────────────────────────────────────┘  │
│                                                             │
│ 0x08002400                                                  │
│    ┌─────────────────────────────────────────────────────┐  │
│    │  Application Firmware           (55KB)              │  │
│    │  0x08002400 - 0x0800FFFF                            │  │
│    └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```
- Bootloader starts at 0x08000000 (default vector table address)
- Application header at 0x08002000 stores CRC and metadata
- Application code starts at 0x08002400 with custom vector table remapping
- Bootloader verifies CRC from header before jumping to the application

## Pin Configuration

| Component	| STM32 Pin	| Notes |
|-----------|-----------|-------|
| LED | PA4 | Active-high indicator |
| Button | PA0 | Pull-down, active-high |
| DHT11 | PB0 | Single-wire data |
| LCD (I2C) | PB10 (SCL), PB11 (SDA) | With PCF8574 backpack |
| MPU6050 | PB10 (SCL), PB11 (SDA) | I2C address: 0x69 |
| DS3231 | PB10 (SCL), PB11 (SDA) | I2C address: 0x68 |
| USART1 (Debug) | PA9 (TX), PA10 (RX) | 115200 baud, 8N1 |

> **Note:** Multiple I2C devices (LCD, MPU6050, DS3231) share the same I2C bus (PB10/PB11) with different addresses.

| Component | STM32 Pin |
|-----------|-----------|
| LED | PA4 |
| Button | PA0 |
| DHT11 | PB0 |
| LCD (I2C) | PB10 (SCL), PB11 (SDA) |
| MPU6050 | PB10, PB11 | 
| DS3231 | PB10, PB11 |
| USART1 (debug) | PA9 (TX), PA10(RX) |

## CRC Calculation & Verification
The bootloader uses a 32-bit CRC (Cyclic Redundancy Check) to ensure the integrity of the application firmware before executing it.

### How CRC is Calculated
The CRC algorithm processes the application firmware byte by byte, performing a series of bitwise operations to generate a unique 32-bit checksum. The algorithm works as follows:
1. Initialization - The CRC value starts at `0xFFFFFFFF`
2. Byte Processing - Each byte of the application firmware is XORed with the current CRC value
3. Bit Shifting - For each bit in the byte (8 bits total), the algorithm checks the least significant bit:
    - If the bit is 1 → The CRC is shifted right and XORed with a fixed polynomial (`0xEDB88320`)
    - If the bit is 0 → The CRC is simply shifted right
4. Finalization - After all bytes are processed, the result is XORed with `0xFFFFFFFF` to produce the final CRC32 value

This polynomial (`0xEDB88320`) is the standard CRC-32 used in protocols like Ethernet, PNG, and ZIP files.

### Verification Process
1. During Application Build - The CRC of the entire application firmware is calculated and stored in the Application Header at address `0x08002000`
2. During Boot - When the device powers on, the bootloader:
    - Reads the stored CRC from the Application Header
    - Calculates a fresh CRC of the application firmware in flash memory
    - Compares the two values
3. Result:
    - CRC Match → Firmware is valid → Green LED lights up → Jump to application
    - CRC Mismatch → Firmware is corrupted or missing → Stay in bootloader (safe mode)

## Application Integration
To work with the bootloader, the application firmware requires two key modifications: linker script changes and vector table relocation.

### Linker Script Modifications
The application's linker script must be configured to start at `0x08002400` instead of the default `0x08000000`:
- FLASH origin - Changed from `0x08000000` to `0x08002400`
- FLASH length - Reduced to 55KB to fit within the allocated space

### Vector Table Relocation
The interrupt vector table (containing addresses for reset, exceptions, and IRQ handlers) must be moved from its default location at `0x08000000` to the new application start address `0x08002400`.

By default, the STM32 expects the vector table at the beginning of flash (`0x08000000`). Since the bootloader now occupies that space, the application needs its own vector table at its new location. Without relocation, interrupts would jump to bootloader handlers instead of application handlers

Early in the application's startup code, the Vector Table Offset Register (VTOR) is updated to point to `0x08002400`. This tells the CPU to look for interrupt handlers at the application's relocated vector table

```c
SCB->VTOR = 0x08002400;  // Set vector table offset to application start
```
This single line at the beginning of `main()` ensures all interrupts work correctly for the application.

## Related Projects 
- [STM32_MicroSD_Cloud_Logger](https://github.com/rubin-khadka/STM32_MicroSD_Cloud_Logger)

## Resources
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DHT11 Sensor Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
- [MPU6050 Sensor Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
- [RTC DS3231 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf)
- [PCF8574 I2C Backpack Datasheet](https://www.ti.com/lit/ds/symlink/pcf8574.pdf)

## Project Status
- **Status**: Complete
- **Version**: v1.0
- **Last Updated**: April 2026

## Contact
**Rubin Khadka Chhetri**  
📧 rubinkhadka84@gmail.com <br>
🐙 GitHub: https://github.com/rubin-khadka
