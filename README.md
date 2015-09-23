# nRF24L01P-LightWeight
Smaller library for the nRF24L01P with minimal features. This does not support auto auto acknowledgement or multiple pipes.
# Getting Started#
Using this sample code will require two Arduinos and two nrf24l01+ radios. 
To quickly deploy and test code, download the files. Simply change ```#define transmitState``` to ```true``` or ```false``` to set the radio to either transmitting or receiving.

Possible uses include using the transmitting node as a remote sensor. Configure the transmitting node read a sensor and transmit over radio.

### Hardware configuration###

| Arduino pin      | NRF24L01+ pin |
|------------------|---------------|
| 13               | SCK           |  
| 12               | MISO          | 
| 11               | MOSI          |  
| 10               | CSN           | 
| 9                | CE            |  
| 3.3v             | VCC           |   
| GND              | GND           |  
| NA           | IRQ           |