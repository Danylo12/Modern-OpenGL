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
		MessageBox(NULL, "Inicjacja okna nie powiodla siê",
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
	case WM_DESTROY://zamykanie okna => konczenie aplikacji
		PostQuitMessage(0);
		break;
	case WM_SIZE: // Zmiana rozmiaru okna
		RECT rect;
		GetClientRect(hWnd, &rect);
		szerokoscObszaruUzytkownika = rect.right - rect.left;
		wysokoscObszaruUzytkownika = rect.bottom - rect.top;
		break;

	default: // automatyczne przetwarzanie komunikatów 
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
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // styl okna
	wc.lpfnWndProc = (WNDPROC)::WndProc; // procedura okna
	wc.cbClsExtra = 0; // dodatkowe bajty za rezerwowane za klas¹ okna
	wc.cbWndExtra = 0; // dodatkowe bajty zarezerwowane za instancj¹ okna
	wc.hInstance = uchwytAplikacji; // instancja aplikacji 
	wc.hIcon = LoadIcon(uchwytAplikacji, MAKEINTRESOURCE(IDI_ICON1)); //uchwyt ikony 
	wc.hIconSm = LoadIcon(uchwytAplikacji, MAKEINTRESOURCE(IDI_ICON1)); //uchwyt ikony 


	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // uchwyt kursora
	wc.hbrBackground = NULL; // uchwyt pêdzla t³a 
	wc.lpszMenuName = NULL; // nazwa menu
	wc.lpszClassName = nazwaOkna; // nazwa klasy okna

	// Rejestracja klasy okna
	if (RegisterClassEx(&wc) == 0) return false;

	//ustawienia dla okna
	DWORD stylOkna = WS_OVERLAPPEDWINDOW;
	
	if (trybPelnoekranowy)
	{
		//ustawienia dla trybu pe³noekranowego
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

	// Tworzenie okna
	uchwytOkna = CreateWindow(
		nazwaOkna, // nazwa klasy okna
		nazwaOkna, // nazwa okna
		stylOkna, // styl okna
		polozenieOkna.x, polozenieOkna.y, // po³o¿enie okna(x, y)
		rozmiarOkna.x, rozmiarOkna.y, // rozmiar okna szerokoœæ, wysokoœæ)
		NULL, // uchwyt okna nadrzêdnego(parent)
		NULL, // uchwyt menu
		uchwytAplikacji, // uchwyt instancji aplikacji
		NULL // parametr komunikatu informuj¹cego o utworzeniu okna
		);

	if (uchwytOkna == NULL) return false;

	// Pokazanie i aktualizacja okna
	ShowWindow(uchwytOkna, SW_SHOW);
	UpdateWindow(uchwytOkna);

	return true;
}

bool Okno::ZmianaRozdzielczosci(long szerokosc, 
	long wysokosc, long glebiaKolorow) const
{
	DEVMODE dmScreenSettings; //struktura trybu wyœwietlania
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); //czyszczenie pamiêci
		dmScreenSettings.dmSize = sizeof(dmScreenSettings); //rozmiar struktury
		dmScreenSettings.dmPelsWidth = szerokosc; //nowa szerokoœæ ekranu
		dmScreenSettings.dmPelsHeight = wysokosc; //nowa wysokoœæ ekranu
		dmScreenSettings.dmBitsPerPel = glebiaKolorow; //liczba bitów na piksel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}

WPARAM Okno::Uruchom()
{
	// Pêtla g³ówna - obs³uga komunikatów
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
				// w oknie, podwójne buforowanie
	opisFormatuPikseli.iPixelType = PFD_TYPE_RGBA; // typ koloru RGB z kana³em alfa
	opisFormatuPikseli.cColorBits = 32;
				// jakoœæ kolorów 4 bajty(po bajcie na ka¿dy kana³)
	opisFormatuPikseli.cDepthBits = 32;
				// g³êbokoœæ bufora g³êbi(z - buffer)
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
					//tworzenie tymczasowego kontekstu renderowania (tradycyjnie)
	if (uchwytTymczasowegoRC == NULL) return false;
	if (!wglMakeCurrent(uchwytDC, uchwytTymczasowegoRC)) return false;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(NULL, "Inicjacja biblioteki GLEW nie powiod³a siê", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			return false;
	}

	const int major_min = 3; const int minor_min = 3;
	int major, minor;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	if (major<major_min || (major == major_min && minor<minor_min))
	{
		MessageBox(NULL, "Wersja OpenGL jest niewystarczaj¹ca",
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
	case WM_CREATE: // Utworzenie okna
		// zmienna uchwytOkna nie jest jeszcze zainicjowana
		if (!InicjujWGL(hWnd))
		{
			MessageBox(NULL, "Pobranie kontekstu renderowania nie powiod³o siê", 
				"Aplikacja OpenGL", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
		}
		idProgramuShaderow = PrzygotujShadery("Basic.vsh", "Basic.fsh", false);
		if (idProgramuShaderow == NULL)
		{
			MessageBox(NULL, "Przygotowanie shaderów nie powiod³o siê", "Aplikacja OpenGL", MB_OK | MB_ICONERROR);
				exit(EXIT_FAILURE);
		}
		UmiescInformacjeNaPaskuTytulu(hWnd);
		InicjujBuforWerteksów();
		UstawienieSceny();
		break;
	case WM_DESTROY: // Zamkniêcie okna
		UsuñBuforWerteksów();
		UsunWGL();
		break;
	case WM_SIZE: //Zmiana rozmiaru okna
		UstawienieSceny();
		break;
	case WM_PAINT: //Okno wymaga odrysowania
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

void OknoGL::UstawienieSceny(bool rzutowanieIzometryczne) // wartoœæ domyœlna false
{
	glViewport(0, 0, szerokoscObszaruUzytkownika, wysokoscObszaruUzytkownika);
	/*// okno OpenGL = wnêtrze formy(domyœlnie)
	// ustawienie punktu projekcji
	glMatrixMode(GL_PROJECTION); // prze³¹czenie na macierz projekcji
	glLoadIdentity();

	float wsp = wysokoscObszaruUzytkownika / (float)szerokoscObszaruUzytkownika;
	if (!rzutowanieIzometryczne)
		//left, right, boflom, top, znear, zfar(clipping)
		//mnozenie macierzy rzutowania przez macierz perspektywy - ustalanie frustum 
		glFrustum(-1.0f, 1.0f, wsp*-1.0f, wsp*1.0f, 1.0f, 10.0f);
	else
		glOrtho(-1.0f, 1.0f, wsp *-1.0f, wsp*1.0f, 1.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW); // powrót do macierzy widoku modelu
	glEnable(GL_DEPTH_TEST);
	//z-buffer aktywny = ukrywanie niewidocznych powierzchni

	glFrontFace(GL_CCW); //odtworzenie ustawieñ domyœlnych
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glCullFace(GL_BACK); //odtworzenie ustawieñ domyœlnych
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

	GLint parametrMacierzŒwiata = glGetUniformLocation(idProgramuShaderow, "macierzSwiata");

	float macierzŒwiata[16] =

	{

	1.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 0.0f, 1.0f

	};

	glUniformMatrix4fv(parametrMacierzŒwiata, 1, true, macierzŒwiata);

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
	// Przygotowanie buforów
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //czyœci bufory

	glLoadIdentity();    //macierz model-widok = macierz jednostkowa
	glTranslatef(0.0f, 0.0f, -3.0f);  

	//Rysowanie trójk¹ta

	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);

	//z bufora na ekran
	SwapBuffers(uchwytDC);
}


void OknoGL::InicjujBuforWerteksów()
{
	const float x0 = 1.0f;
	const float y0 = 1.0f;
	const float z0 = 1.0f;
	Werteks werteksy[] =
	{
		Werteks(-x0, -y0, 0.0f, 1, 1, 0, 1), //dolny lewy
		Werteks(x0, -y0, 0.0f, 1, 0, 1, 1), //dolny prawy
		Werteks(0, y0, 0.0f, 0, 1, 1, 1), //górny
		Werteks(-x0, y0, 0.0f, 0, 1, 1, 1), //górny lewy
		Werteks(x0, y0, 0.0f, 1, 1, 1, 1) //górny prawy
	};
	//-----------------------
	//Vertex Array Object (VAO)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Vertex Buffer Object (VBO)
	glGenBuffers(3, &vbo[0]); //tworzy bufory
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(werteksy), werteksy,
		GL_STATIC_DRAW);
	GLuint atrybutPo³o¿enie = 0;
	GLuint atrybutKolor = 3;
	glVertexAttribPointer(atrybutPo³o¿enie,
		Werteks::liczbaWspó³rzêdnychPo³o¿enia,
		GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu, 0);
	glEnableVertexAttribArray(atrybutPo³o¿enie);
	glVertexAttribPointer(atrybutKolor,
		Werteks::liczbaSk³adowychKoloru,
		GL_FLOAT, GL_FALSE, Werteks::rozmiarWerteksu,
		(const GLvoid*) Werteks::rozmiarWektoraPo³o¿enia);
	glEnableVertexAttribArray(atrybutKolor);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	//-----------------------

	//GLubyte indeksy[] = { 0, 3, 1, 4 };	//trójk¹ty ustawione ty³em
	GLubyte indeksy[] = { 0, 1, 3, 4 };		//trójk¹ty ustawione przodem
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeksy),
		indeksy, GL_STATIC_DRAW);
}


void OknoGL::UsuñBuforWerteksów()
{
	glDeleteBuffers(3, vbo);
	glDeleteVertexArrays(1, &vao);
}

#pragma region Shadery
void Poka¿Komunikat(char* tekst, UINT typ = 0)
{
	MessageBoxA(NULL, tekst, "Aplikacja OpenGL – Kompilacja shaderów", MB_OK | typ);
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

	//Wczytywanie kodu z pliku
	char kodShadera[maksymalnaWielkoscKodu] = "";
	if (CzytajPlikTxt(nazwaPliku, maksymalnaWielkoscKodu, kodShadera) == NULL)
	{
		Poka¿Komunikat("Brak lub niemo¿liwe odczytanie pliku shadera", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat(kodShadera);

	//Tworzenie obiektu shadera
	GLuint idShadera = glCreateShader(typ);
	if (idShadera == NULL) return NULL;

	//Dostarczanie zrodla do obiektu shadera
	const GLchar* zrodlo[1];
	zrodlo[0] = kodShadera;
	glShaderSource(idShadera, 1, zrodlo, NULL);

	//Kompilacja shadera
	glCompileShader(idShadera);

	//Weryfikacje kompilacji
	GLint powodzenie;
	glGetShaderiv(idShadera, GL_COMPILE_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetShaderInfoLog(idShadera, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Kompilacja shadera nie powiod³a siê:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka¿Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat("Kompilacja shadera zakoñczy³a siê sukcesem", MB_ICONINFORMATION);

	return idShadera;
}

unsigned int OknoGL::PrzygotujShadery(const char* vsNazwaPliku, const char* fsNazwaPliku, bool trybDebugowania)
{
	//Kompilacja shadera werteksów
	GLuint idShaderaWerteksów = KompilujShader(vsNazwaPliku, GL_VERTEX_SHADER, trybDebugowania);
	if (idShaderaWerteksów == NULL)
	{
		Poka¿Komunikat("Kompilacja shadera werteksów nie powiod³a siê", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat("Kompilacja shadera werteksów zakoñczy³a siê sukcesem", MB_ICONINFORMATION);

	//Kompilacja shadera fragmentów
	GLuint idShaderaFragmentów = KompilujShader(fsNazwaPliku, GL_FRAGMENT_SHADER, trybDebugowania);
	if (idShaderaFragmentów == NULL)
	{
		Poka¿Komunikat("Kompilacja shadera fragmentów nie powiod³a siê", MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat("Kompilacja shadera fragmentów zakoñczy³a siê sukcesem", MB_ICONINFORMATION);

	//Tworzenie obiektu programu
	GLuint idProgramu = glCreateProgram();

	//Przylaczanie shaderow
	glAttachShader(idProgramu, idShaderaWerteksów);
	glAttachShader(idProgramu, idShaderaFragmentów);

	//Linkowanie
	glLinkProgram(idProgramu);

	//Weryfikacja linkowania
	GLint powodzenie;
	glGetProgramiv(idProgramu, GL_LINK_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Linkowanie programu shaderów nie powiod³o siê:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka¿Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat("Linkowanie programu shaderów powiod³o siê", MB_ICONINFORMATION);

	glValidateProgram(idProgramu);

	//Walidacja programu
	glGetProgramiv(idProgramu, GL_VALIDATE_STATUS, &powodzenie);
	if (!powodzenie)
	{
		const int maxInfoLogSize = 2048;
		GLchar infoLog[maxInfoLogSize];
		glGetProgramInfoLog(idProgramu, maxInfoLogSize, NULL, infoLog);
		char komunikat[maxInfoLogSize + 64] = "Uwaga! Walidacja programu shaderów nie powiod³a siê:\n";
		strcat_s(komunikat, (char*)infoLog);
		Poka¿Komunikat(komunikat, MB_ICONERROR);
		return NULL;
	}
	else if (trybDebugowania) Poka¿Komunikat("Walidacja programu shaderów powiod³a siê", MB_ICONINFORMATION);

	//Uzycie programu
	glUseProgram(idProgramu);

	//Usuwanie niepotrzebnych obiektow shadera
	glDeleteShader(idShaderaWerteksów);
	glDeleteShader(idShaderaFragmentów);

	return idProgramu;
}
#pragma endregion