#include <windows.h>
#pragma comment(lib,"winmm.lib")
#include "resource.h"

HINSTANCE hInst = NULL;
HDC  hDC = NULL;  // Ŀ���豸DC
HDC  hMemDC = NULL;  // �ڴ滺��DC
HBITMAP  hBitmap = NULL;  // λͼ���
HPEN  hPen = NULL;  // ���������õĻ���
BOOL  bNewLine = FALSE;  // �Ƿ�ʼ�����µ�����

const int  bmpWidth = 800; // ͼƬ�ߴ�.��
const int  bmpHeight = 600;  // ͼƬ�ߴ�.��

// ����ȫģʽ��ɾ��
#define Safe_DeleteObject(p)  if(p) { DeleteObject(p); }

// ���ڹ���
LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);  //���ڹ��̺���

VOID  OnInit(HWND hwnd);  // ��ʼ��
VOID StartLine(int x, int y);    // �������
VOID DrawLine(int x, int y);   // ��������
VOID  Render(HWND hwnd);  // ����ͼ�������ڴ滺�塰������������
VOID  CleanUp();  // ����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LPCSTR lpszTitle = TEXT("Drawing");		//���ڱ���
	LPCSTR lpszName = TEXT("MainWindow"); // �����������
	LPCSTR lpszIconFile = TEXT("steam_dota.ico");  // ����ͼ��

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

	int scw = GetSystemMetrics(SM_CXSCREEN); // ��Ļ���
	int sch = GetSystemMetrics(SM_CYSCREEN);  // ��Ļ�߶�

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
			// ����Ϣ������û���κ���Ϣ����ִ���������������細�����ݵĻ���
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

	case WM_PAINT:   // ÿ��������Ҫ�ػ�ʱִ�д˲���
		Render(hwnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)    // ��������µļ���ESC
		{
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
		}
		else
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1),hInst ,SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
		}
		break;

	case WM_LBUTTONDOWN:  // ���������£���ʼ��������
		bNewLine = TRUE;
		StartLine(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONUP:  // �������ͷţ�ֹͣ����
		bNewLine = FALSE;
		break;

	case WM_MOUSEMOVE:
		if (bNewLine)   // �ƶ�����������
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

	return 0;	//�����˳�
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
	// Stretch��ʾ��Ҫ��������ͼ��
	StretchBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, bmpWidth, bmpHeight, SRCCOPY);
}

VOID CleanUp()
{
	Safe_DeleteObject(hDC);
	Safe_DeleteObject(hMemDC);
	Safe_DeleteObject(hBitmap);
	Safe_DeleteObject(hPen);
}