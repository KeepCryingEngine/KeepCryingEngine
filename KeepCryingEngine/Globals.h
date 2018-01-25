#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "MemLeaks.h"

#define LOG_DEBUG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum class update_status
{
	UPDATE_CONTINUE,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Useful typedefs ---------
typedef unsigned int uint;

// Deletes a buffer
template<typename T>
inline void RELEASE(T*& x)
{
	if(x)
	{
		delete x;
		x = nullptr;
	}
}

// Deletes an array of buffers
template<typename T>
inline void RELEASE_ARRAY(T*& x)
{
	if(x)
	{
		delete[] x;
		x = nullptr;
	}
}

// Configuration -----------


#endif // !_GLOBALS_H_