#include "Application.h"

Application::Application()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Timer = 0;
	m_movSpeed = 0.01f;
	m_mouseSpeeed = 0.02f;
	m_rollSpeed = 1.0f;
	m_camMov = 0.1f;
	m_lightMov = 0.02f;
	pause = false;
	keysLocked = false;
}

Application::Application(const Application& other)
{
}

Application::~Application()
{
}

bool Application::Initialize()
{
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	CurrentSampleCount = SampleCountHigh;
	CurrentSampleQuality = SampleQualityHigh;


	InitializeObjects();

	LogSampleInfo();

	CurrentSampleQuality = m_Graphics->GetMultiSampleQualityLevel(CurrentSampleCount);

	return true;
}

bool Application::InitializeObjects()
{
	bool result = false;

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd, CurrentSampleCount, CurrentSampleQuality);
	if(!result)
	{
		return false;
	}

	// Create the timer object.
	m_Timer = new Timer;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}



	return true;
}

void Application::Shutdown()
{

	ShutdownObjects();

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}

void Application::ShutdownObjects()
{
	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
}

void Application::Run()
{
	MSG msg;
	bool done;
	bool result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the update
			result = Update();
			if(!result)
			{
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		if(m_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}

	return;
}

bool Application::Update()
{
	bool result;
	int mouseX = 0;
	int mouseY = 0;

	// Update Input
	result = m_Input->Update();
	if(!result)
	{
		return false;
	}

	if(m_Input->IsForwardPressed() == true)
	{
		m_Graphics->UpdateCameraMovement(m_movSpeed, 0);
	}
	
	if(m_Input->IsBackPressed() == true)
	{
		m_Graphics->UpdateCameraMovement(-m_movSpeed, 0);
	}

	if(m_Input->IsRightPressed() == true)
	{
		m_Graphics->UpdateCameraMovement(0, m_movSpeed);
	}

	if(m_Input->IsLeftPressed() == true)
	{
		m_Graphics->UpdateCameraMovement(0, -m_movSpeed);
	}

	if(m_Input->IsRollRightPressed() == true)
	{
		m_Graphics->UpdateCameraOrientation(0, 0, m_rollSpeed);
	}

	if(m_Input->IsRollLeftPressed() == true)
	{
		m_Graphics->UpdateCameraOrientation(0, 0, -m_rollSpeed);
	}

	if(m_Input->IsSavePointPressed() == true)
	{
		m_Graphics->SavePoint();
	}
	
	if(m_Input->IsCameraDrivePressed() == true)
	{
		m_Graphics->RunPoints();
	}

	if(m_Input->IsLightForwardPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, 0.0f, m_lightMov);
	}

	if(m_Input->IsLightBackwardPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, 0.0f, -m_lightMov);
	}

	if(m_Input->IsLightRightPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, m_lightMov, 0.0f);
	}

	if(m_Input->IsLightLeftPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, -m_lightMov, 0.0f);
	}

	if(m_Input->IsLightUpPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, 0.0f, m_camMov);
	}

	if(m_Input->IsLightDownPressed() == true)
	{
		m_Graphics->UpdateLightPosition(0.0f, 0.0f, -m_camMov);
	}

	if(m_Input->IsActivateBumpMapRenderModePressed() == true)
	{
		m_Graphics->ActivateBumpMapRenderMode();
	}
	else if(m_Input->IsActivateShadowMapRenderModePressed() == true)
	{
		m_Graphics->ActivateShadowMapRenderMode();
	}

	if(m_Input->isToggleWireFrameModePressed() == true)
	{
		m_Graphics->ToggleWireFrameMode(true);
	}
	else if(m_Input->isToggleWireFrameModeOffPressed() == true)
	{
		m_Graphics->ToggleWireFrameMode(false);
	}
	else if (m_Input->isToggleSampleCountAndQualityPressed() == true && keysLocked == false)
	{
		pause = true;
		keysLocked = true;
		ToggleSampleCount();
		//ToggleSampleQuality();

		int sampleQuality = 0;
		
		if (CurrentSampleCount > 0)
		{
			sampleQuality = m_Graphics->GetMultiSampleQualityLevel(CurrentSampleCount);
		}
		 
		//int sampleQuality = CurrentSampleQuality;

		if (sampleQuality == -1)
		{
			wchar_t errorMessage[128];
			swprintf(errorMessage, L"Multisampling with Sample-Count %d, not supported on this device!\n", CurrentSampleCount);
			MessageBox(m_hwnd, errorMessage, L"Error", MB_OK);
		}
		else
		{
			CurrentSampleQuality = sampleQuality;
			LogSampleInfo();

			m_Graphics->Shutdown();
			m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd, CurrentSampleCount, CurrentSampleQuality);
		}

		
		pause = false;
	}
	else if (m_Input->isToggleSampleQualityPressed() == true)
	{
		keysLocked = false;
	}

	if (pause) //graphics is being reinitialized and update is called meanwhile
		return true;

	m_Input->GetMouseDelta(mouseX, mouseY);
	m_Graphics->UpdateCameraOrientation(-mouseX*m_mouseSpeeed, -mouseY*m_mouseSpeeed, 0);

	// Update Timer
	m_Timer->Update();
	result = m_Graphics->Update( m_Timer->GetTime() );
	if(!result)
	{
		return false;
	}

	// Render
	result = m_Graphics->Render();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Application::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 1280;
		screenHeight = 800;

		// Place the window in the middle of the screen.
		//posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		//posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

		posX = 0;
		posY = 0;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}

void Application::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the Application class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
	
void Application::ToggleSampleQuality()
{
	switch (CurrentSampleQuality)
	{
	case SampleQualityLow:
		CurrentSampleQuality = SampleQualityMid;
		break;
	case SampleQualityMid:
		CurrentSampleQuality = SampleQualityHigh;
		break;
	case SampleQualityHigh:
		CurrentSampleQuality = SampleQualityUltra;
		break;
	case SampleQualityUltra:
		CurrentSampleQuality = SampleQualityLow;
		break;
	default:
		break;
	}
}


void Application::ToggleSampleCount()
{
	switch (CurrentSampleCount)
	{
	case SampleCountNone:
		CurrentSampleCount = SampleCountLow;
		break;
	case SampleCountLow:
		CurrentSampleCount = SampleCountMid;
		break;
	case SampleCountMid:
		CurrentSampleCount = SampleCountHigh;
		break;
	case SampleCountHigh:
		CurrentSampleCount = SampleCountUltra;
		break;
	case SampleCountUltra:
		CurrentSampleCount = SampleCountNone;
		break;
	default:
		break;
	}
}