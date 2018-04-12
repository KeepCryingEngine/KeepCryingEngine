#include "ModuleLog.h"

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include "Application.h"
#include "ModuleEditorUI.h"

using namespace std;

struct ConsoleLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;

	void    Clear() 
	{ 
		Buf.clear(); 
		LineOffsets.clear(); 
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size);
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(title, p_open);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) ImGui::LogToClipboard();

		if (Filter.IsActive())
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != NULL; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
				if (Filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}
		}
		else
		{
			ImGui::TextUnformatted(Buf.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};

ModuleLog::ModuleLog()
{}

ModuleLog::~ModuleLog()
{}

bool ModuleLog::Start() 
{
	static ConsoleLog cLog;

	if (editorConsole == nullptr)
	{
		editorConsole = &cLog;
	}

	return true;
}

update_status ModuleLog::Update()
{
	if (consoleDraw)
	{
		editorConsole->Draw("Editor Console");
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleLog::ToggleConsole()
{
	consoleDraw ^= 1;
}

void ModuleLog::Log(const char* logMessage, logType lt)
{
	string separator = "\n";
	string logLabel;

	switch (lt)
	{
		case logType::ERROR_MESSAGE:
			logLabel = "[ERROR] - ";
			break;
		case logType::WARNING_MESSAGE:
			logLabel = "[WARNING] - ";
			break;
		case logType::INFO_MESSAGE:
			logLabel = "[INFO] - ";
			break;
		default:
			logLabel = "[UNIDENTIFIED] - ";
			break;
	}
	
	editorConsole->AddLog((logLabel + logMessage + separator).c_str(), ImGui::GetFrameCount());
}

bool ModuleLog::CleanUp()
{
	return true;
}

//------------------------------------------------------

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

void Log_Hooker(MonoString * message, logType type)
{
	char * logMessage = mono_string_to_utf8(message);
	App->log->Log(logMessage, type);
}
