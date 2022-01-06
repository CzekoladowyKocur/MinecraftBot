#include "include/Application.h"
#ifdef APIENTRY
#undef APIENTRY
#endif

INT WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	INT nCmdShow)
{
	Application* application = new Application();
	if (application->Initialize())
	{
		if (application->Run())
		{
			if (application->Shutdown())
			{
			}
			else
			{
				delete application;
				printf("Failed to properly shutdown the application\n");
				return EXIT_FAILURE;
			} /* Shutdown */
		}
		else
		{
			delete application;
			printf("An error has occured while running the application\n");
			return EXIT_FAILURE;
		} /* Run */
	}
	else
	{
		delete application;
		printf("Failed to initialize application\n");
		return EXIT_FAILURE;
	} /* Initialize */

	delete application;
	printf("Shutting down. . .\n");
	return EXIT_SUCCESS;
}