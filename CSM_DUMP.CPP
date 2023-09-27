#include "CSM_DUMP.H"

#ifdef __cplusplus
	#define __CPPARGS	...
#else
	#define __CPPARGS
#endif

// Sample sequence for DAC.
uint8_t pcm_sequence[PCM_SEQ_SIZE] =
{
	0x00,
	0x40,
	0x80,
	0xC0,
	0xFF,
	0x80
};

uint8_t dma1_seq[DMA1_SEQ_SIZE] =
{
  0x80, 0x80, 0x80, 0x80, 0x7d, 0x75, 0x80, 0x95, 0xa0, 0x99, 0x5a, 0x53,
  0x6a, 0x83, 0x9c, 0x71, 0x6d, 0x9a, 0xaf, 0xaf, 0x7f, 0x53, 0x53, 0x7e,
  0x7f, 0x73, 0x6e, 0x87, 0xaf, 0xb0, 0x8c, 0x6b, 0x63, 0x76, 0x7f, 0x74,
  0x5c, 0x74, 0x95, 0xaa, 0xaf, 0x7f, 0x71, 0x7e, 0x7a, 0x7e, 0x5f, 0x5a,
  0x77, 0xbc, 0xc7, 0xc2, 0xb2, 0xa9, 0xcb, 0xb5, 0xa3, 0x88, 0x86, 0xa1,
  0xc2, 0xc3, 0xac, 0xaa, 0xb7, 0xc0, 0xb8, 0x90, 0x8f, 0x93, 0xaa, 0xba,
  0x9e, 0x94, 0xa4, 0xa3, 0x9b, 0x73, 0x4e, 0x52, 0x61, 0x68, 0x69, 0x5b,
  0x59, 0x68, 0x6a, 0x58, 0x45, 0x38, 0x45, 0x65, 0x6c, 0x73, 0x77, 0x7c,
  0x9b, 0xa3, 0x76, 0x63, 0x76, 0x88, 0x93, 0x90, 0x90, 0xa6, 0xbe, 0xd2,
  0xcc, 0xb4, 0xa5, 0xa0, 0x9a, 0x85, 0x75, 0x67, 0x8a, 0xad, 0x9b, 0x85,
  0x70, 0x67, 0x6b, 0x5c, 0x40, 0x3d, 0x48, 0x62, 0x77, 0x6a, 0x61, 0x64,
  0x68, 0x67, 0x5b, 0x44, 0x45, 0x59, 0x74, 0x79, 0x6f, 0x69, 0x78, 0x7e,
  0x6e, 0x59, 0x3f, 0x40, 0x51, 0x4e, 0x40, 0x35, 0x46, 0x52, 0x42, 0x23,
  0x0d, 0x0d, 0x0f, 0x17, 0x1c, 0x21, 0x34, 0x46, 0x4f, 0x4d, 0x3d, 0x35,
  0x3d, 0x49, 0x4c, 0x4c, 0x49, 0x5e, 0x6f, 0x72, 0x67, 0x5a, 0x55, 0x55,
  0x50, 0x49, 0x44, 0x46, 0x5d, 0x70, 0x70, 0x6d, 0x6e, 0x72, 0x7d, 0x80,
  0x7c, 0x7d, 0x92, 0xaa, 0xb7, 0xb7, 0xb2, 0xba, 0xc5, 0xc5, 0xbd, 0xb8,
  0xc3, 0xda, 0xea, 0xe1, 0xde, 0xd5, 0xdd, 0xda, 0xc7, 0xb8, 0xaf, 0xb8,
  0xc5, 0xbf, 0xb5, 0xb5, 0xb5, 0xb9, 0xb2, 0xa4, 0x99, 0x9d, 0xad, 0xb5,
  0xb2, 0xae, 0xb7, 0xc4, 0xc5, 0xbe, 0xbc, 0xc0, 0xcb, 0xd1, 0xce, 0xd3,
  0xd6, 0xd9, 0xe3, 0xdf, 0xd4, 0xca, 0xc5, 0xc7, 0xc2, 0xbc, 0xb5, 0xb7,
  0xbe, 0xbe, 0xaf, 0xa1, 0xa2, 0xa2, 0xa5, 0x9e, 0x98, 0x9d, 0xa7, 0xb0,
  0xb5, 0xaf, 0xaa, 0xb0, 0xb4, 0xb1, 0xae, 0xad, 0xb7, 0xc2, 0xc5, 0xbd,
  0xb5, 0xaf, 0xa8, 0x9b, 0x8b, 0x85, 0x7e, 0x80
};

uint8_t dma3_seq[DMA3_SEQ_SIZE] =
{
  0x80, 0x80, 0x7f, 0x80, 0x83, 0x85, 0x8d, 0x7a, 0x72, 0x7b, 0x7d, 0x88,
  0x8c, 0x9d, 0x72, 0x65, 0x76, 0x7b, 0x8d, 0x93, 0xac, 0x6b, 0x58, 0x72,
  0x78, 0x91, 0x9a, 0xbb, 0x64, 0x4b, 0x6e, 0x76, 0x96, 0xa0, 0xca, 0x5d,
  0x3e, 0x69, 0x74, 0x9b, 0xa7, 0xda, 0x56, 0x31, 0x65, 0x71, 0x9f, 0xae,
  0xe9, 0x4f, 0x25, 0x60, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f,
  0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f,
  0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4,
  0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4,
  0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f,
  0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b,
  0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4,
  0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f,
  0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f,
  0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4,
  0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4,
  0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f,
  0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b,
  0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4,
  0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f,
  0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f,
  0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4,
  0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4,
  0xb4, 0xf4, 0x4b, 0x1f, 0x5f, 0x6f, 0xa4, 0xb3, 0xf0, 0x4e, 0x26, 0x62,
  0x71, 0xa0, 0xad, 0xe2, 0x54, 0x31, 0x66, 0x73, 0x9c, 0xa7, 0xd4, 0x5b,
  0x3d, 0x6a, 0x75, 0x97, 0xa1, 0xc7, 0x61, 0x48, 0x6e, 0x77, 0x93, 0x9b,
  0xb9, 0x67, 0x54, 0x72, 0x79, 0x8f, 0x94, 0xab, 0x6e, 0x5f, 0x76, 0x7b,
  0x8b, 0x8e, 0x9e, 0x74, 0x6b, 0x7a, 0x7d, 0x87, 0x88, 0x90, 0x7a, 0x77,
  0x7e, 0x7f, 0x82, 0x80, 0x80, 0x80
};


uint16_t card_base;
uint32_t old_irq_ctrl;
uint16_t int3cnt;
uint16_t int7cnt;

void interrupt (*old_irq3)(__CPPARGS);
void interrupt (*old_irq7)(__CPPARGS);

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
uint8_t detectAYType(uint16_t in_port, uint8_t *detect_stage)
{
	uint8_t io_data;
	// Reset all registers.
	resetAY(in_port);
	// Write some bit-patterns to test PSG presence, data bus and registers integrity.
	writeAYReg(in_port, AY_R0, 0xA5);	// REG 0b00000000, DATA 0b10100101
	writeAYReg(in_port, AY_RC, 0x5A);	// REG 0b00001100, DATA 0b01011010
	writeAYReg(in_port, AY_R3, 0x0F);	// REG 0b00000011, DATA 0b00001111
	(*detect_stage) = 0;
	// Read first pattern.
	io_data = readAYReg(in_port, AY_R0);
	if(io_data!=0xA5)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		(*detect_stage) = 1;
		return PSG_NONE;
	}
	// Read second pattern.
	io_data = readAYReg(in_port, AY_RC);
	if(io_data!=0x5A)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		(*detect_stage) = 2;
		return PSG_NONE;
	}
	// Read third pattern.
	io_data = readAYReg(in_port, AY_R3);
	if(io_data!=0x0F)
	{
		// Read byte does not match written one.
		resetAY(in_port);
		(*detect_stage) = 3;
		return PSG_NONE;
	}
	(*detect_stage) = 4;
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
	uint8_t detect_stage;
	// Detect PSG type.
	temp = detectAYType(in_port, &detect_stage);
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
		// Fill up all registers.
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
	printf("by Maksim Kryukov aka Fagear (fagear@mail.ru), 2023\n\r");
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
	uint8_t mix_ctrl, port_ctrl, volume_ctrl, pcm_idx;
	uint8_t reg1, reg2, reg3, reg4;
	uint32_t buf_adr;
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
	cprintf("   SOUND");
	normvideo();
	printf(" [C]");
	gotoxy(1, out_start+9);
	printf("Master volume:      ");
	highvideo();
	cprintf("    100%");
	normvideo();
	printf(" [-][+]");
	highvideo();
	// Zero out all AY registers.
	resetAY(card_base);
	test_bits = 0;
	// Preset frequencies and amplitudes.
	mix_ctrl = 0xFF;								// Turn off all channels
	port_ctrl = AY_IOB_DMA_DIS|AY_IOB_IRQ_DIS|AY_IOB_C_OUT;
	volume_ctrl = 0xFF;								// Full amplitude on output
	pcm_idx = 0;									// Set first sample in the sequence
	writeAYReg(card_base, AY_REG_A_FREQ_FINE, 75);	// Preset ~1500Hz (@1.79 MHz) for CH A
	writeAYReg(card_base, AY_REG_A_LVL, 0x0F);		// Preset maximum amplitude CH A
	writeAYReg(card_base, AY_REG_B_FREQ_FINE, 112);	// Preset ~1000Hz (@1.79 MHz) for CH B
	writeAYReg(card_base, AY_REG_B_LVL, 0x0F);		// Preset maximum amplitude CH B
	//writeAYReg(card_base, AY_REG_C_FREQ_FINE, 224);	// Preset ~500Hz (@1.79 MHz) for CH C
	writeAYReg(card_base, AY_REG_C_FREQ_FINE, 10);
	writeAYReg(card_base, AY_REG_C_LVL, 0x0F);		// Preset maximum amplitude CH C
	writeAYReg(card_base, AY_REG_MIXER, mix_ctrl);	// Apply AY mixer settings
	writeAYReg(card_base, AY_REG_IO_A, volume_ctrl);// Turn volume up (via LM13600, connected to AY IO Port A)
	writeAYReg(card_base, AY_REG_IO_B, port_ctrl);	// Apply AY IO port B (mux control)

	keyscan = 0;
	// Wait for keypress.
	while(keyscan!=KBD_ESC_CODE)
	{
		gotoxy(40, out_start+8);
		cprintf("IRQ3 count: %03u, IRQ7 count: %03u", int3cnt, int7cnt);

		/*buf_adr = FP_SEG(dma1_seq);
		buf_adr = (buf_adr<<4);
		buf_adr += FP_OFF(dma1_seq);

		outportb(DMA_03REG_RST, DUMMY_WRITE);
		reg1 = inportb(DMA_03REG_CH1ADR);
		reg2 = inportb(DMA_03REG_CH1ADR);
		outportb(DMA_03REG_RST, DUMMY_WRITE);
		reg3 = inportb(DMA_03REG_CH1CNT);
		reg4 = inportb(DMA_03REG_CH1CNT);

		gotoxy(40, out_start+9);
		cprintf("Buf: 0x%x 0x%x 0x%08x",
				FP_SEG(dma1_seq),
				FP_OFF(dma1_seq),
				buf_adr);
		gotoxy(40, out_start+10);
		cprintf("DMA adr: 0x%02x 0x%02x, DMA cnt: 0x%02x, 0x%02x",
				reg1,
				reg2,
				reg3,
				reg4);*/


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
				mix_ctrl&=~AY_A_TONE_EN;
				gotoxy(21, out_start+1);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHA;
				mix_ctrl|=AY_A_TONE_EN;
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
				mix_ctrl&=~AY_B_TONE_EN;
				gotoxy(21, out_start+2);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHB;
				mix_ctrl|=AY_B_TONE_EN;
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
				mix_ctrl&=~AY_C_TONE_EN;
				gotoxy(21, out_start+3);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_CHC;
				mix_ctrl|=AY_C_TONE_EN;
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
				port_ctrl|=AY_IOB_MIX_MON;
				gotoxy(21, out_start+7);
				cprintf(" ENABLED");
			}
			else
			{
				test_bits&=~TST_MONO;
				port_ctrl&=~AY_IOB_MIX_MON;
				gotoxy(21, out_start+7);
				cprintf("DISABLED");
			}
		}
		else if((keyscan=='c')||(keyscan=='C'))
		{
			// Channel C switch toggle.
			if((test_bits&TST_CDMA)==0)
			{
				test_bits|=TST_CDMA;
				port_ctrl&=~(AY_IOB_C_OUT|AY_IOB_DMA_DIS|AY_IOB_IRQ_DIS);
				gotoxy(21, out_start+8);
				cprintf(" DMA REQ");
			}
			else
			{
				test_bits&=~TST_CDMA;
				port_ctrl|=AY_IOB_C_OUT|AY_IOB_DMA_DIS|AY_IOB_IRQ_DIS;
				gotoxy(21, out_start+8);
				cprintf("   SOUND");
			}
		}
		else if(keyscan=='-')
		{
			// Volume down.
			if(volume_ctrl==VOL_100)
			{
				volume_ctrl = VOL_075;
				gotoxy(21, out_start+9);
				cprintf("     75%");
			}
			else if(volume_ctrl==VOL_075)
			{
				volume_ctrl = VOL_050;
				gotoxy(21, out_start+9);
				cprintf("     50%");
			}
			else if(volume_ctrl==VOL_050)
			{
				volume_ctrl = VOL_025;
				gotoxy(21, out_start+9);
				cprintf("     25%");
			}
			else
			{
				volume_ctrl = VOL_000;
				gotoxy(21, out_start+9);
				cprintf("      0%");
			}
		}
		else if(keyscan=='+')
		{
			// Volume up.
			if(volume_ctrl==VOL_000)
			{
				volume_ctrl = VOL_025;
				gotoxy(21, out_start+9);
				cprintf("     25%");
			}
			else if(volume_ctrl==VOL_025)
			{
				volume_ctrl = VOL_050;
				gotoxy(21, out_start+9);
				cprintf("     50%");
			}
			else if(volume_ctrl==VOL_050)
			{
				volume_ctrl = VOL_075;
				gotoxy(21, out_start+9);
				cprintf("     75%");
			}
			else
			{
				volume_ctrl = VOL_100;
				gotoxy(21, out_start+9);
				cprintf("    100%");
			}
		}
		if(keyscan=='4')
		{
			pcm_idx++;
			if(pcm_idx>=PCM_SEQ_SIZE)
			{
				pcm_idx = 0;
			}
			outportb(card_base+CSM_PCM1, pcm_sequence[pcm_idx]);
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
			pcm_idx++;
			if(pcm_idx>=PCM_SEQ_SIZE)
			{
				pcm_idx = 0;
			}
			outportb(card_base+CSM_PCM2, pcm_sequence[pcm_idx]);
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
	// Print PSG type.
	printAYType(card_base);
	resetAY(card_base);
	// Print help.
	printf("\n\r\n\r[Esc]: back to main menu\n\r\n\r");
	printf("Single sub-port addressing test.\n\r");
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
			outportb(card_base+CSM_IRQ_CLR, DUMMY_WRITE);
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

// Setup DMA channels.
void setupDMAChannels()
{
	uint32_t buf_addr;
	uint16_t buf_len;
	// Get buffer address and length.
	buf_addr = FP_SEG(dma1_seq);
	buf_addr = (buf_addr<<4) + FP_OFF(dma1_seq);
	buf_len = DMA1_SEQ_SIZE;
	// 8237 setup.
	// Mask DMA channel 1.
	outportb(DMA_03REG_MASK, (DMA_MASK_EN|DMA_CH1_SEL));
	// Mask DMA channel 3.
	outportb(DMA_03REG_MASK, (DMA_MASK_EN|DMA_CH3_SEL));
	// Enable Single transfer forward reading DMA on channel 1.
	outportb(DMA_03REG_MODE, (DMA_MODE_SGL|DMA_MODE_AUTO|DMA_MODE_RD|DMA_CH1_SEL));
	// Set segment page address.
	outportb(DMA_03REG_CH1PG, (uint8_t)(buf_addr>>16));
	// Set buffer address.
	outportb(DMA_03REG_RST, DUMMY_WRITE);
	outportb(DMA_03REG_CH1ADR, (uint8_t)buf_addr);
	outportb(DMA_03REG_CH1ADR, (uint8_t)(buf_addr>>8));
	// Set buffer length.
	outportb(DMA_03REG_RST, DUMMY_WRITE);
	outportb(DMA_03REG_CH1CNT, (uint8_t)buf_len);
	outportb(DMA_03REG_CH1CNT, (uint8_t)(buf_len>>8));
	// Enable DMA on channel 1.
	outportb(DMA_03REG_MASK, DMA_CH1_SEL);
	// Acknowledge interrupt.
	outportb(IRQ_CMD_BASE, IRQ_ACK_INT);
}

// Temporary handler for IRQ3.
void interrupt csm_irq3(__CPPARGS)
{
	uint8_t temp_reg;
	int3cnt++;
	// Clear IRQ latch in CSM.
	outportb(card_base+CSM_IRQ_CLR, DUMMY_WRITE);
	// Acknowledge interrupt.
	outportb(IRQ_CMD_BASE, IRQ_ACK_INT);

	temp_reg = readAYReg(card_base, AY_REG_IO_B);
	temp_reg |= (AY_IOB_DMA_DIS|AY_IOB_C_OUT);
	writeAYReg(card_base, AY_REG_IO_B, temp_reg);

	temp_reg = readAYReg(card_base, AY_REG_MIXER);
	temp_reg |= (AY_C_TONE_EN);
	writeAYReg(card_base, AY_REG_MIXER, temp_reg);
}

// Temporary handler for IRQ7.
void interrupt csm_irq7(__CPPARGS)
{
	int7cnt++;
	// Clear IRQ latch in CSM.
	outportb(card_base+CSM_IRQ_CLR, DUMMY_WRITE);
	// Acknowledge interrupt.
	outportb(IRQ_CMD_BASE, IRQ_ACK_INT);
}

// Replace IRQ handlers used by CSM for testing.
void saveIntHandlers()
{
	// Disable interrupts.
	disable();
	// Save IRQ control state.
	old_irq_ctrl = inport(IRQ_CTRL_BASE);
	old_irq3 = getvect(ISA_IRQ3);	// Save IRQ3 handler.
	old_irq7 = getvect(ISA_IRQ7);	// Save IRQ7 handler.
	// Set new handlers.
	setvect(ISA_IRQ3, csm_irq3);
	setvect(ISA_IRQ7, csm_irq7);
	// Clear IRQ flag.
	outportb(card_base+CSM_IRQ_CLR, DUMMY_WRITE);
	// Allow IRQs for CSM.
	outportb(IRQ_CTRL_BASE, old_irq_ctrl&(~(ISA_IRQ3_MASK|ISA_IRQ7_MASK)));
	// Enable interrupts.
	enable();
}

// Restore original IRQ handlers after testing.
void restoreIntHandlers()
{
	// Disable interrupts.
	disable();
	// Restore original handlers.
	setvect(ISA_IRQ3, old_irq3);
	setvect(ISA_IRQ7, old_irq7);
	// Restore IRQ control state.
	outportb(IRQ_CTRL_BASE, old_irq_ctrl);
	// Enable interrupts.
	enable();
}


// Main function.
int main(int argc, const char* argv[])
{
	uint8_t keyscan, out_start;
	uint16_t in_base;

	// Reset INT counters.
	int3cnt = int7cnt = 0;

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
		// Main menu.
		keyscan = processPageMain(card_base);
		if((keyscan=='r')||(keyscan=='R'))
		{
			// Normal register list.
			processAYStdRegTable(card_base);
			keyscan = 0;
		}
		else if((keyscan=='e')||(keyscan=='E'))
		{
			// Out-of-bound and full fill register list.
			processAYOvfRegTable(card_base);
			keyscan = 0;
		}
		else if((keyscan=='s')||(keyscan=='S'))
		{
			// Setup DMA queue.
			setupDMAChannels();
			// Save current interrupt vectors.
			saveIntHandlers();
			// Sound channels and muxer/amplifier test.
			processSoundMuxTest(card_base);
			// Restore interrupt handlers.
			restoreIntHandlers();
			keyscan = 0;
		}
		else if((keyscan=='g')||(keyscan=='G'))
		{
			// Gamepad read test.
			processGamepadTest(card_base);
			keyscan = 0;
		}
		else if((keyscan=='d')||(keyscan=='D'))
		{
			// Single internal device register I/O test.
			processAddressSpamTest(card_base);
			keyscan = 0;
		}
	}
	return 0;
}