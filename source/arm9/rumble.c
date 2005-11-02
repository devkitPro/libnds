#include <nds.h>
#include <nds/jtypes.h>
#include <nds/arm9/rumble.h>

//---------------------------------------------------------------------------------
void sysSetCartOwner(bool arm9)
//---------------------------------------------------------------------------------
{
	WAIT_CR = (WAIT_CR & ~0x0080) | (arm9 ? 0 : 0x80);
}

//---------------------------------------------------------------------------------
bool isRumbleInserted(void)
//---------------------------------------------------------------------------------
{
	uint16 four[4];

	sysSetCartOwner(true);

	// First, check for 0x96 to see if it's a GBA game
	if (GBA_HEADER.is96h == 0x96)
	{
		return false;
	} else {
		// Now check to see if it's true open bus, or if D1 is pulled low
		four[0] = GBA_BUS[0] & 0xFF;
		four[1] = GBA_BUS[1] & 0xFF;
		four[2] = GBA_BUS[2] & 0xFF;
		four[3] = GBA_BUS[3] & 0xFF;

		return (four[0] == 0x00) && (four[2] == 0x00) && (four[1] == 0x01) && (four[3] == 0x01);
	}
}

//---------------------------------------------------------------------------------
void setRumble(bool position)
//---------------------------------------------------------------------------------
{
	RUMBLE_PAK = (position ? 2 : 0);
}
