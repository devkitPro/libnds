#ifndef __TOUCH_H__
#define __TOUCH_H__
/*! \struct touchPosition
	\brief holds data related to the touch screen
*/
typedef struct {
	u16	rawx; //!< Raw x value from the A2D
	u16	rawy; //!< Raw x value from the A2D
	u16	px;   //!< Processes pixel X value
	u16	py;   //!< Processes pixel Y value
	u16	z1;   //!< Raw cross panel resistance
	u16	z2;   //!< Raw cross panel resistance
} touchPosition;

#endif
