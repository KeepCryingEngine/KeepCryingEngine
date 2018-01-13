#ifndef _MEMLEAKS_H_
#define _MEMLEAKS_H_

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#ifdef _DEBUG
		#ifndef DBG_NEW
			#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
			#define new DBG_NEW
		#endif
	#endif  // _DEBUG

	#define ReportMemoryLeaks() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#endif // !_MEMLEAKS_H_