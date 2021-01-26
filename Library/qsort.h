#ifndef __QSORT_H
#define __QSORT_H

#include "stdinc.h"
#include "ysizet.h"

void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));

#endif // __QSORT_H
