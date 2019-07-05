#include <windows.h>
#pragma comment(lib,"winmm.lib")
#include "resource.h"

HINSTANCE hInst = NULL;
HDC  hDC = NULL;  // 目标设备DC
HDC  hMemDC = NULL;  // 内存缓冲DC
HBITMAP  hBitmap = NULL;  // 位图句柄
HPEN  hPen = NULL;  // 绘制线条用的画笔
BOOL  bNewLine = FALSE;  // 是否开始绘制新的线条

const int  bmpWidth = 800; // 图片尺寸.宽
const int  bmpHeight = 600;  // 图片尺寸.高

// “安全模式”删除
#define Safe_DeleteObject(p)  if(p) { DeleteObject(p); }

// 窗口过程
LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);  //窗口过程函数

VOID  OnInit(HWND hwnd);  // 初始化
VOID StartLine(int x, int y);    // 线条起点
VOID DrawLine(int x, int y);   // 绘制线条
VOID  Render(HWND hwnd);  // “贴图”：将内存缓冲“贴”到窗口上
VOID  CleanUp();  // 清理

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LPCSTR lpszTitle = TEXT("Drawing");		//窗口标题
	LPCSTR lpszName = TEXT("MainWindow"); // 窗口类的名字
	LPCSTR lpszIconFile = TEXT("steam_dota.ico");  // 程序图标

	hInst = hInstance;
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = lpszName;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	int scw = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度
	int sch = GetSystemMetrics(SM_CYSCREEN);  // 屏幕高度

	//int left = scw / 10;  // 10%
	//int top = sch / 10;
	//int width = scw * 4 / 5;  // 80%
	//int height = sch * 4 / 5;

	int left = (scw - bmpWidth) / 2;
	int top = (sch - bmpHeight) / 2;

	HWND hwnd = CreateWindow(lpszName, lpszTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		bmpWidth, bmpHeight, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, left, top, bmpWidth, bmpHeight, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 若消息队列中没有任何消息，则执行其他操作，比如窗口内容的绘制
			// Render Loop
		}
	}

	UnregisterClass(lpszName, wndClass.hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnInit(hwnd);
		break;

	case WM_PAINT:   // 每当窗口需要重绘时执行此操作
		Render(hwnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)    // 如果被按下的键是ESC
		{
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
		}
		else
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1),hInst ,SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
		}
		break;

	case WM_LBUTTONDOWN:  // 鼠标左键按下，开始绘制线条
		bNewLine = TRUE;
		StartLine(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONUP:  // 鼠标左键释放，停止绘制
		bNewLine = FALSE;
		break;

	case WM_MOUSEMOVE:
		if (bNewLine)   // 移动鼠标绘制曲线
		{
			DrawLine(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_DESTROY:
		CleanUp();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;	//正常退出
}

VOID OnInit(HWND hwnd)
{
	hDC = GetDC(hwnd);
	hMemDC = CreateCompatibleDC(hDC);
	hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	SelectObject(hMemDC, hBitmap);
	hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
	SelectObject(hMemDC, hPen);
}

VOID StartLine(int x, int y)
{
	MoveToEx(hMemDC, x,y, NULL);
}

VOID DrawLine(int x, int y)
{
	LineTo(hMemDC, x,y);
}

VOID  Render(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom -rect.top;
	// Stretch表示将要“缩放贴图”
	StretchBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, bmpWidth, bmpHeight, SRCCOPY);
}

VOID CleanUp()
{
	Safe_DeleteObject(hDC);
	Safe_DeleteObject(hMemDC);
	Safe_DeleteObject(hBitmap);
	Safe_DeleteObject(hPen);
}