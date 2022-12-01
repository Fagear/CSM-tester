#include "C:\DOS\FMONSTER\AY\SOFT\CSMTEST\CSM_DUMP.H"


uint8_t pcm_sequence[6] =
{
	0x00,
	0x40,
	0x80,
	0xC0,
	0xFF,
	0x80
};

uint8_t detect_stage;

// Get scancode from keyboard.
uint8_t getSingleScancode()
{
	uint8_t keyscan;
	keyscan = 0;
	while(keyscan!=KBD_ESC_CODE)
	{
		// Wait for keypress.
		keyscan = getch();
		// Detect dual-code keys.
		if(keyscan==0x00)
		{
			// Skip dual-scancode keys.
			keyscan = getch();
		}
		else
		{
			break;
		}
	}
	return keyscan;
}

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
	for(i=AY_R0;i<=AY_RF;i++)
	{
		writeAYReg(in_port, i, 0x00);
	}
}

// Fill all AY registers with 0xFF.
void fillAY(uint16_t in_port)
{
	uint8_t i;
	for(i=AY_R0;i<=AY_RF;i++)
	{
		writeAYReg(in_port, i, 0xFF);
	}
}

// Detect PSG IC type.
uint8_t detectAYType(uint16_t in_port)
{
	uint8_t io_data;
	// Reset all registers.
	resetAY(in_port);
	// Write some bit-patterns to test PSG presence, data bus and registers integrity.
	writeAYReg(in_port, AY_R0, 0xA5);	// REG 0b00000000, DATA 0b10100101
	writeAYReg(in_port, AY_RC, 0x5A);	// REG 0b00001100, DATA 0b01011010
	writeAYReg(in_port, AY_R3, 0x0F);	// REG 0b00000011, DATA 0b00001111
	detect_stage = 0;
	// Read first pattern.
	io_data = readAYReg(in_port, AY_R0);
	if(io_data!=0xA5)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		detect_stage = 1;
		return PSG_NONE;
	}
	// Read second pattern.
	io_data = readAYReg(in_port, AY_RC);
	if(io_data!=0x5A)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		detect_stage = 2;
		return PSG_NONE;
	}
	// Read third pattern.
	io_data = readAYReg(in_port, AY_R3);
	if(io_data!=0x0F)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		detect_stage = 3;
		return PSG_NONE;
	}
	detect_stage = 4;
	// Check for YM2149.
	writeAYReg(in_port, AY_REG_C_LVL, 0xFF);		// This register should mask to 0x1F right away on AY...
	io_data = inportb(in_port+CSM_AY_DATA);		// Read it back without re-addressing
	if(io_data==0xFF)
	{
		// YM2149 and AVR-AY read the same right after write without masking if register address was not changed.
		// Check if MSBs mask to 0x0F after re-addressing.
		writeAYReg(in_port, AY_REG_A_FREQ_ROUGH, 0xFF);
		writeAYReg(in_port, AY_REG_B_FREQ_ROUGH, 0xFF);
		writeAYReg(in_port, AY_REG_C_FREQ_ROUGH, 0xFF);
		if((readAYReg(in_port, AY_REG_A_FREQ_ROUGH)==0x0F)&&
			(readAYReg(in_port, AY_REG_B_FREQ_ROUGH)==0x0F)&&
			(readAYReg(in_port, AY_REG_C_FREQ_ROUGH)==0x0F))
		{
			// 0xFF masks to 0x0F after re-addressing on YM2149.
			resetAY(in_port);
			return PSG_YM2149;
		}
		else
		{
			// May also check out-of-bound registers:
			// for 0x3f, 0x7f, 0xbf readbacks
			// but there is no reason (yet).
			// It is AVR-AY emulator.
			resetAY(in_port);
			return PSG_AVR_AY;
		}
	}
	else if(io_data!=0x1F)
	{
		// AY-type PSGs should read as 0x1F after write to [AY_REG_C_LVL]...
		resetAY(in_port);
		return PSG_UNKNOWN;
	}
	// Check for AY8930.
	writeAYReg(in_port, AY_REG_SHAPE_MODE, AY8930_BANK_B);	// AY8930 should switch to Bank B.
	io_data = inportb(in_port+CSM_AY_DATA);
	if(io_data==AY8930_BANK_B)
	{
		resetAY(in_port);
		return PSG_AY8930;
	}
	// Check for AY-3-8910A.
	io_data = readAYReg(in_port, 0x15);		// AY-3-8910A should always read back out-of-bounds as register #.
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
		printf("No AY-type PSG detected at ");
		highvideo();
		cprintf("%04xh (failed at stage %u)", in_port, detect_stage);
		normvideo();
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
		else if(temp==PSG_AVR_AY)
		{
			cprintf("AVR-AY emulator");
		}
		else
		{
			cprintf("unknown");
		}
		normvideo();
		printf(" PSG onboard.");
		// Print suggestions.
		if(temp==PSG_YM2149)
		{
			printf("\n\rDMA transfers to DAC will not work!");
		}
		else if(temp==PSG_AVR_AY)
		{
			printf("\n\rMixer switching, volume control and DMA transfers to DAC will not work!");
		}
		else if(temp==PSG_AY8930)
		{
			printf("\n\rCSM will work as it should.");
		}
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
	port_res = readAYReg(in_port, AY_REG_SHAPE_MODE);
	outportb(in_port+CSM_AY_DATA, (port_res&0x0F));

	// Cycle through ports.
	for(i=(in_ofs+AY_R0);i<=(in_ofs+AY_RF);i++)
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
	r15_data = readAYReg(in_port, AY_REG_SHAPE_MODE);
	if(in_bank==AY8930_BANK_A)
	{
		// Set AY8930-mode, Bank A.
		outportb(in_port+CSM_AY_DATA, ((r15_data&0x0F)|AY8930_BANK_A));
	}
	else if(in_bank==AY8930_BANK_B)
	{
		// Set AY8930-mode, Bank B.
		outportb(in_port+CSM_AY_DATA, ((r15_data&0x0F)|AY8930_BANK_B));
	}
	else
	{
		return;
	}

	// Cycle through ports.
	for(i=AY_R0;i<=AY_RF;i++)
	{
		port_res = readAYReg(in_port, i);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		if(in_bank==AY8930_BANK_A)
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
	writeAYReg(in_port, AY_REG_SHAPE_MODE, r15_data);
}

// Print all filled AY register data.
void printAYOvfReg(uint16_t in_port, uint8_t in_ofs)
{
	uint8_t i, port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();

	// Cycle through ports.
	for(i=(in_ofs+AY_R0);i<=(in_ofs+AY_RF);i++)
	{
		// Fill up the register.
		writeAYReg(in_port, i, 0xFF);
		// Read data from that AY register without re-addressing.
		port_res = inportb(in_port+CSM_AY_DATA);
		// Print at desired location on screen.
		gotoxy(x_coord, y_coord++);
		printf("AY fill @ R%02X: ", i);
		highvideo();	// Switch to "highlight text".
		cprintf("0x%02x", port_res);
		normvideo();	// Switch to "non-highlight text".
	}
}

// Print gamepad state.
void printGamepadState(uint16_t in_port, uint8_t in_ofs)
{
	uint8_t port_res, x_coord, y_coord;
	// Store starting screen coordinates.
	x_coord = wherex();
	y_coord = wherey();
	// Read data from I/O port.
	in_port += in_ofs;
	port_res = inportb(in_port);
	// Print test header.
	normvideo();	// Switch to "non-highlight text".
	gotoxy(x_coord, y_coord);
	printf("-----------------------");
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

// Print header.
void printHeader()
{
	printf("Covox Sound Master (CSM) testing utility v%u.%u\n\r", VER_MAJOR, VER_MINOR);
	printf("by Maksim Kryukov aka Fagear (fagear@mail.ru), 2022\n\r");
}

// Print usage message.
void printUsage()
{
	printHeader();
	printf("\n\rUsage: csm_dump.exe [port_hex]\n\rExample: csm_dump.exe 220\n\r");
	printf("Usually available ports: 220, 240, 280, 2C0\n\r");
}

// Print main startup page.
uint8_t processPageMain(uint16_t card_base)
{
	uint8_t keyscan;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	// Print PSG type.
	printAYType(card_base);
	resetAY(card_base);
	// Print help.
	printf("\n\r\n\r");
	highvideo();
	cprintf("[R]");
	normvideo();
	printf(": normal registers dump\n\r");
	highvideo();
	cprintf("[E]");
	normvideo();
	printf(": overflow AY registers dump\n\r");
	highvideo();
	cprintf("[S]");
	normvideo();
	printf(": sound/switching tests\n\r");
	highvideo();
	cprintf("[G]");
	normvideo();
	printf(": gamepad state\n\r");
	highvideo();
	cprintf("[D]");
	normvideo();
	printf(": addressing tests\n\r");
	highvideo();
	cprintf("[Esc]");
	normvideo();
	printf(": exit\n\r\n\r");
	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		keyscan = getSingleScancode();
		if((keyscan=='r')||(keyscan=='R')
			||(keyscan=='e')||(keyscan=='E')
			||(keyscan=='s')||(keyscan=='S')
			||(keyscan=='g')||(keyscan=='G')
			||(keyscan=='d')||(keyscan=='D'))
		{
			break;
		}
	}
	return keyscan;
}

// Print register table page.
void processAYStdRegTable(uint16_t card_base)
{
	uint8_t keyscan, out_start;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	// Print PSG type.
	printAYType(card_base);
	// Print help.
	printf("\n\r\n\r[Esc]: back to main menu\n\r\n\r");
	printf("  I/O port read    |   AY8910 regs   |  AY8930, bank A   |  AY8930, bank B\n\r");
	printf("--------------------------------------------------------------------------\n\r");
	// Save first data output line.
	out_start = wherey();
	// Cycle displaying standard register set.
	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		fillAY(card_base);
		// Dump AY registers (AY8930-mode, Bank A).
		gotoxy(41, out_start);
		printAYExpReg(card_base, AY8930_BANK_A);
		// Dump AY registers (AY8930-mode, Bank B).
		gotoxy(61, out_start);
		printAYExpReg(card_base, AY8930_BANK_B);
		// Dump AY registers (AY8910-mode).
		gotoxy(23, out_start);
		printAYStdReg(card_base, 0x00);
		// Dump I/O data.
		gotoxy(1, out_start);
		printBaseDump(card_base);
		resetAY(card_base);
		keyscan = getSingleScancode();
	}
}

// Print out-of-bound AY register table page.
void processAYOvfRegTable(uint16_t card_base)
{
	uint8_t keyscan, out_start;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	// Print PSG type.
	printAYType(card_base);
	// Print help.
	printf("\n\r\n\r[Esc]: back to main menu\n\r\n\r");
	printf(" AY std regs  |  AY out-of-bound |    AY 0xFF fill\n\r");
	printf("------------------------------------------------------\n\r");
	// Save first data output line.
	out_start = wherey();
	// Cycle displaying out-of-bounds registers.
	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		// Dump AY registers.
		gotoxy(36, out_start);
		printAYOvfReg(card_base, 0x00);
		gotoxy(18, out_start);
		printAYStdReg(card_base, 0x10);
		gotoxy(1, out_start);
		printAYStdReg(card_base, 0x00);
		resetAY(card_base);
		keyscan = getSingleScancode();
	}
}

// Print sound and mixer testing page.
void processSoundMuxTest(uint16_t card_base)
{
	uint8_t keyscan, out_start, test_bits;
	uint8_t mix_ctrl, port_ctrl, volume_ctrl, pcm_data;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	// Print PSG type.
	printAYType(card_base);
	// Print help.
	printf("\n\r\n\r[Esc]: back to main menu\n\r\n\r");
	printf("Sound and switching test.\n\r");
	out_start = wherey();
	gotoxy(1, out_start+1);
	printf("AY channel A sound: ");
	highvideo();
	cprintf("DISABLED");
	normvideo();
	printf(" [1]");
	gotoxy(1, out_start+2);
	printf("AY channel B sound: ");
	highvideo();
	cprintf("DISABLED");
	normvideo();
	printf(" [2]");
	gotoxy(1, out_start+3);
	printf("AY channel C sound: ");
	highvideo();
	cprintf("DISABLED");
	normvideo();
	printf(" [3]");
	gotoxy(1, out_start+4);
	printf("PCM DAC @ 0x%03x:    ", (card_base+CSM_PCM1));
	highvideo();
	cprintf("STATIC  ");
	normvideo();
	printf(" [4]");
	gotoxy(1, out_start+5);
	printf("PCM DAC @ 0x%03x:    ", (card_base+CSM_PCM2));
	highvideo();
	cprintf("STATIC  ");
	normvideo();
	printf(" [5]");
	gotoxy(1, out_start+7);
	printf("Downmix to mono:    ");
	highvideo();
	cprintf("DISABLED");
	normvideo();
	printf(" [D]");
	gotoxy(1, out_start+8);
	printf("Channel C switch:   ");
	highvideo();
	cprintf("DISABLED");
	normvideo();
	printf(" [C]");
	gotoxy(1, out_start+9);
	printf("Master volume:      ");
	highvideo();
	cprintf("    100%");
	normvideo();
	printf(" [-][+]");
	highvideo();
	// Zero out all registers.
	resetAY(card_base);
	test_bits = 0;
	// Preset frequencies and amplitudes.
	mix_ctrl = 0xFF;											// Turn off all channels
	port_ctrl = 0xE0;
	volume_ctrl = 0xFF;
	pcm_data = 0;
	writeAYReg(card_base, AY_REG_A_FREQ_FINE, 224);	// Preset ~500Hz (@1.79 MHz) for CH A
	writeAYReg(card_base, AY_REG_A_LVL, 0x0F);		// Preset maximum amplitude CH A
	writeAYReg(card_base, AY_REG_B_FREQ_FINE, 111);	// Preset ~1000Hz (@1.79 MHz) for CH B
	writeAYReg(card_base, AY_REG_B_LVL, 0x0F);		// Preset maximum amplitude CH B
	writeAYReg(card_base, AY_REG_C_FREQ_FINE, 74);	// Preset ~1500Hz (@1.79 MHz) for CH C
	writeAYReg(card_base, AY_REG_C_LVL, 0x0F);		// Preset maximum amplitude CH C
	writeAYReg(card_base, AY_REG_MIXER, mix_ctrl);	// Apply mixer settings
	writeAYReg(card_base, AY_REG_IO_A, volume_ctrl);// Turn volume up (via LM13600, connected to IO Port A)
	writeAYReg(card_base, AY_REG_IO_B, port_ctrl);	// Apply IO port B (mux control)

	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		//if(kbhit())
		{
			keyscan = getSingleScancode();
		}
		/*else
		{
			keyscan = 0;
		}*/
		if(keyscan=='1')
		{
			// Channel A toggle.
			if((test_bits&TST_CHA)==0)
			{
				test_bits|=TST_CHA;
				mix_ctrl&=~(1<<0);
				gotoxy(21, out_start+1);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHA;
				mix_ctrl|=(1<<0);
				gotoxy(21, out_start+1);
				cprintf("DISABLED");
			}
		}
		else if(keyscan=='2')
		{
			// Channel B toggle.
			if((test_bits&TST_CHB)==0)
			{
				test_bits|=TST_CHB;
				mix_ctrl&=~(1<<1);
				gotoxy(21, out_start+2);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHB;
				mix_ctrl|=(1<<1);
				gotoxy(21, out_start+2);
				cprintf("DISABLED");
			}
		}
		else if(keyscan=='3')
		{
			// Channel C toggle.
			if((test_bits&TST_CHC)==0)
			{
				test_bits|=TST_CHC;
				mix_ctrl&=~(1<<2);
				gotoxy(21, out_start+3);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHC;
				mix_ctrl|=(1<<2);
				gotoxy(21, out_start+3);
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
				gotoxy(21, out_start+7);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_MONO;
				port_ctrl&=~(1<<4);
				gotoxy(21, out_start+7);
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
				gotoxy(21, out_start+8);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_COFF;
				port_ctrl|=(1<<7);
				gotoxy(21, out_start+8);
				cprintf("DISABLED");
			}
		}
		else if(keyscan=='-')
		{
			// Volume down.
			if(volume_ctrl==0xFF)
			{
				volume_ctrl=0xCC;
				gotoxy(21, out_start+9);
				cprintf("     75%");
			}
			else if(volume_ctrl==0xCC)
			{
				volume_ctrl=0x88;
				gotoxy(21, out_start+9);
				cprintf("     50%");
			}
			else if(volume_ctrl==0x88)
			{
				volume_ctrl=0x44;
				gotoxy(21, out_start+9);
				cprintf("     25%");
			}
			else
			{
				volume_ctrl=0x00;
				gotoxy(21, out_start+9);
				cprintf("      0%");
			}
		}
		else if(keyscan=='+')
		{
			// Volume up.
			if(volume_ctrl==0x00)
			{
				volume_ctrl=0x44;
				gotoxy(21, out_start+9);
				cprintf("     25%");
			}
			else if(volume_ctrl==0x44)
			{
				volume_ctrl=0x88;
				gotoxy(21, out_start+9);
				cprintf("     50%");
			}
			else if(volume_ctrl==0x88)
			{
				volume_ctrl=0xCC;
				gotoxy(21, out_start+9);
				cprintf("     75%");
			}
			else
			{
				volume_ctrl=0xFF;
				gotoxy(21, out_start+9);
				cprintf("    100%");
			}
		}
		if(keyscan=='4')
		{
			pcm_data++;
			if(pcm_data>=6)
			{
				pcm_data = 0;
			}
			outportb(card_base+CSM_PCM1, pcm_sequence[pcm_data]);
			gotoxy(21, out_start+4);
			cprintf("UPDATING");
		}
		else
		{
			gotoxy(21, out_start+4);
			cprintf("STATIC  ");
		}
		if(keyscan=='5')
		{
			pcm_data++;
			if(pcm_data>=6)
			{
				pcm_data = 0;
			}
			outportb(card_base+CSM_PCM2, pcm_sequence[pcm_data]);
			gotoxy(21, out_start+5);
			cprintf("UPDATING");
		}
		else
		{
			gotoxy(21, out_start+5);
			cprintf("STATIC  ");
		}
		// Update registers after keypress.
		writeAYReg(card_base, AY_REG_MIXER, mix_ctrl);		// Update mixer
		writeAYReg(card_base, AY_REG_IO_B, port_ctrl);		// Update IO B (mux flags)
		writeAYReg(card_base, AY_REG_IO_A, volume_ctrl);	// Update IO A (volume control)
	}
}

// Print gamepad testing page.
void processGamepadTest(uint16_t card_base)
{
	uint8_t out_start;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	// Print PSG type.
	printAYType(card_base);
	resetAY(card_base);
	// Print help.
	printf("\n\r\n\rAny key: back to main menu\n\r\n\r");
	printf("Gamepads test.\n\r\n\r");
	printf("GamePad 1 (top/right)                  GamePad 2 (bottom/left)\n\r");
	// Save first data output line.
	out_start = wherey();
	// Cycle while any key is hit.
	while(!kbhit())
	{
		// Repeat output from the left.
		gotoxy(1, out_start);
		// Test GamePad 1.
		printGamepadState(card_base, CSM_GPAD1);
		// Repeat output from the middle of the screen.
		gotoxy(40, out_start);
		// Test GamePad 2.
		printGamepadState(card_base, CSM_GPAD2);
	}
	out_start = getSingleScancode();
}

// Print single port testing page.
void processAddressSpamTest(uint16_t card_base)
{
	uint8_t out_start;
	uint8_t keyscan;
	uint8_t flag;
	// Prepare screen.
	normvideo();
	clrscr();
	_setcursortype(_NOCURSOR);
	gotoxy(1, 1);
	// Print header.
	printHeader();
	resetAY(card_base);
	// Print help.
	printf("\n\r\n\r[Esc]: back to main menu\n\r\n\r");
	printf("Single sub-port addressing test.\n\r\n\r");
	out_start = wherey();
	gotoxy(1, out_start+1);
	highvideo();
	cprintf("[1]");
	normvideo();
	printf(": perform a write into AY address register 0x%03x\n\r", card_base+CSM_AY_REG);
	gotoxy(1, out_start+2);
	highvideo();
	cprintf("[2]");
	normvideo();
	printf(": perform a write into AY data register 0x%03x\n\r", card_base+CSM_AY_DATA);
	gotoxy(1, out_start+3);
	highvideo();
	cprintf("[3]");
	normvideo();
	printf(": perform a read from AY address register 0x%03x\n\r", card_base+CSM_AY_REG);
	gotoxy(1, out_start+4);
	highvideo();
	cprintf("[4]");
	normvideo();
	printf(": perform a read from AY data register 0x%03x\n\r", card_base+CSM_AY_DATA);
	gotoxy(1, out_start+5);
	highvideo();
	cprintf("[5]");
	normvideo();
	printf(": perform a write to DAC register 0x%03x\n\r", card_base+CSM_PCM1);
	gotoxy(1, out_start+6);
	highvideo();
	cprintf("[6]");
	normvideo();
	printf(": perform a write to DAC (copy) register 0x%03x\n\r", card_base+CSM_PCM2);
	gotoxy(1, out_start+7);
	highvideo();
	cprintf("[7]");
	normvideo();
	printf(": perform a write to IRQ clear flag 0x%03x\n\r", card_base+CSM_IRQ_CLR);
	gotoxy(1, out_start+8);
	highvideo();
	cprintf("[8]");
	normvideo();
	printf(": perform a read from Gamepad 1 register 0x%03x\n\r", card_base+CSM_GPAD1);
	gotoxy(1, out_start+9);
	highvideo();
	cprintf("[9]");
	normvideo();
	printf(": perform a read from Gamepad 2 register 0x%03x\n\r", card_base+CSM_GPAD2);
	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		keyscan = getSingleScancode();
		if(keyscan=='1')
		{
			// Write to AY internal register address.
			outportb(card_base+CSM_AY_REG, 0x00);
			keyscan = 0;
		}
		else if(keyscan=='2')
		{
			// Write to AY data register.
			outportb(card_base+CSM_AY_DATA, 0x0F);
			keyscan = 0;
		}
		else if(keyscan=='3')
		{
			// Read from AY data register.
			inportb(card_base+CSM_AY_REG);
			keyscan = 0;
		}
		else if(keyscan=='4')
		{
			// Read from AY internal register address.
			inportb(card_base+CSM_AY_DATA);
			keyscan = 0;
		}
		else if(keyscan=='5')
		{
			// Write to DAC data register.
			outportb(card_base+CSM_PCM1, 0x80);
			keyscan = 0;
		}
		else if(keyscan=='6')
		{
			// Write to DAC (copy) data register.
			outportb(card_base+CSM_PCM2, 0x80);
			keyscan = 0;
		}
		else if(keyscan=='7')
		{
			// Write to IRQ clear flag.
			outportb(card_base+CSM_IRQ_CLR, 0x00);
			keyscan = 0;
		}
		else if(keyscan=='8')
		{
			// Read from Gamepad 1 register.
			inportb(card_base+CSM_GPAD1);
			keyscan = 0;
		}
		else if(keyscan=='9')
		{
			// Read from Gamepad 2 register.
			inportb(card_base+CSM_GPAD2);
			keyscan = 0;
		}
		gotoxy(1, out_start+11);
		if(flag==0)
		{
			printf("Update indicator: _-_-_-_-_-_");
			flag = 1;
		}
		else
		{
			printf("Update indicator: -_-_-_-_-_-");
			flag = 0;
		}
	}
}

// Main function.
int main(int argc, const char* argv[])
{
	uint8_t keyscan, out_start;
	uint16_t card_base, in_base;
	// Set default card address.
	card_base = CSM_BASE_DEF;
	// Prepare screen.

	// Check command line parameters.
	if(argc==2)
	{
		if(strcmp(argv[1], "/?")==0)
		{
			// Print help.
			printUsage();
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
	else if(argc>2)
	{
		// Print help.
		printUsage();
		return 0;
	}

	keyscan = 0;
	while(keyscan!=KBD_ESC_CODE)
	{
		keyscan = processPageMain(card_base);
		if((keyscan=='r')||(keyscan=='R'))
		{
			processAYStdRegTable(card_base);
			keyscan = 0;
		}
		else if((keyscan=='e')||(keyscan=='E'))
		{
			processAYOvfRegTable(card_base);
			keyscan = 0;
		}
		else if((keyscan=='s')||(keyscan=='S'))
		{
			processSoundMuxTest(card_base);
			keyscan = 0;
		}
		else if((keyscan=='g')||(keyscan=='G'))
		{
			processGamepadTest(card_base);
			keyscan = 0;
		}
		else if((keyscan=='d')||(keyscan=='D'))
		{
			processAddressSpamTest(card_base);
			keyscan = 0;
		}
	}
	return 0;
}