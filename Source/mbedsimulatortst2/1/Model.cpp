#include "ModelImpl.h"
#include "Model.h"
using namespace ModelImpl;

namespace Model
{
	// no dynamic allocation

	const SYNCHRONIZATION_CHANNEL_DATA CHANNEL_1_DATA =
	{
		DBGSTR("X"),		/* name */
		false,				/* urgent */
		false,				/* broadcast */
	};
	SYNCHRONIZATION_CHANNEL CHANNEL_1 =
	{
		&CHANNEL_1_DATA,
		NULL,
		NULL,
	};

	const SYNCHRONIZATION_CHANNEL_DATA CHANNEL_2_DATA =
	{
		DBGSTR("Y"),
		false,
		false,				/* broadcast */
	};
	SYNCHRONIZATION_CHANNEL CHANNEL_2 =
	{
		&CHANNEL_2_DATA,
		NULL,
		NULL,
	};


	const TRANSITION_ENTRY* const STATE_1_OUT_TABLE[] =
	{
		&TRANSITION_1,		
		NULL		
	};

	const TRANSITION_ENTRY* const STATE_2_OUT_TABLE[] =
	{
		&TRANSITION_2,
		NULL
	};

	const STATE_ENTRY_DATA STATE_1_DATA =
	{
		DBGSTR("Template_StateA"),				 /* name */
		&STATE_1_OUT_TABLE,  /* transitions */
		StateNormal,								 /* mode */
	};

	STATE_ENTRY STATE_1 =
	{
		&STATE_1_DATA,
		NULL,
		NULL,
	};

	const STATE_ENTRY_DATA STATE_2_DATA =
	{
		DBGSTR("Template_StateA"),
		(TRANSITION_ENTRY* const*)&STATE_2_OUT_TABLE,
		StateNormal,								 /* mode */
	};

	STATE_ENTRY STATE_2 =
	{
		&STATE_2_DATA,
		NULL,
		NULL,
	};

	const TRANSITION_ENTRY_DATA TRANSITION_1_DATA =
	{
		DBGSTR("Template_StateA_StateB_1"),		/* name */
		&STATE_2,								/* target */
	};

	TRANSITION_ENTRY TRANSITION_1 = 
	{
		&TRANSITION_1_DATA,
		NULL, 		/* void(*transition_executed)(TRANSITION_ENTRY* me); */
		NULL,
		NULL		/* void* context; */
	};

	const TRANSITION_ENTRY_DATA TRANSITION_2_DATA =
	{
		DBGSTR("Template_StateB_StateA_1"),
		&STATE_1,
	};
	TRANSITION_ENTRY TRANSITION_2 =
	{
		&TRANSITION_2_DATA,
		NULL, 		/* void(*transition_executed)(TRANSITION_ENTRY* me); */
		NULL,
		NULL		/* void* context; */
	};


	STATE_ENTRY* const PROCESS_1_STATES[] =
	{
		&STATE_1,
		&STATE_2,
		NULL
	};

	extern PROCESS_ENTRY_STATE PROCESS_1_STATE;
	PROCESS_ENTRY_THREAD PROCESS_1_THREAD(&PROCESS_1_STATE);

	const PROCESS_ENTRY_DATA PROCESS_1_DATA
	{
		DBGSTR("Template"),			/* name */
		(STATE_ENTRY* const*)PROCESS_1_STATES, /* states */
		&STATE_1,	/* initstate */
		&PROCESS_1_THREAD, /* threadptr */
	};

	PROCESS_ENTRY PROCESS_1
	{
		&PROCESS_1_DATA,
		NULL, /* void(*process_start)(PROCESS_ENTRY* me); */
		NULL, /* void* context; */
	};

	PROCESS_ENTRY_STATE PROCESS_1_STATE = 
	{
		&PROCESS_1,
		&PROCESS_1_THREAD,
		&PROCESS_1_DATA,
		NULL,
		NULL,
		NULL,
		false,
		ProcessInit, /* running */
	};

	PROCESS_ENTRY_STATE* const PROCESS_LIST[] =
	{
		&PROCESS_1_STATE,
		NULL
	};


}

namespace ModelImpl
{
	volatile PROCESS_ENTRY_STATE* const* PROCESS_LIST = ::Model::PROCESS_LIST;
}
