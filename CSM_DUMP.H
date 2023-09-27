/**************************************************************************************************************************************************************
CSM_DUMP.H

Copyright © 2023 Maksim Kryukov <fagear@mail.ru>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Created: 2020-08

Covox Sound Master sound card testing utility with PSG auto-detection.
Tests PSG audio channels, PCM output, gain control and gamepad inputs.

**************************************************************************************************************************************************************/

#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdctype.h"

#define KBD_ESC_CODE		0x1B	// Scancode for [Esc] key
#define CSM_BASE_DEF		0x220	// Default Covox Sound Master base address
#define PCM_SEQ_SIZE		6		// Size of the PCM sample sequence
#define DMA1_SEQ_SIZE		284	   	// Size of the test sequence for DMA channel 1
#define DMA3_SEQ_SIZE		330		// Size of the test sequence for DMA channel 3
#define DUMMY_WRITE			0x0		// Byte for dumy writes

// CSM internal devices offsets from the base address.
enum
{
	CSM_AY_REG = 0x0,		// Number of register in AY
	CSM_AY_DATA = 0x1,		// Data for AY register
	CSM_PCM1 = 0x2,			// Access port for 8-bit DAC
	CSM_IRQ_CLR = 0x3,		// IRQ clear
	CSM_GPAD1 = 0x5,		// Gamepad 1 port
	CSM_GPAD2 = 0x4,		// Gamepad 2 port
	CSM_PCM2 = 0xF,			// Access port for 8-bit DAC (same as [CSM_PCM1])
};

// AY8930 registers.
enum
{
	AY_R0 = 0x00,
	AY_R1 = 0x01,
	AY_R2 = 0x02,
	AY_R3 = 0x03,
	AY_R4 = 0x04,
	AY_R5 = 0x05,
	AY_R6 = 0x06,
	AY_R7 = 0x07,
	AY_R8 = 0x08,
	AY_R9 = 0x09,
	AY_RA = 0x0A,
	AY_RB = 0x0B,
	AY_RC = 0x0C,
	AY_RD = 0x0D,
	AY_RE = 0x0E,
	AY_RF = 0x0F,
	AY_REG_A_FREQ_FINE = AY_R0,		// Frequency of channel A, 8-bit LSB fine tone adjustment.
	AY_REG_A_FREQ_ROUGH = AY_R1,	// Frequency of channel A, 4-bit MSB rough tone adjustment.
	AY_REG_B_FREQ_FINE = AY_R2,		// Frequency of channel B, 8-bit LSB fine tone adjustment.
	AY_REG_B_FREQ_ROUGH = AY_R3,	// Frequency of channel B, 4-bit MSB rough tone adjustment.
	AY_REG_C_FREQ_FINE = AY_R4,		// Frequency of channel C, 8-bit LSB fine tone adjustment.
	AY_REG_C_FREQ_ROUGH = AY_R5,	// Frequency of channel C, 4-bit MSB rough tone adjustment.
	AY_REG_NOISE_FREQ = AY_R6,		// Frequency of noise, 5-bit.
	AY_REG_MIXER = AY_R7,			// I/O ports and mixer flags.
	AY_REG_A_LVL = AY_R8,			// Level of channel A, 5-bit
	AY_REG_B_LVL = AY_R9,			// Level of channel B, 5-bit
	AY_REG_C_LVL = AY_RA,			// Level of channel C, 5-bit
	AY_REG_ENV_FREQ_FINE = AY_RB,	// Frequency of envelope, 8-bit LSB fine adjustment.
	AY_REG_ENV_FREQ_ROUGH = AY_RC,	// Frequency of envelope, 8-bit MSB rough adjustment.
	AY_REG_SHAPE_MODE = AY_RD,		// Shape of envelope and mode select (for AY8930).
	AY_REG_IO_A = AY_RE,			// Parallel I/O Port A, 8-bit
	AY_REG_IO_B = AY_RF,			// Parallel I/O Port B, 8-bit
};

// AY8930 register banks in [AY_REG_SHAPE_MODE].
enum
{
	AY8930_BANK_A = 0xA0,
	AY8930_BANK_B = 0xB0
};

// Supported AY-compatible ICs.
enum
{
	PSG_NONE,				// AY-compatible PSG not found
	PSG_AY8910,				// AY-3-8910 detected
	PSG_AY8930,				// AY8930 detected
	PSG_YM2149,				// YM2149 detected
	PSG_KC89C72,			// KC89C72 detected
	PSG_AVR_AY,				// ATmega AY-emulator
	PSG_UNKNOWN				// Some unknown variant of AY-compatible
};

// Bits for AY [AY_REG_MIXER].
enum
{
	AY_A_TONE_EN = (1<<0),	// Enable AY channel A tone output.
	AY_B_TONE_EN = (1<<1),	// Enable AY channel B tone output.
	AY_C_TONE_EN = (1<<2),	// Enable AY channel C tone output.
	AY_A_NOISE_EN = (1<<3),	// Enable AY channel A noise output.
	AY_B_NOISE_EN = (1<<4),	// Enable AY channel B noise output.
	AY_C_NOISE_EN = (1<<5),	// Enable AY channel C noise output.
};

// AY IO port B bits for [AY_REG_IO_B].
enum
{
	AY_IOB_MIX_MON = (1<<4),// Downmix stereo to mono in the external (to AY) mixer.
	AY_IOB_DMA_DIS = (1<<5),// Disable reaction on DMA ACKs.
	AY_IOB_IRQ_DIS = (1<<6),// Disable IRQ requests from DMA ACKs.
	AY_IOB_C_OUT = (1<<7),	// Switch AY channel C to audio output instead of DMA DRQ.
};

// Gamepad buttons/pins.
enum
{
	GP_BTN_UP = (1<<1),		// Gamepad/joystick: directional, up
	GP_BTN_DOWN	= (1<<0),	// Gamepad/joystick: directional, down
	GP_BTN_LEFT	= (1<<3),	// Gamepad/joystick: directional, left
	GP_BTN_RIGHT = (1<<2),	// Gamepad/joystick: directional, right
	GP_BTN_FIRE	= (1<<4),	// Gamepad/joystick: action/fire
	MS_BTN_LB = (1<<4),		// Mouse: left button
	MS_BTN_MB = (1<<5),		// Mouse: middle button
	MS_BTN_RB = (1<<6),		// Mouse: right button
};

// Test functions.
enum
{
	TST_CHA = (1<<0),		// Turn on Channel A sound
	TST_CHB = (1<<1),		// Turn on Channel B sound
	TST_CHC = (1<<2),		// Turn on Channel C sound
	TST_MONO = (1<<3),		// Switch on downmix to mono
	TST_CDMA = (1<<4),		// Redirect Channel C to DMA
};

// Gain control steps.
enum
{
	VOL_000 = 0x00,
	VOL_025 = 0x44,
	VOL_050 = 0x88,
	VOL_075 = 0xCC,
	VOL_100 = 0xFF
};

// Interrupt stuff.
enum
{
	IRQ_CMD_BASE = 0x20,	// Base address for IRQ command register
	IRQ_CTRL_BASE = 0x21,	// Base address for IRQ control register
	ISA_IRQ3 = 0x0B,		// IRQ3 vector
	ISA_IRQ7 = 0x0F,		// IRQ7 vector
	ISA_IRQ3_MASK = (1<<3),	// IRQ3 mask
	ISA_IRQ7_MASK = (1<<7),	// IRQ7 mask
	IRQ_ACK_INT = 0x20,		// Content for [IRQ_CMD_BASE] register to end IRQ
};

// DMA stuff.
enum
{
	DMA_03REG_CH1CNT = 0x03,// DMA counter register for ch 1
	DMA_03REG_CH3CNT = 0x07,// DMA counter register for ch 3
	DMA_03REG_CH1ADR = 0x02,// DMA start addresss for ch 1
	DMA_03REG_CH3ADR = 0x06,// DMA start addresss for ch 3
	DMA_03REG_MASK = 0x0A,	// DMA (single) mask register (ch 0... ch 3)
	DMA_03REG_MODE = 0x0B,	// DMA mode register (ch 0... ch 3)
	DMA_03REG_RST = 0x0C,	// Flip-flop reset register (ch 0... ch 3)
	DMA_03REG_CH1PG = 0x83,	// Page register for ch 1
	DMA_03REG_CH3PG = 0x82,	// Page register for ch 3
	DMA_CH1_SEL = 0x01,		// DMA ch 1 for [DMA_03REG_MODE] and [DMA_03REG_MASK]
	DMA_CH3_SEL = 0x03,		// DMA ch 3 for [DMA_03REG_MODE] and [DMA_03REG_MASK]
	DMA_MASK_EN = (1<<2),	// DMA mask enable for [DMA_03REG_MASK]
	DMA_MODE_RD = 0x08,		// Device will read from memory
	DMA_MODE_WR = 0x04,		// Device will write into memory
	DMA_MODE_AUTO = (1<<4),	// Auto-init DMA on transfer completion
	DMA_MODE_DEC = (1<<5),	// DMA will decrement address on each transfer (instead of incrementing)
	DMA_MODE_SGL = 0x40,	// Single transfer DMA
	DMA_MODE_BLK = 0x80,	// Block transfer DMA
};

// Version info.
enum
{
	VER_MAJOR = 1,
	VER_MINOR = 6
};

uint8_t getSingleScancode();									// Get scancode from keyboard.
uint8_t readAYReg(uint16_t in_port, uint8_t reg);				// Read data from AY register.
void writeAYReg(uint16_t in_port, uint8_t reg, uint8_t data);	// Write some data to AY register.
void resetAY(uint16_t in_port);									// Reset AY registers.
void fillAY();													// Fill all AY registers with 0xFF.
uint8_t detectAYType(uint16_t in_port, uint8_t *detect_stage);	// Detect PSG IC type.
void printAYType(uint16_t in_port);								// Print PSG IC type.
void printBaseDump(uint16_t in_port);							// Print I/O port read data from base_port+[0...F].
void printAYStdReg(uint16_t in_port, uint8_t in_ofs);			// Print all AY register data for AY8910-compatibility mode.
void printAYExpReg(uint16_t in_port, uint8_t in_bank);			// Print all AY register data for AY8930-expanded mode.
void printAYOvfReg(uint16_t in_port, uint8_t in_ofs);			// Print all filled AY register data.
void printGamepadState(uint16_t in_port, uint8_t in_ofs);		// Print gamepad state.
void printUsage();												// Print usage message.
uint8_t processPageMain(uint16_t card_base);					// Print main startup menu.
void processAYStdRegTable(uint16_t card_base);					// Print register table page.
void processAYOvfRegTable(uint16_t card_base);					// Print out-of-bound AY register table page.
void processSoundMuxTest(uint16_t card_base);					// Print sound and mixer testing page.
void processGamepadTest(uint16_t card_base);					// Print gamepad testing page.
void processAddressSpamTest(uint16_t card_base);				// Print single port testing page.
void saveIntHandlers();											// Replace IRQ handlers used by CSM for testing.
void restoreIntHandlers();										// Restore original IRQ handlers after testing.

int main(int argc, const char* argv[]);
