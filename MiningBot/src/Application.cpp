#include "include\Application.h"
#include <tchar.h>

Application::Application()
	:
	m_Running(true),
	m_Mining(false),
	m_MinecraftWindowHandle(NULL),
	m_MinecraftThreadID(0),
	m_ProgramThreadID(0)
{}

Application::~Application()
{
}

bool Application::Initialize()
{
	m_MinecraftWindowHandle = RetrieveMinecraftWindowHandle();
	if (!m_MinecraftWindowHandle)
	{
		printf("Failed to retrieve minecraft's window handle. Is the game running?\n");
		return false;
	}

	m_ProgramThreadID = GetCurrentThreadId();
	if (!m_ProgramThreadID)
	{
		printf("Failed to retrieve current thread ID\n");
		return false;
	}

	m_MinecraftThreadID = GetWindowThreadProcessId(
		m_MinecraftWindowHandle,
		NULL);

	if (!m_MinecraftThreadID)
	{
		printf("Failed to retrieve minecraft's thread ID\n");
		return false;
	}

	return true;
}

bool Application::Run()
{
	INT value[3];
	SystemParametersInfoA(
		SPI_GETMOUSESPEED,
		NULL,
		value,
		NULL);	
	const INT systemMouseSpeed = value[0];

	AttachThreadInput(m_MinecraftThreadID, m_ProgramThreadID, TRUE);
	while (m_Running)
	{
		if (IsMinecraftWindowFocused())
		{
			if (GetAsyncKeyState(KEY_O) & (1 << 15))
			{
				m_Mining = !m_Mining;
				if (m_Mining)
				{
					HoldKey(KEY_W, TRUE);
					HoldMouseButton(EMouseButton::Left, TRUE);
				}
				else
				{
					HoldKey(KEY_W, FALSE);
					HoldMouseButton(EMouseButton::Left, FALSE);
				}

				Sleep(500);
			}

			if (m_Mining)
			{
				CaptureGameFrame(m_CapturedFrame);
				if (!FoundLava(m_CapturedFrame, 0.5f))
				{

				}
				else
				{
					/* PANIC */
					HoldKey(KEY_W, FALSE);
					HoldMouseButton(EMouseButton::Left, FALSE);
					PressKey(KEY_S, 2000);

					do
					{
						MoveMouse(200 * systemMouseSpeed, 0, true);
						Sleep(1000);
						CaptureGameFrame(m_CapturedFrame);
					} while (FoundLava(m_CapturedFrame, 0.5f));

					HoldKey(KEY_W, TRUE);
					HoldMouseButton(EMouseButton::Left, TRUE);
				}
			}

			system("cls");
			printf("Window focused\n");
		}
		else
		{
			HoldKey(KEY_W, FALSE);
			HoldMouseButton(EMouseButton::Left, FALSE);

			if (GetAsyncKeyState(KEY_T) & (1 << 15))
			{
				m_Running = false;
				break;
			}
		}
		
		/* Unpress mouse button */
		system("cls");
		printf("Window unfocused\n");
	}

	AttachThreadInput(m_MinecraftThreadID, m_ProgramThreadID, FALSE);
	return true;
}

bool Application::Shutdown()
{
	return true;
}

HWND Application::RetrieveMinecraftWindowHandle()
{
	INTERNALSCOPE HWND windowHandle = NULL;
	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		if (IsWindowVisible(hwnd))
		{
			CONST INT windowNameLength = GetWindowTextLength(hwnd);
			if (windowNameLength < 1)
				return true;

			std::wstring windowTitle;
			windowTitle.resize(windowNameLength + 1);

			GetWindowText(hwnd, windowTitle.data(), windowNameLength + 1);
			std::wcout << L"Found window: " << windowTitle << L'\n';

			constexpr wchar_t windowLookupHint[]{ L"Minecraft" };
			if (windowTitle.find(windowLookupHint) != std::string::npos)
				windowHandle = hwnd;
		}

		return true;
		}, NULL);

	return windowHandle;
}

BOOL Application::IsMinecraftWindowFocused()
{
	AttachThreadInput(m_MinecraftThreadID, m_ProgramThreadID, TRUE);
	return m_MinecraftWindowHandle == GetFocus() && m_MinecraftWindowHandle == GetActiveWindow();
}

BOOL Application::CaptureGameFrame(cv::Mat& outImage)
{
	HDC minecraftDeviceContext = GetDC(m_MinecraftWindowHandle);
	HDC copyRegion = CreateCompatibleDC(minecraftDeviceContext);

	RECT gameRectangle;
	GetClientRect(m_MinecraftWindowHandle, &gameRectangle);

	const int32_t width = gameRectangle.bottom;
	const int32_t height = gameRectangle.right;
	HBITMAP bitmap = CreateCompatibleBitmap(minecraftDeviceContext, width, height);
	
	SelectObject(copyRegion, bitmap);
	BitBlt(copyRegion, 0, 0, width, height, minecraftDeviceContext, 0, 0, SRCCOPY);

	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1; 
	bi.biYPelsPerMeter = 2; 
	bi.biClrUsed = 3; 
	bi.biClrImportant = 4;
	outImage = std::move(cv::Mat(height, width, (((0) & ((1 << 3) - 1)) + (((4) - 1) << 3)))); 

	GetDIBits(copyRegion, bitmap, 0, height, outImage.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	DeleteObject(bitmap);
	DeleteDC(copyRegion); 
	ReleaseDC(m_MinecraftWindowHandle, minecraftDeviceContext);
	return TRUE;
}

VOID Application::HoldKey(const WORD keyCode, const BOOL hold)
{
	INPUT input;
	memset(&input, 0, sizeof(INPUT));

	/* 0 - Key down */
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = keyCode;
	input.ki.wScan = MapVirtualKeyExA(keyCode, 0, (HKL)GetKeyboardLayout(m_MinecraftThreadID));
	input.ki.dwFlags = hold ? 0 : KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));
}

VOID Application::HoldMouseButton(const EMouseButton mouseCode, const BOOL hold)
{
	INPUT mouseBuffer{ 0 };
	mouseBuffer.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	mouseBuffer.mi.dwFlags |= mouseCode == EMouseButton::Left ? (hold ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP) : (hold ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP);

	SendInput(1, &mouseBuffer, sizeof(INPUT));
}

VOID Application::PressKey(const WORD keyCode, const DWORD sleep)
{
	INPUT input;
	memset(&input, 0, sizeof(INPUT));
	
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = keyCode;
	input.ki.wScan = MapVirtualKeyExA(keyCode, 0, (HKL)GetKeyboardLayout(m_MinecraftThreadID));
	input.ki.time = sleep;
	SendInput(1, &input, sizeof(INPUT));	

	if(sleep)
		Sleep(sleep);

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

VOID Application::PressMouseButton(const EMouseButton mouseButton, const int32_t sleep)
{
	INPUT mouseBuffer;
	memset(&mouseBuffer, 0, sizeof(INPUT));

	mouseBuffer.mi.time = sleep;
	mouseBuffer.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | (mouseButton == EMouseButton::Left ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN));
	SendInput(1, &mouseBuffer, sizeof(INPUT));
	
	if(sleep)
	Sleep(sleep);
	
	mouseBuffer.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | (mouseButton == EMouseButton::Left ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP));
	SendInput(1, &mouseBuffer, sizeof(INPUT));
}

VOID Application::MoveMouse(const double xPosition, const double yPosition, const bool relative)
{
	INPUT input;
	memset(&input, 0, sizeof(INPUT));
	
	input.type = INPUT_MOUSE;
	input.mi.dx = xPosition;
	input.mi.dy = yPosition;
	input.mi.dwFlags = relative ? MOUSEEVENTF_MOVE : MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	input.mi.time = 0;

	SendInput(1, &input, sizeof(INPUT));
}

bool Application::FoundLava(const cv::Mat& image, const float threshold)
{
	const cv::Scalar lavaHSVLow = cv::Scalar(2, 48, 130);
	const cv::Scalar lavaHSVHigh = cv::Scalar(72, 90, 225);
	std::vector<std::vector<cv::Point>> contours;
	cv::Mat maskImage;

	cv::inRange(image, lavaHSVLow, lavaHSVHigh, maskImage);
	cv::findContours(maskImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	uint32_t mask = 0;
	for (uint32_t i = 0; i < maskImage.rows; ++i)
	{
		uint8_t* iter = maskImage.ptr(i);
		for (uint32_t j = 0; j < maskImage.cols; ++j)
		{
			mask += iter[j];
		}
	}

	const uint32_t pixelCount = maskImage.rows * maskImage.cols;
	double maskDensity = static_cast<double>(mask) / static_cast<double>(pixelCount);

	printf("Mask density: %f\n", maskDensity);
	return maskDensity > threshold;
}
