#include "Input.h"

Input::Input()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
	isSavePointPressed = false;
	isRunPointsPressed = false;
	isClearPointsPressed = false;
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;


	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void Input::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool Input::Update()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Input::ProcessInput()
{
	// Update delta of mouse cursor based on the change of the mouse location during the Update
	m_mouseDeltaX = m_mouseState.lX;
	m_mouseDeltaY = m_mouseState.lY;

	// Update the location of the mouse cursor based on the change of the mouse location during the Update
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height
	/*
	if(m_mouseX < 0)
	{
		m_mouseX = 0;
	}

	if(m_mouseY < 0)
	{
		m_mouseY = 0;
	}

	if(m_mouseX > m_screenWidth)
	{
		m_mouseX = m_screenWidth;
	}

	if(m_mouseY > m_screenHeight)
	{
		m_mouseY = m_screenHeight;
	}
	*/

	return;
}

bool Input::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsForwardPressed()
{
	if(m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsBackPressed()
{
	if(m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsLeftPressed()
{
	if(m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsRightPressed()
{
	if(m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsRollRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_E] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsRollLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_Q] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsSavePointPressed()
{
	if(m_keyboardState[DIK_SPACE] & 0x80)
	{
		if(!isSavePointPressed)
		{
			return false;
		}

		isSavePointPressed = false;
		return true;
	}

	isSavePointPressed = true;
	return false;
}

bool Input::IsCameraDrivePressed()
{
	if(m_keyboardState[DIK_RETURN] & 0x80)
	{
		if(!isRunPointsPressed)
		{
			return false;
		}
		return true;
	}

	isRunPointsPressed = true;
	return false;
}


bool Input::IsLightForwardPressed()
{
	if(m_keyboardState[DIK_UPARROW] & 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsLightBackwardPressed()
{
	if(m_keyboardState[DIK_DOWNARROW] & 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsLightRightPressed()
{
	if(m_keyboardState[DIK_RIGHTARROW] & 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsLightLeftPressed()

{
	if(m_keyboardState[DIK_LEFTARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsLightUpPressed()

{
	if(m_keyboardState[DIK_N] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsLightDownPressed()
{
	if(m_keyboardState[DIK_M] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsActivateBumpMapRenderModePressed()
{
	if(m_keyboardState[DIK_1] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsActivateShadowMapRenderModePressed()
{
	if(m_keyboardState[DIK_2] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::isToggleWireFrameModePressed()
{
	if(m_keyboardState[DIK_3] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::isToggleWireFrameModeOffPressed()
{
	if(m_keyboardState[DIK_4] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::isToggleSampleCountAndQualityPressed()
{
	if(m_keyboardState[DIK_5] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::isToggleSampleQualityPressed()
{
	if(m_keyboardState[DIK_6] & 0x80)
	{
		return true;
	}

	return false;
}

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{ 
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

void Input::GetMouseDelta(int& mouseX, int& mouseY)
{ 
	mouseX = m_mouseDeltaX;
	mouseY = m_mouseDeltaY;

	return;
}