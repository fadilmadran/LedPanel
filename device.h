/*
 * device.h
 *
 *  Created on: Aug 28, 2016
 *      Author: mehmet
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "stdint.h"
#include "stddef.h"


typedef struct
{
	uint8_t ClockDisplayFlag;
	uint8_t ClockDisplayInterval;
	uint8_t ClockDisplayDuration;
	uint8_t Padding;
} t_ClockData;


typedef struct
{
	uint8_t LightSensorFlag;
	uint8_t LightSensorVal;
	uint8_t LedBrightness;
	uint8_t Padding;
} t_LightData ;

typedef struct
{
		uint32_t CpuTemp;
		uint32_t OutsideTemp;
		uint32_t Panel1Current;
		uint32_t Panel2Current;
		uint32_t Panel3Current;
		uint32_t Panel4Current;
		t_LightData LightData;
		t_ClockData ClockData;
} t_DeviceInfo;

typedef struct
{
		t_LightData LightData;
		t_ClockData ClockData;
		uint32_t DataTime;
		uint32_t IPAddr;
		uint32_t Netmask;
		uint32_t GateWay;
		uint32_t Pad1;
		uint32_t Pad2;
} t_DeviceSet;

typedef struct
{
		uint8_t Ndx;
		uint8_t x;
		uint8_t y;
		uint8_t font;
		uint8_t scroll;
		uint8_t speed;
		uint16_t Pad;
		uint8_t Text[20];
		uint32_t Pad2;
} t_TextData;

typedef struct
{
	uint8_t Draw;
	uint32_t NextScroll;
	uint8_t ScrollCharNdx;
	uint8_t ScrollColNdx;

} t_TextInfo;

extern volatile uint8_t  DisplayMemChanged;
extern volatile uint8_t clockDisplay;
extern t_TextInfo TextInfo[3];
extern t_TextData TextDatas[3];
extern t_DeviceInfo DeviceInfo;
extern t_DeviceSet DeviceSet;

#endif /* DEVICE_H_ */
