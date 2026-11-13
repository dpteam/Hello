#include <iostream>         // I/O Stream
#include <ctime>            // Date & Time
#include <cmath>			// Math
#include <fstream>          // FileStream
#include <gl/glut.h>        // glut32

//#pragma comment(linker, "/entry:mainCRTStartup")
//#pragma comment(linker, "/subsystem:WINDOWS")

#define WMAIN_CLASSNAME "WndClassName"
#define WMAIN_CAPTION "Hello, world!"

using namespace std;
std::ofstream logfile;

static HGLRC hRC;       // Постоянный контекст рендеринга
static HDC hDC;         // Приватный контекст устройства GDI

bool keys[256];			// Массив, используемый для операций с клавиатурой
bool active=true;		// Флаг активности окна, установленный в true по умолчанию
bool fullscreen=false;	// Флаг режима окна, установленный в полноэкранный по умолчанию

int i;
int nGen;
int nPrime;

typedef struct starstruct {
	float x;
	float y;
	float z;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool isPrime (int n);
void starmake(starstruct *star, int maxstar, int distance);
void Logger_WriteFile (char *reason);
void DrawGLScene_Starfield(starstruct *star, int maxstar, float size, int texid);
void DrawGLScene_Triangle(void);

// *** main ***

int main(int argc, char *argv[])
{
    HINSTANCE hInstance=(HINSTANCE)GetModuleHandle(NULL);
	MSG            msg;        // Структура сообщения Windows
	WNDCLASS       wc;         // Структура класса Windows для установки типа окна
	HWND           hWnd;       // Сохранение дискриптора окна

	logfile.open("hello.log", std::ofstream::out | std::ofstream::trunc);

	/* register window class */
	wc.style			= CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon (NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH) GetStockObject (BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= WMAIN_CLASSNAME;

	if(!RegisterClass(&wc))
	{
		Logger_WriteFile("Error: Failed To Register The Window Class.");
		return 1;
	}

	/* create main window */
	hWnd = CreateWindow(WMAIN_CLASSNAME, WMAIN_CAPTION, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);

	if(!hWnd)
	{
		Logger_WriteFile("Error: Window Creation Error.");       
		//MessageBox(0,"Window Creation Error.","Error",MB_OK|MB_ICONERROR); 
		return 1;
	}
	if(fullscreen == 1)
	{
		DEVMODE dmScreenSettings;									// Режим работы

		memset(&dmScreenSettings, 0, sizeof(DEVMODE));				// Очистка для хранения установок
		dmScreenSettings.dmSize	= sizeof(DEVMODE);					// Размер структуры Devmode
		dmScreenSettings.dmPelsWidth	= 640;						// Ширина экрана
		dmScreenSettings.dmPelsHeight	= 480;						// Высота экрана
		dmScreenSettings.dmFields	= DM_PELSWIDTH | DM_PELSHEIGHT;	// Режим Пиксела
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		
		// Переключение в полный экран
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		SetFocus(hWnd);
	}
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    do
    {
        nGen += rand()%127+2;
        if(nGen <= 127 && isPrime(nGen))
        {
            nPrime << nGen;
            std::cout << nPrime << std::endl;
            Sleep(250);
        }
    } while(isPrime(nPrime));
	DrawGLScene_Triangle();									// Нарисовать сцену // Вызвать поток 603-20
	//DrawGLScene_Starfield(NULL, 255, 5, NULL);
	SwapBuffers(hDC);										// Переключить буфер экрана
	if (keys[VK_ESCAPE]) SendMessage(hWnd,WM_CLOSE,0,0);	// Если ESC - выйти
    return 0;
}

// *** Window Procedure ***

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    AllocConsole();
    SetConsoleTitle("Codename Hello");
    PAINTSTRUCT ps;
    hDC = GetDC(hWnd);	// Получить контекст устройства для окна
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
		//GetClientRect(hWnd, &Screen);
		if(GetClientRect(hWnd, &Screen))
		{
			int width = Screen.right - Screen.left;
			int height = Screen.bottom - Screen.top;
		}
		Logger_WriteFile("Info: OpenGL initialized");
		break;
    case WM_SIZE:
        InvalidateRect(hWnd,NULL,true);
        break;
    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        EndPaint(hWnd,&ps);
        break;
	case WM_DESTROY:
	case WM_CLOSE:
        FreeConsole();
		ChangeDisplaySettings(NULL, 0);
		wglMakeCurrent(hDC,NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd,hDC);
		Logger_WriteFile("Info: Application closed");
		logfile.close();
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		keys[wParam] = TRUE;
		break;
	case WM_KEYUP:
		keys[wParam] = FALSE;
		break;
    default: 
        return DefWindowProc(hWnd,message,wParam,lParam);
    }
    return 0;
}

bool isPrime (int n)        // Функция проверки простого числа
{
    for (int i = 2; i < sqrt(n); i++)
        if (n % i == 0)
            return false;   // число делится, сл. непростое
    return n > 1;
}

void starmake(starstruct *star, int maxstar, int distance) {
	for(int i=1; i<maxstar; i++) {
		star[i].x = (float)(rand()%distance - (distance / 2));
		star[i].y = (float)(rand()%distance - (distance / 2));
		star[i].z = (float)(rand()%distance - (distance / 2));
	}
}

void Logger_WriteFile (char *reason)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	
	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"[%I:%M:%S] ",timeinfo); //"%d-%m-%Y %I:%M:%S"
	std::string str(buffer);

	logfile << str << reason << "\n" << endl;
}

void DrawGLScene_Starfield(starstruct *star, int maxstar, float size, int texid) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glColor3f(1.0f,1.0f,1.0f);
	
	glBindTexture(GL_TEXTURE_2D, texid);
	
	for(int i=1; i<maxstar; i++) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex3f(star[i].x,        star[i].y,        star[i].z);
		glTexCoord2f(1.0f,0.0f); glVertex3f(star[i].x + size, star[i].y,        star[i].z);
		glTexCoord2f(1.0f,1.0f); glVertex3f(star[i].x + size, star[i].y + size, star[i].z);
		glTexCoord2f(0.0f,1.0f); glVertex3f(star[i].x,        star[i].y + size, star[i].z);
		glEnd();
	}
	glDisable(GL_BLEND);
}

void DrawGLScene_Triangle(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// очистка Экрана и буфера глубины
	glLoadIdentity();
	// Сброс просмотра

	glBegin (GL_TRIANGLES);
	glColor3f(1.0f,0.0f,0.0f);              // Красный цвет
	glVertex3f( 0.0f, 0.386f, 0.0f);        // Вверх        // Upper - WIN! (Down - Hm... LET!)
	
	glColor3f(0.0f,1.0f,0.0f);              // Зеленный цвет
	glVertex3f(-0.276f,-0.276f, 0.0f);      // Слева снизу  // Down Left 
	
	glColor3f(0.0f,0.0f,1.0f);              // Синий цвет
	glVertex3f( 0.276f,-0.276f, 0.0f);      // Справа снизу // Down Right 
	glEnd ();
}
