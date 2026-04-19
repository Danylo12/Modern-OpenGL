#ifndef OPENGL_H 
#define OPENGL_H 

#define W1N32_LEAN_AND_MEAN 
#include <windows.h> 

class Okno
{
private:
	bool ZmianaRozdzielczosci(long szerokosc, long wysokosc,
		long glebiaKolorow = 32) const;

protected:
	HWND uchwytOkna;
	long szerokoscObszaruUzytkownika, wysokoscObszaruUzytkownika;

public:
	Okno() :uchwytOkna(NULL){};
	bool Inicjuj(HINSTANCE uchwytAplikacji, POINT polozenieOkna, POINT rozmiarOkna,
		bool trybPelnoekranowy = false,	bool zmianaRozdzielczosci = true);
	WPARAM Uruchom();
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

class OknoGL : public Okno
{
private:
	HGLRC uchwytRC; 
	HDC uchwytDC; 
	bool UstalFormatPikseli(HDC uchwytDC) const;
	bool InicjujWGL(HWND uchwytOkna);
	void UsunWGL();
	void UmiescInformacjeNaPaskuTytulu(HWND uchwytOkna);

	unsigned int vao;
	unsigned int vbo[3];
	void InicjujBuforWerteks�w();
	void Usu�BuforWerteks�w();

	unsigned int idProgramuShaderow;
	static unsigned int KompilujShader(
		const char* nazwaPliku, unsigned int typ,
		bool trybDebugowania = false);
	static unsigned int PrzygotujShadery(
		const char* vsNazwaPliku,
		const char* fsNazwaPliku,
		bool trybDebugowania = false);

protected:
	void UstawienieSceny(bool rzutowanieIzometryczne = false);
	void RysujScene();
public:
	OknoGL() :Okno(), uchwytRC(NULL), uchwytDC(NULL) {};
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
} okno;


#endif 