
/* The MIT License (MIT)
*
* Copyright (c) 2015 Chris Woodle cwoodle2014@my.fit.edu, Florida Institute of Technology
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <SPI.h>
#include "nRF24L01.h"

// Bit value
#define _BV(x) (1<<(x))

#define CE_PIN   9
#define CSN_PIN 10
#define transmitState true

uint64_t pipe = 0xF0F0F0F0AALL;

const uint8_t max_payload_size = 32;

class Radio
{
private:
	uint8_t ce_pin; // Chip enable sets RX or TX 
	uint8_t csn_pin; // SPI chip select 
	uint8_t payload_size; // in Bytes
protected:
	// Chip select pin
	void csn(int mode) {
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
		SPI.setClockDivider(SPI_CLOCK_DIV4);
		//CSN pin low to alert the 24L01 that it is about to receive SPI data
		digitalWrite(csn_pin, mode);
	}
	// Set chip enable, LOW for standby	
	void ce(int level)
	{
		digitalWrite(ce_pin, level);
	}
	uint8_t readRegister(uint8_t reg, uint8_t* buffer, uint8_t numberOfBytes)
	{
		// Value of the satus register
		uint8_t status;

		csn(LOW);
		status = SPI.transfer(R_REGISTER | (REGISTER_MASK & reg));
		while (numberOfBytes--) {
			*buffer++ = SPI.transfer(0xff);
		}
		csn(HIGH);

		return status;
	}
	// Returns one byte from the desired register
	uint8_t readRegisterByte(uint8_t reg)
	{
		csn(LOW);
		SPI.transfer(R_REGISTER | (REGISTER_MASK & reg));
		uint8_t result = SPI.transfer(0xff);
		csn(HIGH);
		return result;
	}
	uint8_t writeRegister(uint8_t reg, const uint8_t* buffer, uint8_t numberOfBytes)
	{
		// Value of the satus register
		uint8_t status;

		csn(LOW);
		status = SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
		while (numberOfBytes--) {
			SPI.transfer(*buffer++);
		}
		csn(HIGH);

		return status;
	}
	// Writes one byte to the desired register
	uint8_t writeRegisterByte(uint8_t reg, uint8_t value)
	{
		uint8_t status;
		csn(LOW);
		status = SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
		SPI.transfer(value);
		csn(HIGH);
		return status;
	}
	// Flush TX_FIFO, used in RX mode
	uint8_t flushRXBuffer()
	{
		uint8_t status;

		csn(LOW);
		status = SPI.transfer(FLUSH_RX);
		csn(HIGH);

		return status;
	}
	// Flush RX_FIFO, used in TX mode
	uint8_t flushTXBuffer()
	{
		uint8_t status;
		csn(LOW);
		status = SPI.transfer(FLUSH_TX);
		csn(HIGH);
		return status;
	}
public:
	Radio(uint8_t _cepin, uint8_t _cspin) {
		ce_pin = _cepin;
		csn_pin = _cspin;
		payload_size = 32;
	}
	void initialize() {
		// Initialize pins
		pinMode(ce_pin, OUTPUT);
		pinMode(csn_pin, OUTPUT);

		ce(LOW);
		csn(HIGH);

		// Initialize SPI bus
		SPI.begin();

		//Give time for radio to stabalize		
		delay(5);

		// Set channel (0-127)	
		writeRegisterByte(RF_CH, 63);

		// Set rf power to max
		uint8_t setup = readRegisterByte(RF_SETUP);
		setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)); // 0x03 max power
		writeRegisterByte(RF_SETUP, setup);

		// Radio speed is 1Mb/s by default
		// Write to RF_SETUP to change to 2Mb/s

		// Initialize 1-byte CRC 
		uint8_t config = readRegisterByte(CONFIG); 
		config |= _BV(EN_CRC);
		writeRegisterByte(CONFIG, config);

		// Reset current status
		writeRegisterByte(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

		// Clear buffers
		flushRXBuffer();
		flushTXBuffer();
	}
	void startListening() {
		writeRegisterByte(CONFIG, readRegisterByte(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
		writeRegisterByte(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));
		// Flush buffers
		flushRXBuffer();
		flushTXBuffer();
		// Start
		ce(HIGH);
		// wait for the radio to come up (130us actually only needed)
		delayMicroseconds(130);
	}
	// Must call stopListening() before write()
	void stopListening() {
		ce(LOW);
		flushTXBuffer();
		flushRXBuffer();
	}
	void write(const void* buffer)
	{
		// Transmitter power up
		writeRegisterByte(CONFIG, (readRegisterByte(CONFIG) | _BV(PWR_UP)) & ~_BV(PRIM_RX));
		delayMicroseconds(150);

		// Send the payload
		const uint8_t* current = reinterpret_cast<const uint8_t*>(buffer);
		uint8_t dataLength = payload_size;

		csn(LOW);
		SPI.transfer(W_TX_PAYLOAD); // First send command
		while (dataLength--) {
			SPI.transfer(*current++); // Then send payload
		}
		csn(HIGH);


		ce(HIGH);
		delayMicroseconds(15);
		ce(LOW);

		// Block until packet sent or timeout 
		uint8_t observe_tx;
		uint8_t status;
		uint32_t sent_at = millis();
		const uint32_t timeout = 500; //ms to wait for timeout
		do
		{
			status = readRegister(OBSERVE_TX, &observe_tx, 1);
		} while (!(status & (_BV(TX_DS))) && (millis() - sent_at < timeout));

		//Reset Status
		writeRegisterByte(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));

		// Power down
		powerDown();
	}
	// Are bytes available to read.
	bool available() {
		uint8_t status;
		// Get value of status register
		csn(LOW);
		status = SPI.transfer(NOP); // Send no operation to read STATUS register
		csn(HIGH);
		
		bool result = (status & _BV(RX_DR)); // Check status if payloaad has been recieved, RX_DR is asserted when data arrives at RX FIFO
		if (result)
		{
			writeRegisterByte(STATUS, _BV(RX_DR));	// Writes data ready back to STATUS		
		}

		return result;
	}
	void read(void* buffer) {
		uint8_t* current = reinterpret_cast<uint8_t*>(buffer);
		uint8_t dataLength = payload_size;

		csn(LOW);

		SPI.transfer(R_RX_PAYLOAD); //read the contents of the RX FIFO 
		while (dataLength--) {
			*current++ = SPI.transfer(0xff);
		}
		csn(HIGH);
	}
	void openReadingPipe(uint64_t address) {
		// Reading pipe 1
		writeRegister(RX_ADDR_P1, reinterpret_cast<const uint8_t*>(&address), 5);
		writeRegisterByte(RX_PW_P1, payload_size);
		writeRegisterByte(EN_RXADDR, readRegisterByte(EN_RXADDR) | _BV(ERX_P1));
	}
	void openWritingPipe(uint64_t address) {

		//Writing on pipe 0
		writeRegister(RX_ADDR_P0, reinterpret_cast<uint8_t*>(&address), 5);
		writeRegister(TX_ADDR, reinterpret_cast<uint8_t*>(&address), 5);
		writeRegisterByte(RX_PW_P0, min(payload_size, max_payload_size));
	}
	// Low power mode
	void powerDown() {
		writeRegisterByte(CONFIG, readRegisterByte(CONFIG) & ~_BV(PWR_UP));
	}
	// Normal power mode
	void powerUp() {
		writeRegisterByte(CONFIG, readRegisterByte(CONFIG) | _BV(PWR_UP));
	}
};


Radio radio(CE_PIN, CSN_PIN);

void setup()
{
	radio.initialize();
	Serial.begin(115200);
	Serial.println("Starting");
	if (transmitState) {
		radio.openWritingPipe(pipe);
		Serial.println("Writing");
	}
	else {
		Serial.println("Reading");
		radio.openReadingPipe(pipe);
		radio.startListening();
	}
}

void loop()
{
	int values[32];
	if (transmitState) {
		values[0] = analogRead(A0);
		radio.write(values);
		Serial.println(values[0]);
		delay(10);
	}
	else {
		if (radio.available()) {
			radio.read(values);
			Serial.println(values[0]);

		}
	}
}
