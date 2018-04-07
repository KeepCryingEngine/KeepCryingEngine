#ifndef _MODULELOG_H_
#define _MODULELOG_H_

#include "Module.h"

struct ConsoleLog;

enum logType
{
	ERROR_MESSAGE,
	WARNING_MESSAGE,
	INFO_MESSAGE
};

class ModuleLog : public Module
{
public:
	ModuleLog();
	~ModuleLog();

	update_status Update() override;
	bool CleanUp() override;
	
	void ModuleLog::ToggleConsole();

	void ModuleLog::Log(const char* logMessage, logType lt);

private:
	ConsoleLog * editorConsole = nullptr;
	bool consoleDraw = true;
};

#endif // !_MODULELOG_H_

