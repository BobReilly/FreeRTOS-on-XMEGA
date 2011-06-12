/*
 * ledGroup.c
 *
 *  Created on: 12.06.2011
 *      Author: Yuriy
 */

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "port_driver.h"
#include "ledGroup.h"

/* Allocates memory for the led group with specified amount of leds */
LedGroup * ledGroupInitialize (short amountOfLedsInGroup){
	LedGroup * ledGroup = pvPortMalloc(sizeof(LedGroup));
	ledGroup->amountOfLedsInGroup = 0;
	ledGroup->pins=pvPortMalloc(sizeof(Pin)*amountOfLedsInGroup);
	return ledGroup;
}
/*
 * Adds new led to group. Note that there is important to add
 * leds in the order, because they will be addressed by index.
 * Only 8 leds are supported.
 * Special case for RGB - add Red, then Green, then Blue.
 * Special case for RG - add Red, then Green
 * Returns 1 if success.
 */
short ledGroupAdd(LedGroup * ledGroup, PORT_t * port, uint8_t bitmask, short isActiveLow){
	if ((ledGroup->amountOfLedsInGroup)<8)
	{
		short index = ledGroup->amountOfLedsInGroup;
		//store pin information
		ledGroup->pins[index].port=port;
		ledGroup->pins[index].bitmask=bitmask;
		// Configures pin on the port
		PORT_ConfigurePins(port, bitmask, false, isActiveLow, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc );
		// Configure pin output
		//PORT_SetDirection(port, bitmask);
		PORT_SetPinsAsOutput(port, bitmask);
		//we have added one more led to the group
		ledGroup->amountOfLedsInGroup=(ledGroup->amountOfLedsInGroup)+1;
	} else {
		return 0;
	}
	return 1;
}

/*
 * Sets leds shining according to the bitmask. If you have 8 leds and want
 * leds 1,5 and 8 shining, pass bitmask '10010001'=0x91.
 * It is possible to use Color_enum instead of the bitmask
 */
void ledSet(LedGroup * ledGroup , uint8_t bitmask)
{
	for (int i=0; i<(ledGroup->amountOfLedsInGroup);i++){
		/* bitmask>>i shifts bitmask to the left, effectively placing bit
		 * number i to the 0x01 position. Than clear all other bits and
		 * see if this bit was 1
		 */
		if ((bitmask>>i)&0x01){
			PORT_SetPins(ledGroup->pins[i].port, ledGroup->pins[i].bitmask);
		} else {
			PORT_ClearPins(ledGroup->pins[i].port, ledGroup->pins[i].bitmask);
		}
	}
}
