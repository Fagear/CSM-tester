#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdctype.h"

#define DEF_BASE		0x220

#define AY_MODE_REG	0x0d

// CSM internal devices offsets.
enum
{
	CSM_AY_REG = 0,		// Number of register in AY
	CSM_AY_DATA = 1,		// Data for AY register
	CSM_GPAD1 = 5,			// Gamepad 1 port
	CSM_GPAD2 = 4,			// Gamepad 2 port
};

// AY8930 register banks.
enum
{
	AY8930_BANK_A,
	AY8930_BANK_B
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

// Test functions.
enum
{
	TST_CHA = (1<<0),		// Turn on Channel A sound
	TST_CHB = (1<<1),		// Turn on Channel B sound
	TST_CHC = (1<<2),		// Turn on Channel C sound
	TST_MONO = (1<<3),	// Switch downmix to mono on
	TST_COFF = (1<<4),	// Redirect Channel C to DMA
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

// Read data from AY register.
uint8_t readAYReg(uint16_t in_port, uint8_t reg)
{
	uint8_t read_data;
	read_data = 0;
	// Set AY internal register address.
	outportb(in_port+CSM_AY_REG, reg);
	// Read data from that AY register.
	read_data = inportb(in_port+CSM_AY_DATA);
	return read_data;
}

// Write some data to AY register.
void writeAYReg(uint16_t in_port, uint8_t reg, uint8_t data)
{
	// Set AY internal register address.
	outportb(in_port+CSM_AY_REG, reg);
	// Write data to that AY register.
	outportb(in_port+CSM_AY_DATA, data);
}

// Reset AY registers.
void resetAY(uint16_t in_port)
{
	uint8_t i;
	for(i=0x00;i<=0x0F;i++)
	{
		writeAYReg(in_port, i, 0x00);
	}
}

// Print I/O port read data from base port+[0...F].
void printBaseDump(uint16_t in_port)
{
	uint8_t i, port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();
	// Cycle through ports.
	for(i=0x00;i<=0x0F;i++)
	{
		// Read I/O port data.
		port_res = inportb(in_port+i);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		printf("I/O @ %04xh: ", (in_port+i));
		highvideo();	// Switch to "highlight text".
		cprintf("0x%02x", port_res);
		normvideo();	// Switch to "non-highlight text".
	}
}

// Print all AY register data for AY8910-compatibility mode.
void printAYStdReg(uint16_t in_port, uint8_t in_ofs)
{
	uint8_t i, port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();

	// Set AY8910-mode.
	outportb(in_port+CSM_AY_REG, AY_MODE_REG);
	port_res = inportb(in_port+CSM_AY_DATA);
	outportb(in_port+CSM_AY_DATA, (port_res&0x0F));

	// Cycle through ports.
	for(i=(in_ofs+0x00);i<=(in_ofs+0x0F);i++)
	{
		port_res = readAYReg(in_port, i);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		printf("AY @ R%01X: ", i);
		highvideo();	// Switch to "highlight text".
		cprintf("0x%02x", port_res);
		normvideo();	// Switch to "non-highlight text".
	}
}

// Print all AY register data for AY8930-expanded mode.
void printAYExpReg(uint16_t in_port, uint8_t in_bank)
{
	uint8_t i, port_res, r15_data, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();

	// Save old value of AY-mode register.
	outportb(in_port+CSM_AY_REG, AY_MODE_REG);
	r15_data = inportb(in_port+CSM_AY_DATA);
	if(in_bank!=AY8930_BANK_B)
	{
		// Set AY8930-mode, Bank A.
		outportb(in_port+CSM_AY_DATA, ((r15_data&0x0F)|0xA0));
	}
	else
	{
		// Set AY8930-mode, Bank B.
		outportb(in_port+CSM_AY_DATA, ((r15_data&0x0F)|0xB0));
	}

	// Cycle through ports.
	for(i=0x00;i<=0x0F;i++)
	{
		port_res = readAYReg(in_port, i);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		if(in_bank!=AY8930_BANK_B)
		{
			printf("AY @ R%01XA: ", i);
		}
		else
		{
			printf("AY @ R%01XB: ", i);
		}
		highvideo();	// Switch to "highlight text".
		cprintf("0x%02x", port_res);
		normvideo();	// Switch to "non-highlight text".
	}
	// Restore old value of AY-mode register.
	outportb(in_port+CSM_AY_REG, AY_MODE_REG);
	outportb(in_port+CSM_AY_DATA, r15_data);
}

// Print all AY register overflowen data.
void printAYOvfReg(uint16_t in_port, uint8_t in_ofs)
{
	uint8_t i, port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();

	// Cycle through ports.
	for(i=(in_ofs+0x00);i<=(in_ofs+0x0F);i++)
	{
		// Fill up the register.
		writeAYReg(in_port, i, 0xFF);
		// Read data from that AY register.
		port_res = inportb(in_port+CSM_AY_DATA);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		printf("AY fill @ R%02X: ", i);
		highvideo();	// Switch to "highlight text".
		cprintf("0x%02x", port_res);
		normvideo();	// Switch to "non-highlight text".
	}
}

// Detect PSG chip type.
uint8_t detectAYType(uint16_t in_port)
{
	uint8_t io_data;
	// Reset all registers.
	resetAY(in_port);
	// Check IC presence.
	writeAYReg(in_port, 0x00, 0xA5);
	io_data = inportb(in_port+CSM_AY_DATA);
	if(io_data!=0xA5)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		return PSG_NONE;
	}
	outportb(in_port+CSM_AY_DATA, 0x5A);
	io_data = inportb(in_port+CSM_AY_DATA);
	if(io_data!=0x5A)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		return PSG_NONE;
	}
	// Check for YM2149.
	writeAYReg(in_port, 0x08, 0xFF);		// This register should mask to 0x1F...
	io_data = inportb(in_port+CSM_AY_DATA);
	if(io_data==0xFF)
	{
		// YM2149 reads the same right after write if register was not changed.
		resetAY(in_port);
		return PSG_YM2149;
	}
	else if(io_data!=0x1F)
	{
		// AY-type PSGs should read as 0x1F after write to 0x08...
		resetAY(in_port);
		return PSG_UNKNOWN;
	}
	// Check for AY8930.
	writeAYReg(in_port, AY_MODE_REG, 0xB0);	// AY8930 should switch to Bank B.
	io_data = inportb(in_port+CSM_AY_DATA);
	if(io_data==0xB0)
	{
		resetAY(in_port);
		return PSG_AY8930;
	}
	// Check for AY-3-8910A.
	io_data = readAYReg(in_port, 0x15);		// AY-3-8910A should always read as register #.
	if(io_data==0x15)
	{
		resetAY(in_port);
		return PSG_AY8910;
	}
	else
	{
		resetAY(in_port);
		return PSG_KC89C72;
	}
}

// Print PSG IC type.
void printAYType(uint16_t in_port)
{
	uint8_t temp;
	// Detect PSG type.
	temp = detectAYType(in_port);
	// Print PSG type.
	if(temp==PSG_NONE)
	{
		printf("No AY-type PSG detected!");
	}
	else
	{
		printf("Covox Sound Master (or compatible) at ");
		highvideo();
		cprintf("%04xh", in_port);
		normvideo();
		printf(" has ");
		highvideo();
		// Print IC model.
		if(temp==PSG_AY8930)
		{
			cprintf("AY8930");
		}
		else if(temp==PSG_AY8910)
		{
			cprintf("AY-3-8910A or clone");
		}
		else if(temp==PSG_YM2149)
		{
			cprintf("YM2149F");
		}
		else if(temp==PSG_KC89C72)
		{
			cprintf("KC89C72");
		}
		else
		{
			cprintf("unknown");
		}
		normvideo();
		printf(" PSG onboard.\n\r");
		// Print suggestions.
		if(temp==PSG_YM2149)
		{
			printf("DMA transfers to DAC will not work!");
		}
		else if(temp==PSG_AY8930)
		{
			printf("CSM will work as it should.");
		}
	}
}

// Print gamepad state.
void gamepadPoll(uint16_t in_port)
{
	uint8_t port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();
	// Read data from I/O port.
	port_res = inportb(in_port);
	// Print test header.
	gotoxy(x_coord, y_coord);
	printf("-----------------------");
	normvideo();	// Switch to "non-highlight text".
	gotoxy(x_coord, y_coord+1);
	printf("Input @ I/O %04xh", in_port);
	// Print test data.
	gotoxy(x_coord, y_coord+2);
	printf("Gamepad LEFT:  ");
	highvideo();	// Switch to "highlight text".
	if((port_res&GP_BTN_LEFT)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+3);
	printf("Gamepad RIGHT: ");
	highvideo();
	if((port_res&GP_BTN_RIGHT)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+4);
	printf("Gamepad UP:    ");
	highvideo();
	if((port_res&GP_BTN_UP)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+5);
	printf("Gamepad DOWN:  ");
	highvideo();
	if((port_res&GP_BTN_DOWN)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+6);
	printf("Gamepad FIRE:  ");
	highvideo();
	if((port_res&GP_BTN_FIRE)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+7);
	printf("Mouse LEFT:    ");
	highvideo();
	if((port_res&MS_BTN_LB)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+8);
	printf("Mouse MIDDLE:  ");
	highvideo();
	if((port_res&MS_BTN_MB)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
	gotoxy(x_coord, y_coord+9);
	printf("Mouse RIGHT:   ");
	highvideo();
	if((port_res&MS_BTN_RB)==0)
	{
		cprintf("ACTIVE  ");
	}
	else
	{
		cprintf("released");
	}
	normvideo();
}


// Main function.
int main(int argc, const char* argv[])
{
	uint8_t keyscan, out_start, test_bits, mux_ctrl, port_ctrl;
	uint16_t card_base, in_base;
	// Set default card address.
	card_base = DEF_BASE;
	// Prepare screen.
	clrscr();
	_setcursortype(_NOCURSOR);
	normvideo();
	// Print header.
	printf("Covox Sound Master (CSM) data dumping utility v1.2\n\r");
	printf("by Maksim Kryukov aka Fagear (fagear@mail.ru), 2022\n\r");
	// Check command line parameters.
	if(argc==2)
	{
		if(strcmp(argv[1], "/?")==0)
		{
			// Print help.
			printf("\n\rUsage: csm_dump.exe [port_hex]\n\rExample: csm_dump.exe 240\n\r", argc);
			return 0;
		}
		else
		{
			// Convert input string into port number.
			in_base = (uint16_t)strtol(argv[1], NULL, 16);
			if((in_base>0x200)&&(in_base<0x300))
			{
				// Use provided port number.
				card_base = in_base;
			}
		}
	}
	// Print header.
	printAYType(card_base);
	printf("\n\r[W]: additional info, [S]: sound/switching tests, [Esc]: exit\n\r\n\r");
	printf("  I/O port read    |   AY8910 regs   |  AY8930, bank A   |  AY8930, bank B\n\r");
	printf("--------------------------------------------------------------------------\n\r");
	// Save first data output line.
	out_start = wherey();
	// Cycle displaying standard register set.
	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=0x1B)
	{
		// Dump I/O data.
		gotoxy(1, out_start);
		printBaseDump(card_base);
		// Dump AY registers (AY8910-mode).
		gotoxy(23, out_start);
		printAYStdReg(card_base, 0x00);
		// Dump AY registers (AY8930-mode, Bank A).
		gotoxy(41, out_start);
		printAYExpReg(card_base, AY8930_BANK_A);
		// Dump AY registers (AY8930-mode, Bank B).
		gotoxy(61, out_start);
		printAYExpReg(card_base, AY8930_BANK_B);
		keyscan = getch();
		if((keyscan=='w')||(keyscan=='W')
			||(keyscan=='s')||(keyscan=='S')
			||(keyscan=='j')||(keyscan=='J'))
		{
			break;
		}
	}
	if((keyscan=='w')||(keyscan=='W'))
	{
		// Out-of-bound registers dump.
		normvideo();
		clrscr();
		// Print header.
		printf("CSM base port: ");
		highvideo();
		cprintf("0x%04x\n\r", card_base);
		normvideo();
		printf(" AY std regs  |  AY out-of-bound |    AY 0xFF fill\n\r");
		printf("------------------------------------------------------\n\r");
		// Save first data output line.
		out_start = wherey();
		gotoxy(1, out_start+20);
		printf("Press Esc to exit");
		// Cycle displaying out-of-bounds registers.
		keyscan = 0;
		// Wait for keypress.
		while(keyscan!=0x1B)
		{
			// Dump AY registers.
			gotoxy(1, out_start);
			printAYStdReg(card_base, 0x00);
			gotoxy(18, out_start);
			printAYStdReg(card_base, 0x10);
			gotoxy(36, out_start);
			printAYOvfReg(card_base, 0x00);
			keyscan = getch();
		}
	}
	else if((keyscan=='s')||(keyscan=='S'))
	{
		// Go to sound tests.
		test_bits = 0;
		normvideo();
		clrscr();
		// Print header.
		printf("Covox Sound Master base port: ");
		highvideo();
		cprintf("0x%04x\n\r", card_base);
		normvideo();
		printf("Sound and switching test.\n\r");
		out_start = wherey();
		gotoxy(1, out_start+1);
		printf("Channel A sound:  ");
		highvideo();
		cprintf("DISABLED");
		normvideo();
		printf(" [1]");
		gotoxy(1, out_start+2);
		printf("Channel B sound:  ");
		highvideo();
		cprintf("DISABLED");
		normvideo();
		printf(" [2]");
		gotoxy(1, out_start+3);
		printf("Channel C sound:  ");
		highvideo();
		cprintf("DISABLED");
		normvideo();
		printf(" [3]");
		gotoxy(1, out_start+5);
		printf("Downmix to mono:  ");
		highvideo();
		cprintf("DISABLED");
		normvideo();
		printf(" [D]");
		gotoxy(1, out_start+6);
		printf("Channel C switch: ");
		highvideo();
		cprintf("DISABLED");
		normvideo();
		printf(" [C]");
		gotoxy(1, out_start+8);
		printf("Press Esc to exit");
		highvideo();
		// Zero out all registers.
		resetAY(card_base);
		// Preset frequencies and amplitudes.
		mux_ctrl = 0xFF;							// Turn off all channels
		port_ctrl = 0x80;
		writeAYReg(card_base, 0x00, 224);	// Preset ~500Hz for CH A
		writeAYReg(card_base, 0x08, 0x0F);	// Preset maximum amplitude CH A
		writeAYReg(card_base, 0x02, 111);	// Preset ~1000Hz for CH B
		writeAYReg(card_base, 0x09, 0x0F);	// Preset maximum amplitude CH B
		writeAYReg(card_base, 0x04, 74);		// Preset ~1500Hz for CH C
		writeAYReg(card_base, 0x0A, 0x0F);  // Preset maximum amplitude CH C
		writeAYReg(card_base, 0x07, mux_ctrl);		// Apply mixer settings
		writeAYReg(card_base, 0x0E, 0xFF);	// Turn volume up.
		writeAYReg(card_base, 0x0F, port_ctrl);	// Apply IO port B

		keyscan = 0;
		// Wait for keypress.
		while(keyscan!=0x1B)
		{
			keyscan = getch();
			if(keyscan==0x00)
			{
				// Skip dual-scancode keys.
				keyscan = getch();
			}
			else
			{
				if(keyscan=='1')
				{
					// Channel A toggle.
					if((test_bits&TST_CHA)==0)
					{
						test_bits|=TST_CHA;
						mux_ctrl&=~(1<<0);
						gotoxy(19, out_start+1);
						cprintf(" ENABLED");
					}
					else
					{
						test_bits&=~TST_CHA;
						mux_ctrl|=(1<<0);
						gotoxy(19, out_start+1);
						cprintf("DISABLED");
					}
				}
				else if(keyscan=='2')
				{
					// Channel B toggle.
					if((test_bits&TST_CHB)==0)
					{
						test_bits|=TST_CHB;
						mux_ctrl&=~(1<<1);
						gotoxy(19, out_start+2);
						cprintf(" ENABLED");
					}
					else
					{
						test_bits&=~TST_CHB;
						mux_ctrl|=(1<<1);
						gotoxy(19, out_start+2);
						cprintf("DISABLED");
					}
				}
				else if(keyscan=='3')
				{
					// Channel C toggle.
					if((test_bits&TST_CHC)==0)
					{
						test_bits|=TST_CHC;
						mux_ctrl&=~(1<<2);
						gotoxy(19, out_start+3);
						cprintf(" ENABLED");
					}
					else
					{
						test_bits&=~TST_CHC;
						mux_ctrl|=(1<<2);
						gotoxy(19, out_start+3);
						cprintf("DISABLED");
					}
				}
				else if((keyscan=='d')||(keyscan=='D'))
				{
					// Downmix to mono toggle.
					if((test_bits&TST_MONO)==0)
					{
						test_bits|=TST_MONO;
						port_ctrl|=(1<<4);
						gotoxy(19, out_start+5);
						cprintf(" ENABLED");
					}
					else
					{
						test_bits&=~TST_MONO;
						port_ctrl&=~(1<<4);
						gotoxy(19, out_start+5);
						cprintf("DISABLED");
					}
				}
				else if((keyscan=='c')||(keyscan=='C'))
				{
					// Channel C switch toggle.
					if((test_bits&TST_COFF)==0)
					{
						test_bits|=TST_COFF;
						port_ctrl&=~(1<<7);
						gotoxy(19, out_start+6);
						cprintf(" ENABLED");
					}
					else
					{
						test_bits&=~TST_COFF;
						port_ctrl|=(1<<7);
						gotoxy(19, out_start+6);
						cprintf("DISABLED");
					}
				}
				// Update registers after keypress.
				writeAYReg(card_base, 0x07, mux_ctrl);		// Update mixer
				writeAYReg(card_base, 0x0F, port_ctrl);	// Update IO B
			}
		}
		// Shut down AY.
		resetAY(card_base);
	}
	else if((keyscan=='j')||(keyscan=='J'))
	{
		normvideo();
		clrscr();
		printf("CSM base port: ");
		highvideo();
		cprintf("0x%03x\n\r", card_base);
		normvideo();

		printf("\n\rPress any key to exit.\n\r\n\r");
		printf("GamePad 1 (bottom/left)                GamePad 2 (top/right)\n\r");
		// Save first data output line.
		out_start = wherey();
		// Cycle while any key is hit.
		while(!kbhit())
		{
			// Repeat output from the left.
			gotoxy(1, out_start);
			// Test GamePad 1.
			gamepadPoll(card_base+CSM_GPAD1);
			// Repeat output from the middle of the screen.
			gotoxy(40, out_start);
			// Test GamePad 2.
			gamepadPoll(card_base+CSM_GPAD2);
		}
		out_start = getch();
	}
	return 0;
}