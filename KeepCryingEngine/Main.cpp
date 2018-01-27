#include <stdlib.h>
#include <SDL.h>

#include "Application.h"

enum class main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char ** argv)
{
	ReportMemoryLeaks();

	int main_return = EXIT_FAILURE;
	main_states state = main_states::MAIN_CREATION;

	while(state != main_states::MAIN_EXIT)
	{
		switch(state)
		{
			case main_states::MAIN_CREATION:

				LOG_DEBUG("Application Creation --------------");
				App = new Application();
				state = main_states::MAIN_START;
				break;

			case main_states::MAIN_START:

				LOG_DEBUG("Application Init --------------");
				if(!App->Init())
				{
					LOG_DEBUG("Application Init exits with error -----");
					state = main_states::MAIN_EXIT;
				}
				else
				{
					state = main_states::MAIN_UPDATE;
					LOG_DEBUG("Application Update --------------");
				}

				break;

			case main_states::MAIN_UPDATE:
			{
				update_status update_return = App->Update();

				if(update_return == update_status::UPDATE_ERROR)
				{
					LOG_DEBUG("Application Update exits with error -----");
					state = main_states::MAIN_EXIT;
				}

				if(update_return == update_status::UPDATE_STOP)
					state = main_states::MAIN_FINISH;
			}
			break;

			case main_states::MAIN_FINISH:

				LOG_DEBUG("Application CleanUp --------------");
				if(!App->CleanUp())
				{
					LOG_DEBUG("Application CleanUp exits with error -----");
				}
				else
					main_return = EXIT_SUCCESS;

				state = main_states::MAIN_EXIT;

				break;

		}
	}

	RELEASE(App);
	LOG_DEBUG("Bye :)\n");
	return main_return;
}