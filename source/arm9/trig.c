/*---------------------------------------------------------------------------------

Copyright (C) 2005

Jason Rogers (dovoto)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.
Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source
distribution.

---------------------------------------------------------------------------------*/

#include <nds/arm9/trig_lut.h>
#include <nds/arm9/math.h>
#include <stdlib.h> //for bsearch()


/*
as far as the luts are concerned, an angle has 512 degrees (9 bits long) with an range of [0-511] (512 is the same as 0).
the luts then implement a quarter of a circle, cause its enough to calculate the rest with it.
*/

#define LUT_ANGLE_BITS			9
#define LUT_ANGLE				(1<<LUT_ANGLE_BITS)
#define LUT_ANGLE_MASK			(LUT_ANGLE-1)

#define LUT_QUARTER_ANGLE_BITS	7
#define LUT_QUARTER_ANGLE		(1<<LUT_QUARTER_ANGLE_BITS)
#define LUT_QUARTER_ANGLE_MASK	(LUT_QUARTER_ANGLE-1)


#define LUT_SIZE		LUT_QUARTER_ANGLE

//a libnds angle has 32768 degrees (16 bits long, signed) with an range of [-32768 - 32767]
//(32768, 0 and -32768 al describe the same angle)
#define LIBNDS_ANGLE_BITS		15
#define LIBNDS_ANGLE			(1<<LIBNDS_ANGLE_BITS)
#define LIBNDS_ANGLE_MASK		(LIBNDS_ANGLE-1)


//the number of bits that is used by a libnds angle but gets chopped off with a lut angle.
#define ANGLE_FRACTION_BITS (LIBNDS_ANGLE_BITS - LUT_ANGLE_BITS)
#define ANGLE_FRACTION		(1<<ANGLE_FRACTION_BITS)
#define ANGLE_FRACTION_MASK	(ANGLE_FRACTION-1)


#define LIBNDS_QUARTER_ANGLE (LUT_QUARTER_ANGLE << ANGLE_FRACTION_BITS)


/*
when getting an libnds angle, you should first convert it to a 15 bit unsigned number,
so you only have a positive angle and it doesn't affect the outcome (-270 degree == 90 degree)
*/
#define TO_POSITIVE_ANGLE(angle)	(((angle) < 0) ? (LIBNDS_ANGLE + (angle)) : (angle))

/*
to convert a libnds angle to a lut angle, you must shift it right so it fits in a lut angle and mask of any bits that doesn't fit:
*/
#define LIBNDS_TO_LUT_ANGLE(angle)	 (((angle) >> ANGLE_FRACTION_BITS) & LUT_ANGLE_MASK)


//the number of bits used for the fractional part.
#define SIN_BITSFRACTION 15
#define TAN_BITSFRACTION 16


//maximum tangent value = tan(89 + 63/64 degrees) << TAN_BITSFRACTION
#define MAX_TAN (400 << TAN_BITSFRACTION)



//the size of the luts are one bigger than normal for the compare functions used by bsearch.
//a 1.15 fixed point LUT
const u16 SIN_LUT[LUT_SIZE+1] = {
	0,	402,	804,	1206,	1607,	2009,	2410,	2811,	3211,
	3611,	4011,	4409,	4808,	5205,	5602,	5997,	6392,	6786,
	7179,	7571,	7961,	8351,	8739,	9126,	9512,	9896,	10278,
	10659,	11039,	11416,	11793,	12167,	12539,	12910,	13278,	13645,
	14010,	14372,	14732,	15090,	15446,	15800,	16151,	16499,	16846,
	17189,	17530,	17869,	18204,	18537,	18868,	19195,	19519,	19841,
	20159,	20475,	20787,	21097,	21403,	21706,	22005,	22301,	22594,
	22884,	23170,	23453,	23732,	24007,	24279,	24547,	24812,	25073,
	25330,	25583,	25832,	26077,	26319,	26557,	26790,	27020,	27245,
	27466,	27684,	27897,	28106,	28310,	28511,	28707,	28898,	29086,
	29269,	29447,	29621,	29791,	29956,	30117,	30273,	30425,	30572,
	30714,	30852,	30985,	31114,	31237,	31357,	31471,	31581,	31685,
	31785,	31881,	31971,	32057,	32138,	32214,	32285,	32351,	32413,
	32469,	32521,	32568,	32610,	32647,	32679,	32706,	32728,	32745,
	32758,	32765,	32768
};


//a 16.16 fixed point LUT
const s32 TAN_LUT[LUT_SIZE+1] = {
	0,	804,	1608,	2413,	3219,	4026,	4834,	5643,	6454,
	7267,	8083,	8900,	9721,	10544,	11371,	12201,	13035,	13874,
	14716,	15563,	16415,	17273,	18136,	19005,	19880,	20761,	21650,
	22545,	23449,	24360,	25280,	26208,	27145,	28092,	29050,	30017,
	30996,	31986,	32988,	34002,	35029,	36070,	37125,	38195,	39280,
	40382,	41500,	42635,	43789,	44962,	46155,	47369,	48604,	49862,
	51144,	52451,	53784,	55143,	56531,	57949,	59398,	60879,	62395,
	63946,	65535,	67164,	68834,	70548,	72307,	74115,	75974,	77886,
	79855,	81884,	83976,	86135,	88365,	90669,	93053,	95522,	98081,
	100736,	103492,	106358,	109340,	112446,	115687,	119071,	122609,	126313,
	130197,	134275,	138564,	143081,	147846,	152883,	158217,	163877,	169895,
	176309,	183160,	190499,	198380,	206869,	216043,	225990,	236816,	248647,
	261634,	275959,	291845,	309567,	329471,	351993,	377693,	407305,	441807,
	482534,	531351,	590957,	665398,	761030,	888449,	1066729,	1334015,	1779313,
	2669640,	5340085, MAX_TAN
};



s32 sinLutLookup(int i)
{
	//i = (i >> ANGLE_FRACTION_BITS) & 511;
	i = LIBNDS_TO_LUT_ANGLE(i);

	int lutVal = i & LUT_QUARTER_ANGLE_MASK;

	if(i < 128) return SIN_LUT[lutVal];				//first quarter
	if(i < 256) return SIN_LUT[LUT_SIZE - lutVal];	//second quarter
	if(i < 384) return -SIN_LUT[lutVal];			//thirth quarter
	return -SIN_LUT[LUT_SIZE - lutVal];				//fourth quarter
}


s16 sinLerp(s16 angle)
{
	angle = TO_POSITIVE_ANGLE(angle);

	s32 prev = sinLutLookup(angle);
	s32 next = sinLutLookup(angle + ANGLE_FRACTION);
	s32 interp = angle & ANGLE_FRACTION_MASK;

	int diff = next-prev;
	int interpolationresult = ((diff * interp) >> ANGLE_FRACTION_BITS);
	int result = prev + interpolationresult;

	//convert from .15 to .12 fixed point
	return result >> (SIN_BITSFRACTION - 12);
}


s16 cosLerp(s16 angle)
{
	return sinLerp(angle + LIBNDS_QUARTER_ANGLE); // Cos/sin symmetry
}

s32 tanLutLookup(int i)
{
	//convert from libnds to lut angle, and making sure the angle is in the first half of the circle
	i = (i >> ANGLE_FRACTION_BITS) & 255;

 	int lutVal = i & LUT_QUARTER_ANGLE_MASK;

	if(i == 128) return MAX_TAN;

 	if(i < 128) return TAN_LUT[lutVal];

 	return -TAN_LUT[LUT_SIZE - lutVal];
}


s32 tanLerp(s16 angle)
{
	angle = TO_POSITIVE_ANGLE(angle);
	//s32 lut_val = (angle >> ANGLE_FRACTION_BITS) & 511;
	int lutVal = LIBNDS_TO_LUT_ANGLE(angle);

	s32 prev = ((lutVal == 128 || lutVal == 384) ? -MAX_TAN: tanLutLookup(angle));
	s32 next = tanLutLookup(angle + ANGLE_FRACTION);
	s32 interp = angle & ANGLE_FRACTION_MASK;


	int diff = next-prev;
	int interpolationresult = ((diff * interp) >> ANGLE_FRACTION_BITS);
	int result = prev + interpolationresult;

	return (result >> (TAN_BITSFRACTION - 12));
}


int asinComp(const void *a, const void *b)
{
	u16 par = (*(u16*)a);
	u16* lut = (u16*)b;

	if(par == lut[0] || (par > lut[0] && par < lut[1]))
	{
		return 0;
	}

	if(par < lut[0])
	{
		return -1;
	}
	return 1;
}


s16 asinLerp(s16 par)
{
	bool neg = false;
	u16 param;

	if(par < 0) {
		param = -par;
		neg = true;
	} else {
		param = par;
	}

	//convert from 4.12 to 1.15
	param = param << (SIN_BITSFRACTION - 12);

	if(param < ANGLE_FRACTION)
		return 0;

	if(param > SIN_LUT[LUT_SIZE])
	{
		return (neg ? -LIBNDS_QUARTER_ANGLE : LIBNDS_QUARTER_ANGLE);
	}

	u16* lutIndexPointer = (u16*)bsearch(&param, SIN_LUT, LUT_SIZE + 1, sizeof(u16), asinComp);

	if(lutIndexPointer == NULL)
		return 0;

	int index = (int) (lutIndexPointer-SIN_LUT);
	//index is in (0, 256)

	int angle = intToFixed(index, ANGLE_FRACTION_BITS);

	return (neg ? -angle : angle);
}


s16 acosLerp(s16 par)
{
	//Uses the identity sin(x) = cos(90 - x) => asin(x) = 90 - acos(x)
	return LIBNDS_QUARTER_ANGLE - asinLerp(par); // returns a value in [0, 256]
}


int atanComp(const void *a, const void *b)
{
	s32 par = (*(s32*)a);
	s32* lut = (s32*)b;

	if(par == lut[0] || (par > lut[0] && par < lut[1]))
	{
		return 0;
	}

	if(par < lut[0])
	{
		return -1;
	}
	return 1;
}



//todo: implement and test

//#define inttof32(a) intToFixed((a), 12)

/*
s32 atanLerp(s32 par)
{
	bool neg = false;
	if(par < 0)
	{
		par = -par;
		neg = true;
	}

	/ *
	If the parameter is less than the second value in the LUT, we can be
	assured the binary search will return the first, which is zero. This
	zero will cause a divide by zero later, so this saves the trouble of
	dealing with that.
	// * /
	if(par < TAN_LUT[1])
	{
		return 0;
	}

	/ *
	If the parameter is greater than the maximum value the LUT supports,
	we can be assured the binary search may crap out on us as atanComp
	doesn't run bounds checking. It is faster to add this than fix that
	(in terms of runtime speed, I'm not just being lazy).
	* /
	if(par >= TAN_LUT[LUT_SIZE])
	{
		return inttof32(128);
	}

	s32* at = (s32*)bsearch(&par, TAN_LUT, LUT_SIZE, sizeof(s32), atanComp);

	if(at == NULL)
	{
		at = TAN_LUT+LUT_SIZE;
		neg = !neg;
	}

	s32 angle = divf32(mulf32(inttof32(at - TAN_LUT), par), (*at));


	// returns a value in [-128, 128]
	return (neg ? -angle :angle);
}
//*/
