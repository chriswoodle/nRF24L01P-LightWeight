/*
Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/* Memory Map */
#define CONFIG      0x00		//0000 0000
#define EN_AA       0x01		//0000 0001
#define EN_RXADDR   0x02		//0000 0010
#define SETUP_AW    0x03		//0000 0011
#define SETUP_RETR  0x04		//0000 0100
#define RF_CH       0x05		//0000 0101
#define RF_SETUP    0x06		//0000 0110
#define STATUS      0x07		//0000 0111
#define OBSERVE_TX  0x08		//0000 1000
#define CD          0x09		//0000 1001
#define RX_ADDR_P0  0x0A		//0000 1010
#define RX_ADDR_P1  0x0B		//0000 1011
#define RX_ADDR_P2  0x0C		//0000 1100
#define RX_ADDR_P3  0x0D		//0000 1101
#define RX_ADDR_P4  0x0E		//0000 1110
#define RX_ADDR_P5  0x0F		//0000 1111
#define TX_ADDR     0x10		//0001 0000
#define RX_PW_P0    0x11		//0001 0001
#define RX_PW_P1    0x12		//0001 0010
#define RX_PW_P2    0x13		//0001 0011
#define RX_PW_P3    0x14		//0001 0100	
#define RX_PW_P4    0x15		//0001 0101
#define RX_PW_P5    0x16		//0001 0110		
#define FIFO_STATUS 0x17		//0001 0111
#define DYNPD	    0x1C		//0001 1100
#define FEATURE	    0x1D		//0001 1101

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00		//0000 0000
#define W_REGISTER    0x20		//0010 0000
#define REGISTER_MASK 0x1F		//0001 1111
#define ACTIVATE      0x50		//0101 0000
#define R_RX_PL_WID   0x60		//0110 0000
#define R_RX_PAYLOAD  0x61		//0110 0000
#define W_TX_PAYLOAD  0xA0		//1010 0000
#define W_ACK_PAYLOAD 0xA8		//1010 1000
#define FLUSH_TX      0xE1		//1110 0001
#define FLUSH_RX      0xE2		//1110 0010
#define REUSE_TX_PL   0xE3		//1110 0011
#define NOP           0xFF		//1111 1111

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD         0x09		//0000 1001

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2
