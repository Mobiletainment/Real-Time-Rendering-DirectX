#ifndef _Input_H_
#define _Input_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <stdio.h>
class Input
{
public:

	Input();
	Input(const Input&);
	~Input();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Update();

	bool IsEscapePressed();

	bool IsForwardPressed();
	bool IsBackPressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsRollLeftPressed();
	bool IsRollRightPressed();

	bool IsSavePointPressed();
	bool IsCameraDrivePressed();

	bool IsLightForwardPressed();
	bool IsLightBackwardPressed();
	bool IsLightRightPressed();
	bool IsLightLeftPressed();
	bool IsLightUpPressed();
	bool IsLightDownPressed();

	bool IsActivateBumpMapRenderModePressed();
	bool IsActivateShadowMapRenderModePressed();
	bool isToggleWireFrameModePressed();
	bool isToggleWireFrameModeOffPressed();

	bool isToggleSampleQualityPressed();
	bool isToggleSampleCountAndQualityPressed();


	void GetMouseLocation(int&, int&);
	void GetMouseDelta(int&, int&);

private:

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth;
	int m_screenHeight;
	int m_mouseX;
	int m_mouseY;
	int m_mouseDeltaX;
	int m_mouseDeltaY;
	bool isSavePointPressed;
	bool isRunPointsPressed;
	bool isClearPointsPressed;
	bool isLightForwardPressed;
	bool isLightBackwardPressed;
	bool isLightRightPressed;
	bool isLightLeftPressed;
	bool isLightUpPressed;
	bool isLightDownPressed;
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
};

#endif