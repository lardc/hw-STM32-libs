#ifndef __UTILS_H
#define __UTILS_H

// Include
#include "stdinc.h"

// Macro
#define ABS(a)				(((a) < 0) ? -(a) : (a))
#define MAX(a, b) 			(((a) > (b)) ? (a) : (b))
#define MIN(a, b) 			(((a) < (b)) ? (a) : (b))
#define SIGN(a)				(((a) >= 0) ? 1 : -1)

#endif // __UTILS_H
