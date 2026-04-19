struct Werteks
{
	float x, y, z; 
	float r, g, b, a; 
	static const int liczbaWsp�rz�dnychPo�o�enia = 3;
	static const int rozmiarWektoraPo�o�enia =
		liczbaWsp�rz�dnychPo�o�enia * sizeof(float);
	static const int liczbaSk�adowychKoloru = 4;
	static const int rozmiarWektoraKoloru =
		liczbaSk�adowychKoloru * sizeof(float);
	static const int rozmiarWerteksu =
		rozmiarWektoraPo�o�enia + rozmiarWektoraKoloru;
	Werteks(float x, float y, float z,
		float r, float g, float b, float a = 1.0f)
		: x(x), y(y), z(z),
		r(r), g(g), b(b), a(a)
	{
	}
};