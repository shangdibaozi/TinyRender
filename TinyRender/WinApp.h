#pragma once
#include <windows.h>
#include <tchar.h>
typedef unsigned int UINT32;


class IRender
{
public:
	virtual void render(UINT32** canvas) = 0;
};

class WinApp 
{
protected:
	HINSTANCE	_hInstance;
	HWND		_hWnd;
	HDC			_hScreenDC;
	HBITMAP		_screenHB;
	HBITMAP		_screenOB;
	UINT32**	_canvas;

	int			_width;
	int			_height;
public:
	WinApp(HINSTANCE hInstance = 0)
		:_hInstance(hInstance),
		_hWnd(0)
	{
		WNDCLASSEX wnd;
		memset(&wnd, 0, sizeof(wnd));

		wnd.cbSize = sizeof(wnd);
		wnd.lpfnWndProc = WinApp::wndProc;
		wnd.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
		wnd.hCursor = LoadCursor(_hInstance, IDC_ARROW);
		wnd.hIcon = 0;
		wnd.hIconSm = 0;
		wnd.hInstance = hInstance;
		wnd.lpszClassName = _T("TinyRender");
		wnd.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		RegisterClassEx(&wnd);
	}

	virtual ~WinApp()
	{
		UnregisterClass(_T("TinyRender"), _hInstance);
	}

	virtual bool create(const char* title, int w, int h)
	{
		_width = w;
		_height = h;
		_hWnd = CreateWindowEx(
			0,
			_T("TinyRender"),
			title,
			WS_OVERLAPPEDWINDOW,
			100,
			100,
			w,
			h,
			0,
			0,
			_hInstance,
			this
		);

		if (!_hWnd)
		{
			return false;
		}

		HDC hdc = GetDC(_hWnd);
		_hScreenDC = CreateCompatibleDC(hdc);
		ReleaseDC(_hWnd, hdc);

		BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB, w * h * 4, 0, 0, 0, 0 } };
		LPVOID ptr;
		_screenHB = CreateDIBSection(_hScreenDC, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
		if (_screenHB == NULL)
		{
			return false;
		}
		_screenOB = (HBITMAP)SelectObject(_hScreenDC, _screenHB);

		unsigned char* screenFrameBuff = (unsigned char*)ptr;
		memset(screenFrameBuff, 0, w * h * 4);

		_canvas = (UINT32**)malloc(sizeof(void*) * h);
		for (int i = 0; i < h; i++) 
		{
			_canvas[i] = (UINT32*)(screenFrameBuff + w * 4 * i);
		}

		RECT rect = { 0, 0, w, h };
		AdjustWindowRect(&rect, GetWindowLong(_hWnd, GWL_STYLE), 0);
		int wx = rect.right - rect.left;
		int wy = rect.bottom - rect.top;
		int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
		int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
		if (sy < 0) sy = 0;
		SetWindowPos(_hWnd, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
		SetForegroundWindow(_hWnd);


		UpdateWindow(_hWnd);
		ShowWindow(_hWnd, SW_SHOW);

		return true;
	}

	virtual int run(IRender& iRender)
	{
		MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else 
			{
				//drawPoint(100, 100, 0xff0000);
				iRender.render(_canvas);
				screenUpdate();
				Sleep(1);
			}
		}
		screenClose();
		return 0;
	}

	void drawPoint(int x, int y, UINT32 color)
	{
		_canvas[y][x] = color;
	}

private:
	void screenClose()
	{
		if (_hScreenDC) 
		{
			if (_screenOB)
			{
				SelectObject(_hScreenDC, _screenOB);
				_screenOB = NULL;
			}
			DeleteDC(_hScreenDC);
			_hScreenDC = NULL;
		}

		if (_screenHB)
		{
			DeleteObject(_screenHB);
			_screenHB = NULL;
		}

		if (_hWnd)
		{
			CloseWindow(_hWnd);
			_hWnd = NULL;
		}
	}

	int onEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		{
			onKeyDown(wParam);
		}
		case WM_CLOSE:
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}

	void screenUpdate()
	{
		HDC hdc = GetDC(_hWnd);
		BitBlt(hdc, 0, 0, _width, _height, _hScreenDC, 0, 0, SRCCOPY);
		ReleaseDC(_hWnd, hdc);
	}

	void onKeyDown(int key)
	{
		switch (key)
		{
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
		}
		default:
			break;
		}
	}

protected:
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
	{
		if (msgId == WM_CREATE)
		{
			CREATESTRUCT* create_struct = (CREATESTRUCT*)lParam;
			LPVOID          lpCreateParams = create_struct->lpCreateParams;
			if (lpCreateParams)
			{
				::SetWindowLong(hWnd, GWL_USERDATA, (DWORD_PTR)lpCreateParams);
				WinApp* pThis = (WinApp*)lpCreateParams;

				return pThis->onEvent(hWnd, msgId, wParam, lParam);
			}
		}
		else 
		{
			WinApp* pThis = (WinApp*)GetWindowLong(hWnd, GWL_USERDATA);
			if (pThis)
			{
				return pThis->onEvent(hWnd, msgId, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, msgId, wParam, lParam);
	}
};