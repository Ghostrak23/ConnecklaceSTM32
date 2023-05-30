# **BLE CUSTOM EXAMPLE with STM32WB55**

## **Description**
This project contains a firmware which implements a BLE example using the latest ST example and ST librairies.

The nucleo board runs as BLE peripheral. It advertising with the name "Labo_IoT".
There are 2 services :
- First service
- Echo service

The first service contains 2 characteristics :
- Led characteristic in R/W
- SW characteristic in Notify

The second service contains 1 characteristic :
- Echo characteristic in Notify.

## **How it works ?**
You have to establish a connection from a central BLE to the peripheral. Once done, you can write from your central in Led characteristic :
- 0x01 to set LED1
- 0x02 to set LED2
- 0x03 to set LED3

Then, enable Notification on the 2 others characteristics.

If you push SW3, a counter is incremented in SW characteristic.

If you push SW1, counter is copied on echo characteristic.

## **Source code**
You can search "+BCH" tag to see where application code was added to implement this solution.

## **Built With**
STM32CubeIDE v1.11.0

## **Hardware**
It based on P-NUCLEO-WB55