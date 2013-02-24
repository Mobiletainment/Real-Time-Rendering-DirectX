#ifndef _Application_H_
#define _Application_H_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Input.h"
#include "Graphics.h"
#include "Timer.h"

class Application
{
	enum SampleCount
	{
		SampleCountNone = 0,
		SampleCountLow = 1,
		SampleCountMid = 4,
		SampleCountHigh = 8,
		SampleCountUltra = 16
	};

	//Deprecated; replaced by GetMSAAQualityLevels
	enum SampleQuality
	{
		SampleQualityLow = 0,
		SampleQualityMid = 4,
		SampleQualityHigh = 8,
		SampleQualityUltra = 16
	};
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize();
	bool InitializeObjects();
	void Shutdown();
	void ShutdownObjects();
	void Run();

	int CurrentSampleQuality;
	SampleCount CurrentSampleCount;

	void ToggleSampleQuality();
	void ToggleSampleCount();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	int screenWidth;
	int screenHeight;

	char sampleInfo[64];

	Input* m_Input;
	Graphics* m_Graphics;
	Timer* m_Timer;
	float m_movSpeed;
	float m_rollSpeed;
	float m_mouseSpeeed;
	float m_camMov;
	float m_lightMov;

	bool pause;
	bool keysLocked;

	bool Update();

	void LogSampleInfo() 
	{
		sprintf_s(sampleInfo, "SampleCount: %d, SampleQuality: %d\n", CurrentSampleCount, CurrentSampleQuality);
		OutputDebugStringA(sampleInfo);
	};

	void InitializeWindows(int&, int&);
	void ShutdownWindows();
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Application* ApplicationHandle = 0;

#endif