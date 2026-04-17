# STM32_Custom_Bootloader_CRC

<img width="408" height="637" alt="project_flowchart" src="https://github.com/user-attachments/assets/4f4b6684-b40d-48e1-bc0c-1494eb9748ae" />

## Video Demonstrations

### Hardware Demo

https://github.com/user-attachments/assets/4feb2be0-2729-406f-ae84-f6187003e120

When the device is powered, the green light on PA4 lighnting indicate that the bootloader have verified the crc and now jumping to the applicaiton.

Application is simple sensor dashboard with lcd 16x2 where pressing button displays different modes: temperature/ humidity, time and data and mpu 6050 accelerometer and gyroscope data (scaled).

### Hercules Uart Debug

https://github.com/user-attachments/assets/48d1f5ff-b504-4e79-8dd5-a6804ab16511

Shows the boot loader is loaded and check crc. after verification jumps into application and start the program.
