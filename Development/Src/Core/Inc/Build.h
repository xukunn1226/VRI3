#ifndef VRI3_CORE_BUILD_H_
#define VRI3_CORE_BUILD_H_

#if SHIPPING
	#define DO_CHECK	0
	#define	STATS		0
#else
	#define DO_CHECK	1
	#define	STATS		1
#endif

#endif