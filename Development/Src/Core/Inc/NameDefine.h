
#ifndef REGISTER_NAME
	#define REGISTER_NAME(num, name)		NAME_##name = num,
	#define REGISTERING_ENUM
	enum EName {
#endif


REGISTER_NAME(	0,	None		)

REGISTER_NAME(	1,	Core		)
REGISTER_NAME(	2,	Engine		)
REGISTER_NAME(	3,	Editor		)
REGISTER_NAME(	4,	GamePlay	)

// Log messages
REGISTER_NAME(	10,	Log			)
REGISTER_NAME(	11,	Init		)
REGISTER_NAME(	12,	Exit		)
REGISTER_NAME(	13,	Warning		)
REGISTER_NAME(	14,	Error		)
REGISTER_NAME(	15,	Cmd			)
REGISTER_NAME(	16,	PackageLoad	)
REGISTER_NAME(	17,	PackageSave	)

// Misc
REGISTER_NAME(  18,	Color		)



#ifdef REGISTERING_ENUM
	};
	#undef REGISTER_NAME
	#undef REGISTERING_ENUM
#endif

