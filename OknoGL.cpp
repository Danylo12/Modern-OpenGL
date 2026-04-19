#include "OknoGL.h"
#include "resource.h"
#include "glew.h"
#include "wglew.h"
#include "Werteks.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, int nCmdShow)
{
	POINT polozenieOkna = { 100, 100 };
	POINT rozmiarOkna = { 800, 600 };
	if (!okno.Inicjuj(hInstance, polozenieOkna, rozmiarOkna))
	{
		MessageBox(NULL, "Inicjacja okna nie powiodla si�",
			"Aplikacja OpenGL", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	else return okno.Uruchom();	
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return okno.WndProc(hWnd, message, wParam, lParam);
}

LRESULT Okno::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE: 
		RECT rect;
		GetClientRect(hWnd, &rect);
		szerokoscObszaruUzytkownika = rect.right - rect.left;
		wysokoscObszaruUzytkownika = rect.bottom - rect.top;
		break;

	default: 
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0L;
}

bool Okno::Inicjuj(HINSTANCE uchwytAplikacji, POINT polozenieOkna, POINT rozmiarOkna, 
	bool trybPelnoekranowy, bool zmianaRozdzielczosci)
{
	char nazwaOkna[] = "Aplikacja OpenGL";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	wc.lpfnWndProc = (WNDPROC)::WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = uchwytAplikacji; 
	wc.hIcon = LoadIcon(uchwytAplikacji, MAKEINTRESOURCE(IDI_ICON1)); 
	wc.hIconSm = LoadIcon(uchwytAplikacji, MAKEINTRESOURCE(IDI_ICON1)); 


	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = NULL; 
	wc.lpszMenuName = NULL; 
	wc.lpszClassName = nazwaOkna; 

	if (RegisterClassEx(&wc) == 0) return false;

	DWORD stylOkna = WS_OVERLAPPEDWINDOW;
	
	if (trybPelnoekranowy)
	{
		stylOkna = WS_POPUP;
		polozenieOkna.x = 0;
		polozenieOkna.y = 0;
		if (zmianaRozdzielczosci)
		{
			rozmiarOkna.x = 1024;
			rozmiarOkna.y = 768;
			if (!ZmianaRozdzielczosci(rozmiarOkna.x, rozmiarOkna.y)) return false;
		}
		else
		{
			RECT rozmiarEkranu;
			GetWindowRect(GetDesktopWindow(), &rozmiarEkranu);
				rozmiarOkna.x = rozmiarEkranu.right - rozmiarEkranu.left;
			rozmiarOkna.y = rozmiarEkranu.bottom - rozmiarEkranu.top;
		}
	}

	uchwytOkna = CreateWindow(
		nazwaOkna, 
		nazwaOkna, 
		stylOkna,
		polozenieOkna.x, polozenieOkna.y, 
		rozmiarOkna.x, rozmiarOkna.y, 
		NULL, 
		NULL, 
		uchwytAplikacji, 
		NULL 
		);

	if (uchwytOkna == NULL) return false;

	ShowWindow(uchwytOkna, SW_SHOW);
	UpdateWindow(uchwytOkna);

	return true;
}

bool Okno::ZmianaRozdzielczosci(long szerokosc, 
	long wysokosc, long glebiaKolorow) const
{
	DEVMODE dmScreenSettings; 
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); 
		dmScreenSettings.dmSize = sizeof(dmScreenSettings); 
		dmScreenSettings.dmPelsWidth = szerokosc; 
		dmScreenSettings.dmPelsHeight = wysokosc; 
		dmScreenSettings.dmBitsPerPel = glebiaKolorow; 
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}

WPARAM Okno::Uruchom()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}


//--------------------- WGL

bool OknoGL::UstalFormatPikseli(HDC uchwytDC) const
{
	PIXELFORMATDESCRIPTOR opisFormatuPikseli;
	ZeroMemory(&opisFormatuPikseli, sizeof(opisFormatuPikseli));
	opisFormatuPikseli.nVersion = 1;
	opisFormatuPikseli.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
				
	opisFormatuPikseli.iPixelType = PFD_TYPE_RGBA; 
	opisFormatuPikseli.cColorBits = 32;
				
	opisFormatuPikseli.cDepthBits = 32;
				
	opisFormatuPikseli.iLayerType = PFD_MAIN_PLANE;
	int formatPikseli = ChoosePixelFormat(uchwytDC, &opisFormatuPikseli);
	if (formatPikseli == 0) return false;
	if (!SetPixelFormat(uchwytDC, formatPikseli,&opisFormatuPikseli)) return false;
	return true;
}

bool OknoGL::InicjujWGL(HWND uchwytOkna)
{
	uchwytDC = ::GetDC(uchwytOkna);
	if (!UstalFormatPikseli(uchwytDC)) return false;

	HGLRC uchwytTymczasowegoRC = wglCreateContext(uchwytDC); 
					
	if (uchwytTymczasowegoRC == NULL) return false;
	if (!wglMakeCurrent(uchwytDC, uchwytTymczasowegoRC)) return false;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(NULL, "Inicjacja biblioteki GLEW nie powiod�a si�", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			return false;
	}

	const int major_min = 3; const int minor_min = 3;
	int major, minor;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	if (major<major_min || (major == major_min && minor<minor_min))
	{
		MessageBox(NULL, "Wersja OpenGL jest niewystarczaj�ca",
			"Aplikacja OpenGL", MB_OK | MB_ICONERROR);
		return false;
	}

	int atrybuty[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	uchwytRC = wglCreateContextAttribsARB(uchwytDC, 0, atrybuty);

	if (uchwytRC == NULL) return false;
	if (!wglMakeCurrent(uchwytDC, uchwytRC)) return false;
	wglDeleteContext(uchwytTymczasowegoRC);

	return true;
}

void OknoGL::UsunWGL()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(uchwytRC);
	::ReleaseDC(uchwytOkna, uchwytDC);
}


LRESULT OknoGL::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	long wynik = Okno::WndProc(hWnd, message, wParam, lParam);
	switch (message)
	{
	case WM_CREATE: 
		if (!InicjujWGL(hWnd))
		{
			MessageBox(NULL, "Pobranie kontekstu renderowania nie powiod�o si�", 
				"Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
		}
		idProgramuShaderow = PrzygotujShadery("Basic.vsh", "Basic.fsh", false);
		if (idProgramuShaderow == NULL)
		{
			MessageBox(NULL, "Przygotowanie shader�w nie powiod�o si�", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
				exit(EXIT_FAILURE);
		}
		UmiescInformacjeNaPaskuTytulu(hWnd);
		InicjujBuforWerteks�w();
		UstawienieSceny();
		break;
	case WM_DESTROY: 
		Usu�BuforWerteks�w();
		UsunWGL();
		break;
	case WM_SIZE: 
		UstawienieSceny();
		break;
	case WM_PAINT:
		RysujScene();
		ValidateRect(hWnd, NULL);
		break;
	}
	return wynik;
}

void OknoGL::UmiescInformacjeNaPaskuTytulu(HWND uchwytOkna)
{
	char bufor[256]; 
	GetWindowText(uchwytOkna, bufor, 256);
	const GLubyte* wersja = glGetString(GL_VERSION);
	strcat_s(bufor, " | OpenGL "); strcat_s(bufor, (char*)wersja);
	const GLubyte* dostawca = glGetString(GL_VENDOR);
	strcat_s(bufor, " | "); strcat_s(bufor, (char*)dostawca);
	const GLubyte* kartaGraficzna = glGetString(GL_RENDERER);
	strcat_s(bufor, " | "); strcat_s(bufor, (char*)kartaGraficzna);
	const GLubyte* wersjaGLEW = glewGetString(GLEW_VERSION);
	strcat_s(bufor, " | GLEW ");
	strcat_s(bufor, (char*)wersjaGLEW);
	const GLubyte* wersjaGLSL = glGetString(GL_SHADING_LANGUAGE_VERSION);
	strcat_s(bufor, " | GLSL ");
	strcat_s(bufor, (char*)wersjaGLSL);
	SetWindowText(uchwytOkna, bufor);
}


// ------------------------ OpenGL

void OknoGL::UstawienieSceny(bool rzutowanieIzometryczne) 
{
	glViewport(0, 0, szerokoscObszaruUzytkownika, wysokoscObszaruUzytkownika);
	/*// okno OpenGL = wn�trze formy(domy�lnie)
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	float wsp = wysokoscObszaruUzytkownika / (float)szerokoscObszaruUzytkownika;
	if (!rzutowanieIzometryczne)
		//left, right, boflom, top, znear, zfar(clipping)
		//mnozenie macierzy rzutowania przez macierz perspektywy - ustalanie frustum 
		glFrustum(-1.0f, 1.0f, wsp*-1.0f, wsp*1.0f, 1.0f, 10.0f);
	else
		glOrtho(-1.0f, 1.0f, wsp *-1.0f, wsp*1.0f, 1.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW); // powr�t do macierzy widoku modelu
	glEnable(GL_DEPTH_TEST);
	//z-buffer aktywny = ukrywanie niewidocznych powierzchni

	glFrontFace(GL_CCW); //odtworzenie ustawie� domy�lnych
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glCullFace(GL_BACK); //odtworzenie ustawie� domy�lnych
	glEnable(GL_CULL_FACE);

	GLint parametrUwzglednijKolorWerteksu = glGetUniformLocation(idProgramuShaderow, "UwzglednijKolorWerteksu");

	glUniform1i(parametrUwzglednijKolorWerteksu, true);

	GLint parametrKolor = glGetUniformLocation(idProgramuShaderow, "Kolor");

	float kolor[4] = { 1.0, 0.0, 0.0, 1.0 };

	glUniform4fv(parametrKolor, 1, kolor);*/
	//Parametry shadera

	GLint parametrUwzglednijKolorWerteksu = glGetUniformLocation(idProgramuShaderow, "UwzglednijKolorWerteksu");

	glUniform1i(parametrUwzglednijKolorWerteksu, true);

	GLint parametrKolor = glGetUniformLocation(idProgramuShaderow, "Kolor");

	float kolor[4] = { 1.0, 1.0, 0.0, 1.0 };

	glUniform4fv(parametrKolor, 1, kolor);

	GLint parametrMacierz�wiata = glGetUniformLocation(idProgramuShaderow, "macierzSwiata");

	float macierz�wiata[16] =

	{

	1.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 0.0f, 1.0f

	};

	glUniformMatrix4fv(parametrMacierz�wiata, 1, true, macierz�wiata);

	GLint parametrMacierzWidoku = glGetUniformLocation(idProgramuShaderow, "macierzWidoku");

	float macierzWidoku[16] =

	{

	1.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f, -3.0f,

	0.0f, 0.0f, 0.0f, 1.0f

	};

	glUniformMatrix4fv(parametrMacierzWidoku, 1, true, macierzWidoku);

	GLint parametrMacierzRzutowania = glGetUniformLocation(idProgramuShaderow, "macierzRzutowania");

	float macierzRzutowania[16] =

	{

	1.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.41f, 0.0f, 0.0f,

	0.0f, 0.0f, -1.22f, -2.22f,

	0.0f, 0.0f, -1.0f, 0.0f

	};

	glUniformMatrix4fv(parametrMacierzRzutowania, 1, true, macierzRzutowania);

}


void OknoGL::RysujScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	glLoadIdentity();    
	glTranslatef(0.0f, 0.0f, -3.0f);  


	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);

	SwapBuffers(uchwytDC);
}


void OknoGL::InicjujBuforWerteks�w()
{
	const float x0 = 1.0f;
	const float y0 = 1.0f;
	const float z0 = 1.0f;
	Werteks werteksy[] =
	{
		Werteks(-x0, -y0, 0.0f, 1, 1, 0, 1), 
		Werteks(x0, -y0, 0.0f, 1, 0, 1, 1), 
		Werteks(0, y0, 0.0f, 0, 1, 1, 1), 
		Werteks(-x0, y0, 0.0f, 0, 1, 1, 1),
		Werteks(x0, y0, 0.0f, 1, 1, 1, 1) 
	};
	//-----------------------
	//Vertex Array Object (VAO)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Vertex Buffer Object (VBO)
	glGenBuffers(3, &vbo[0]); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(werteksy), werteksy,
		GL_STATIC_DRAW);
	GLuint atrybutPo�o�enie = 0;
	GLuint atrybutKolor = 3;
	glVertexAttribPointer(atrybutPo�o�enie,
		Werteks::liczbaWsp�rz�dnychPo�o�enia,
		GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, 0);
	glEnableVertexAttribArray(atrybutPo�o�enie);
	glVertexAttribPointer(atrybutKolor,
		Werteks::liczbaSk�adowychKoloru,
		GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu,
		(const GLvoid*) Werteks::rozmiarWektoraPo�o�enia);
	glEnableVertexAttribArray(atrybutKolor);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	//-----------------------

	//GLubyte indeksy[] = { 0, 3, 1, 4 };	
	GLubyte indeksy[] = { 0, 1, 3, 4 };		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeksy),
		indeksy, GL_STATIC_DRAW);
}


void OknoGL::Usu�BuforWerteks�w()
{
	glDeleteBuffers(3, vbo);
	glDeleteVertexArrays(1, &vao);
}

#pragma region Shadery
void Poka�Komunikat(char* tekst, UINT typ = 0)
{
	MessageBoxA(NULL, tekst, "Aplikacja OpenGL � Kompilacja shader�w", MB_OK | typ);
}

#include <fstream>
char* CzytajPlikTxt(const char* nazwaPliku, int rozmiarBufora, char* bufor)
{
	std::ifstream plik_we(nazwaPliku);
	if (plik_we.fail()) return NULL;
	plik_we.read(bufor, rozmiarBufora);
	if (plik_we.eof())
	{
		std::streamsize odczytane = plik_we.gcount();
		plik_we.close();
		bufor[odczytane] = '\0';
		return bufor;
	}
	else
	{
		plik_we.close();
		return NULL;
	}
}

GLuint OknoGL::KompilujShader(const char* nazwaPliku, GLenum typ, bool trybDebugowania)
{
	const int maksymalnaWielkoscKodu = 65535;

	char kodShadera[maksymalnaWielkoscKodu] = "";
	if (CzytajPlikTxt(nazwaPliku, maksymalnaWielkoscKodu, kodShadera) == NULL)
	{
		Poka�Komunikat("Brak lub niemo�liwe odczytanie pliku shadera", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat(kodShadera);

	GLuint idShadera = glCreateShader(typ);
	if (idShadera == NULL) return NULL;

	const GLchar* zrodlo[1];
	zrodlo[0] = kodShadera;
	glShaderSource(idShadera, 1, zrodlo, NULL);

	glCompileShader(idShadera);

	GLint powodzenie;
	glGetShaderiv(idShadera, GL_COMPILE_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetShaderInfoLog(idShadera, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Kompilacja shadera nie powiod�a si�:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka�Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat("Kompilacja shadera zako�czy�a si� sukcesem", MB_ICONINFORMATION);

	return idShadera;
}

unsigned int OknoGL::PrzygotujShadery(const char* vsNazwaPliku, const char* fsNazwaPliku, bool trybDebugowania)
{
	GLuint idShaderaWerteks�w = KompilujShader(vsNazwaPliku, GL_VERTEX_SHADER, trybDebugowania);
	if (idShaderaWerteks�w == NULL)
	{
		Poka�Komunikat("Kompilacja shadera werteks�w nie powiod�a si�", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat("Kompilacja shadera werteks�w zako�czy�a si� sukcesem", MB_ICONINFORMATION);

	GLuint idShaderaFragment�w = KompilujShader(fsNazwaPliku, GL_FRAGMENT_SHADER, trybDebugowania);
	if (idShaderaFragment�w == NULL)
	{
		Poka�Komunikat("Kompilacja shadera fragment�w nie powiod�a si�", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat("Kompilacja shadera fragment�w zako�czy�a si� sukcesem", MB_ICONINFORMATION);

	GLuint idProgramu = glCreateProgram();

	glAttachShader(idProgramu, idShaderaWerteks�w);
	glAttachShader(idProgramu, idShaderaFragment�w);

	glLinkProgram(idProgramu);

	GLint powodzenie;
	glGetProgramiv(idProgramu, GL_LINK_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Linkowanie programu shader�w nie powiod�o si�:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka�Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat("Linkowanie programu shader�w powiod�o si�", MB_ICONINFORMATION);

	glValidateProgram(idProgramu);

	glGetProgramiv(idProgramu, GL_VALIDATE_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Walidacja programu shader�w nie powiod�a si�:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka�Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka�Komunikat("Walidacja programu shader�w powiod�a si�", MB_ICONINFORMATION);

	glUseProgram(idProgramu);

	glDeleteShader(idShaderaWerteks�w);
	glDeleteShader(idShaderaFragment�w);

	return idProgramu;
}
#pragma endregion