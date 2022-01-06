#pragma once
#include "include/Core.h"
#include "vendor/opencv/opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <tlhelp32.h>

enum class EMouseButton
{
	Left = 0,
	Right = 1,
};

class Application
{
public:
	Application();
	~Application();

	bool Initialize();
	bool Run();
	bool Shutdown();
private:
	HWND RetrieveMinecraftWindowHandle();
	BOOL IsMinecraftWindowFocused();
	BOOL CaptureGameFrame(cv::Mat& outImage);

	VOID HoldKey(const WORD keyCode, const BOOL hold);
	VOID HoldMouseButton(const EMouseButton mouseCode, const BOOL hold);

	VOID PressKey(const WORD keyCode, const DWORD sleep = 0);
	VOID PressMouseButton(const EMouseButton mouseButton, const int32_t sleep);
	VOID MoveMouse(const double xPosition, const double yPosition, const bool relative = false);
private:
	bool FoundLava(const cv::Mat& image, const float threshold = 10.0f);
private:
	bool m_Running;
	bool m_Mining;

	/* Minecraft Window Handle */
	HWND m_MinecraftWindowHandle;

	/* Thread ID's (used for attaching input in order to check window focus) */
	DWORD m_ProgramThreadID;
	DWORD m_MinecraftThreadID;

	/* OpenCV */
	cv::Mat m_CapturedFrame;
};