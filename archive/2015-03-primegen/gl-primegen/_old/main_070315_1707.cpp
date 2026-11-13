// Console & Core Includes
//#include <iostream>         // I/O Stream
//#include <string>           // String
#include <stdio.h>          // Standart IO
#include <fstream>          // FileStream
#include <ctime>            // Date & Time

// WinAPI Includes
#include <windows.h>        // WinAPI
#include <gl/gl.h>          // OpenGL
#include <gl/glu.h>         // GLu32
#include <gl/glaux.h>       // GLaux

using namespace std;

static HGLRC hRC;       // Постоянный контекст рендеринга
static HDC hDC;         // Приватный контекст устройства GDI

bool keys[256];         // Массив, используемый для операций с клавиатурой
bool active=true;       // Флаг активности окна, установленный в true по умолчанию
bool fullscreen=false;   // Флаг режима окна, установленный в полноэкранный по умолчанию

void Logger_WriteFile (char *reason)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	
	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"[%I:%M:%S] ",timeinfo); //"%d-%m-%Y %I:%M:%S"
	std::string str(buffer);

	std::ofstream logfile;
	logfile.open("hello.log", std::ofstream::out | std::ofstream::trunc);
	logfile << str << reason << "\n" << endl;
	logfile.close();
}

GLvoid InitGL(GLsizei Width, GLsizei Height)	// Вызвать после создания окна GL
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	// Очистка экрана в черный цвет
	glClearDepth(1.0);		// Разрешить очистку буфера глубины
	glDepthFunc(GL_LESS);	// Тип теста глубины
	glEnable(GL_DEPTH_TEST);// разрешить тест глубины
	glShadeModel(GL_SMOOTH);// разрешить плавное цветовое сглаживание
	glMatrixMode(GL_PROJECTION);// Выбор матрицы проекции
	glLoadIdentity();		// Сброс матрицы проекции
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
	// Вычислить соотношение геометрических размеров для окна
	glMatrixMode(GL_MODELVIEW);// Выбор матрицы просмотра модели
}

GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
	if (Height==0)		// Предотвращение деления на ноль, если окно слишком мало
	Height=1;

	glViewport(0, 0, Width, Height);
	// Сброс текущей области вывода и перспективных преобразований

	glMatrixMode(GL_PROJECTION);// Выбор матрицы проекций
	glLoadIdentity();			// Сброс матрицы проекции

	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
	// Вычисление соотношения геометрических размеров для окна
	glMatrixMode(GL_MODELVIEW);	// Выбор матрицы просмотра модели
}

void DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// очистка Экрана и буфера глубины
	glLoadIdentity();
	// Сброс просмотра
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT	Screen;		// используется позднее для размеров окна
	GLuint	PixelFormat;
	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Размер этой структуры
		1,				// Номер версии (?)
		PFD_DRAW_TO_WINDOW |// Формат для Окна
		PFD_SUPPORT_OPENGL |// Формат для OpenGL
		PFD_DOUBLEBUFFER,// Формат для двойного буфера
		PFD_TYPE_RGBA,	// Требуется RGBA формат
		16,				// Выбор 16 бит глубины цвета
		0, 0, 0, 0, 0, 0,// Игнорирование цветовых битов (?)
		0,				// нет буфера прозрачности
		0,				// Сдвиговый бит игнорируется (?)
		0,				// Нет буфера аккумуляции
		0, 0, 0, 0,		// Биты аккумуляции игнорируются (?)
		16,				// 16 битный Z-буфер (буфер глубины)  
		0,				// Нет буфера траффарета
		0,				// Нет вспомогательных буферов (?)
		PFD_MAIN_PLANE,	// Главный слой рисования
		0,				// Резерв (?)
		0, 0, 0			// Маски слоя игнорируются (?)
	};
	switch (message)	// Тип сообщения
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);	// Получить контекст устройства для окна
		PixelFormat = ChoosePixelFormat(hDC, &pfd);
		// Найти ближайшее совпадение для нашего формата пикселов
		if (!PixelFormat)
		{
			Logger_WriteFile("Error: Can't Find A Suitable PixelFormat.");
			//MessageBox(0,"Can't Find A Suitable PixelFormat.","Error",MB_OK|MB_ICONERROR);
			PostQuitMessage(0);
			// Это сообщение говорит, что программа должна завершится
			break;	// Предтовращение повтора кода
		}
		if(!SetPixelFormat(hDC,PixelFormat,&pfd))
		{
			Logger_WriteFile("Error: Can't Set The PixelFormat.");
			//MessageBox(0,"Can't Set The PixelFormat.","Error",MB_OK|MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		hRC = wglCreateContext(hDC);
		if(!hRC)
		{
			Logger_WriteFile("Error: Can't Create A GL Rendering Context.");
			//MessageBox(0,"Can't Create A GL Rendering Context.","Error",MB_OK|MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		if(!wglMakeCurrent(hDC, hRC))
		{
			Logger_WriteFile("Error: Can't activate GLRC.");
			//MessageBox(0,"Can't activate GLRC.","Error",MB_OK|MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		GetClientRect(hWnd, &Screen);
		InitGL(Screen.right, Screen.bottom);
		Logger_WriteFile("Info: OpenGL initialized");
		break;

	case WM_DESTROY:
	case WM_CLOSE:
		ChangeDisplaySettings(NULL, 0);

		wglMakeCurrent(hDC,NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd,hDC);
		Logger_WriteFile("Info: Application closed");
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		keys[wParam] = TRUE;
		break;

	case WM_KEYUP:
		keys[wParam] = FALSE;
		break;

	case WM_SIZE:
		ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();

	MSG            msg;       // Структура сообщения Windows
	WNDCLASS       wc;       // Структура класса Windows для установки типа окна
	HWND           hWnd;    // Сохранение дискриптора окна

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "RenderWindow";

	if(!RegisterClass(&wc))
	{
        Logger_WriteFile("Error: Failed To Register The Window Class.");
		//MessageBox(0,"Failed To Register The Window Class.","Error",MB_OK|MB_ICONERROR);
		return FALSE;
	}

    const char* TitleStatus = "placeholder"; // placeholder

	hWnd = CreateWindow(wc.lpszClassName, TitleStatus, WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);
    SetConsoleTitle(TitleStatus);

	if(!hWnd)
	{
		MessageBox(0,"Window Creation Error.","Error",MB_OK|MB_ICONERROR); 
		return FALSE;
	}

	DEVMODE dmScreenSettings;			// Режим работы

	memset(&dmScreenSettings, 0, sizeof(DEVMODE));	// Очистка для хранения установок
	dmScreenSettings.dmSize	= sizeof(DEVMODE);		// Размер структуры Devmode
	dmScreenSettings.dmPelsWidth	= 640;			// Ширина экрана
	dmScreenSettings.dmPelsHeight	= 480;			// Высота экрана
	dmScreenSettings.dmFields	= DM_PELSWIDTH | DM_PELSHEIGHT;	// Режим Пиксела
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	// Переключение в полный экран

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	while (1)
	{
		// Обработка всех сообщений
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				return TRUE;
			}
		}
        
		//std::cout << str;
        
        //TitleStatus << status << endl;
        
		DrawGLScene();				// Нарисовать сцену
		SwapBuffers(hDC);			// Переключить буфер экрана
		if (keys[VK_ESCAPE]) SendMessage(hWnd,WM_CLOSE,0,0);	// Если ESC - выйти
	}
}
