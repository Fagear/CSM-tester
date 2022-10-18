#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdctype.h"

#define KBD_ESC_CODE		0x1B	// Scancode for [Esc] key.
#define CSM_BASE_DEF		0x220	// Default Covox Sound Master base address.

// CSM internal devices offsets from the base.
enum
{
	CSM_AY_REG = 0x0,			// Number of register in AY
	CSM_AY_DATA = 0x1,		// Data for AY register
	CSM_PCM1 = 0x2,			// Access port for 8-bit DAC
	CSM_IRQ_CLR = 0x3,		// IRQ clear
	CSM_GPAD1 = 0x5,			// Gamepad 1 port
	CSM_GPAD2 = 0x4,			// Gamepad 2 port
	CSM_PCM2 = 0xF				// Access port for 8-bit DAC (same as [CSM_PCM1])
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
	AY_REG_A_FREQ_FINE = AY_R0,	// Frequency of channel A, 8-bit LSB fine tone adjustment.
	AY_REG_A_FREQ_ROUGH = AY_R1,	// Frequency of channel A, 4-bit MSB rough tone adjustment.
	AY_REG_B_FREQ_FINE = AY_R2,	// Frequency of channel B, 8-bit LSB fine tone adjustment.
	AY_REG_B_FREQ_ROUGH = AY_R3,	// Frequency of channel B, 4-bit MSB rough tone adjustment.
	AY_REG_C_FREQ_FINE = AY_R4,	// Frequency of channel C, 8-bit LSB fine tone adjustment.
	AY_REG_C_FREQ_ROUGH = AY_R5,	// Frequency of channel C, 4-bit MSB rough tone adjustment.
	AY_REG_NOISE_FREQ = AY_R6,		// Frequency of noise, 5-bit.
	AY_REG_MIXER = AY_R7,			// I/O ports and mixer flags.
	AY_REG_A_LVL = AY_R8,			// Level of channel A, 5-bit
	AY_REG_B_LVL = AY_R9,			// Level of channel B, 5-bit
	AY_REG_C_LVL = AY_RA,			// Level of channel C, 5-bit
	AY_REG_ENV_FREQ_FINE = AY_RB,	// Frequency of envelope, 8-bit LSB fine adjustment.
	AY_REG_ENV_FREQ_ROUGH = AY_RC,// Frequency of envelope, 8-bit MSB rough adjustment.
	AY_REG_SHAPE_MODE = AY_RD,		// Shape of envelope and mode select (for AY8930).
	AY_REG_IO_A = AY_RE,				// Parallel I/O Port A, 8-bit
	AY_REG_IO_B = AY_RF				// Parallel I/O Port B, 8-bit
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
	PSG_UNKNOWN				// Some unknown variant of AY-compatible
};

// Gamepad buttons/pins.
enum
{
	GP_BTN_UP = (1<<1),
	GP_BTN_DOWN	= (1<<0),
	GP_BTN_LEFT	= (1<<3),
	GP_BTN_RIGHT = (1<<2),
	GP_BTN_FIRE	= (1<<4),
	MS_BTN_LB = (1<<4),
	MS_BTN_MB = (1<<5),
	MS_BTN_RB = (1<<6)
};

// Test functions.
enum
{
	TST_CHA = (1<<0),		// Turn on Channel A sound
	TST_CHB = (1<<1),		// Turn on Channel B sound
	TST_CHC = (1<<2),		// Turn on Channel C sound
	TST_MONO = (1<<3),	// Switch downmix to mono on
	TST_COFF = (1<<4)		// Redirect Channel C to DMA
};

uint8_t getSingleScancode();										// Get scancode from keyboard.
uint8_t readAYReg(uint16_t in_port, uint8_t reg);			// Read data from AY register.
void writeAYReg(uint16_t in_port, uint8_t reg, uint8_t data);	// Write some data to AY register.
void resetAY(uint16_t in_port);									// Reset AY registers.
void fillAY();															// Fill all AY registers with 0xFF.
uint8_t detectAYType(uint16_t in_port);						// Detect PSG IC type.
void printAYType(uint16_t in_port);								// Print PSG IC type.
void printBaseDump(uint16_t in_port);							// Print I/O port read data from base_port+[0...F].
void printAYStdReg(uint16_t in_port, uint8_t in_ofs);		// Print all AY register data for AY8910-compatibility mode.
void printAYExpReg(uint16_t in_port, uint8_t in_bank);	// Print all AY register data for AY8930-expanded mode.
void printAYOvfReg(uint16_t in_port, uint8_t in_ofs);		// Print all filled AY register data.
void printGamepadState(uint16_t in_port, uint8_t in_ofs);// Print gamepad state.
void printUsage();													// Print usage message.
uint8_t processPageMain(uint16_t card_base);					// Print main startup menu.
void processAYStdRegTable(uint16_t card_base);				// Print register table page.
void processAYOvfRegTable(uint16_t card_base);				// Print out-of-bound AY register table page.
void processSoundMuxTest(uint16_t card_base);				// Print sound and mixer testing page.
void processGamepadTest(uint16_t card_base);					// Print gamepad testing page.

int main(int argc, const char* argv[]);
