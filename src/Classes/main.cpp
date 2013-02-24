#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application* application;
	bool result;

	// Create the Application object.
	application = new Application;
	if(!application)
	{
		return 0;
	}

	// Initialize and run the Application object.
	result = application->Initialize();
	if(result)
	{
		application->Run();
	}

	// Shutdown and release the Application object.
	application->Shutdown();
	delete application;
	application = 0;

	return 0;
}