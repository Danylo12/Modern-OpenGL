struct Werteks
{
	float x, y, z; //wspó³rzêdne po³ozenia
	float r, g, b, a; //sk³adowe koloru
	static const int liczbaWspó³rzêdnychPo³o¿enia = 3;
	static const int rozmiarWektoraPo³o¿enia =
		liczbaWspó³rzêdnychPo³o¿enia * sizeof(float);
	static const int liczbaSk³adowychKoloru = 4;
	static const int rozmiarWektoraKoloru =
		liczbaSk³adowychKoloru * sizeof(float);
	static const int rozmiarWerteksu =
		rozmiarWektoraPo³o¿enia + rozmiarWektoraKoloru;
	Werteks(float x, float y, float z,
		float r, float g, float b, float a = 1.0f)
		: x(x), y(y), z(z),
		r(r), g(g), b(b), a(a)
	{
	}
};