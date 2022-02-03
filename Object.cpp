#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Object
{
private:
	int ilosc_danych;//dane do renderu w bajtach
	int liczba_vertexow;//liczba vertexow do narysowania
	float *ptr_dane_do_renderu;

	unsigned int textures[2];
	unsigned int *tex[2];
	
public:
	const Object()
	{
		liczba_vertexow = 0;
	}

	void load_objn_mesh(string filename)
	{

		string wiersz;//wiersz danych w formie string
		string x, y, z;//wsp�rz�dne vertexa w formie string
		string u, v;// wsp�rz�dne uv w formie string
		string xn, yn, zn;//wsp�rz�dne wektora normalnego w formie string
		string wiersz_poz_1vert, wiersz_poz_2vert, wiersz_poz_3vert;// numery wierszy w .obj dla pozycji vertex�w danego tr�jk�ta
		string wiersz_uv_1vert, wiersz_uv_2vert, wiersz_uv_3vert;//numery wierszy w .obj dla wsp�rz�dnych uv vertex�w danego tr�jk�ta
		string wiersz_normalnego_1vert, wiersz_normalnego_2vert, wiersz_normalnego_3vert;//numery wierszy w .obj dla wektor�w normalnych vertex�w danego tr�jk�ta
		string niepotrzebny;//niepotrzebny fragment wiersza z .obj
		ifstream file;
		int numer_pierwszego_wiersza_poz_vert = 3;
		int liczba_wszyst_floatow_do_renderu = 0;

		int liczba_wierszy_pos = 0;
		int liczba_wierszy_uv = 0;
		int liczba_wierszy_normal = 0;
		bool vertexy = true;
		bool uv_ki = true;
		bool normalne = true;
		bool trojkaty = true;


		file.open(filename);
		if (file.good() == false)
		{
			cout << "Nie mozna otworzyc pliku!";
		}

		for (int index = 0; index < numer_pierwszego_wiersza_poz_vert; index++)//pierwsze 3 wiersze w .obj nas nie interesuj� dlatego przechodzimy przez nie do pierwszego vertexa w .obj
		{
			getline(file, wiersz);
		}

		//obliczamy ile jest wierszy z pozycjami vertex�w
		while (vertexy)
		{
			getline(file, wiersz);//pobieramy wiersz danych
			if (wiersz.find("vt") != string::npos)//je�li znajdziemy "vt" to if si� wykona. Funkcja Find() zwraca pozycj� szukanego wyra�enia "vt" w "wiersz", je�li 
			{									  //nie znajdzie szukanego wyra�enia to zwraca "string::npos"
				vertexy = false;
				liczba_wierszy_uv++;//zaczynamy liczy� wiersze z wsp�rz�dnymi "uv"
			}
			else
			{
				liczba_wierszy_pos++;//liczba wierszy z pozycjami vertex�w
			}

		}
		//kontynuujemy liczenie wierszy ze wsp�rz�dnymi "uv"
		while (uv_ki)
		{
			getline(file, wiersz);
			if (wiersz.find("vn") != string::npos)//wykonujemy to samo co wcze�niej z t� r�nic�, �e tutaj szukamy "vn"
			{
				uv_ki = false;
				liczba_wierszy_normal++;//zaczynamy liczy� wiersze z wektorami normalnymi
			}
			else
			{
				liczba_wierszy_uv++;//liczba wierszy z "uv"
			}

		}
		//kontynuujemy liczenie wierszy z wektorami normalnymi
		while (normalne)//obliczanie poz pocz uv i pamieci dla v_list
		{
			getline(file, wiersz);
			if (wiersz.find("s off") != string::npos)
			{
				normalne = false;
			}
			else
			{
				liczba_wierszy_normal++;//kontynuacja liczenia wierszy z normalnymi
			}

		}

		while (trojkaty)//obliczanie potrzebnych pamieci do zarezerwowania
		{
			getline(file, wiersz);
			if (wiersz.find("f") != string::npos)
			{
				liczba_wszyst_floatow_do_renderu += 24;//24 floaty/tr�jk�t. Jeden vertex to (x, y, z, u, v, xn, yn, zn), tr�jk�t ma 3 vertexy
				liczba_vertexow += 3;// +3 bo s� 3 vertexy/tr�jk�t
			}
			else
			{
				trojkaty = false;
			}

		}

		ptr_dane_do_renderu = new float[liczba_wszyst_floatow_do_renderu];
		int *ptr_numery_wierszy_poz = new int[liczba_vertexow];
		int *ptr_numery_wierszy_uv = new int[liczba_vertexow];
		int *ptr_numery_wierszy_norm = new int[liczba_vertexow];
		string *ptr_v_string = new string[liczba_wierszy_pos];
		string *ptr_uv_string = new string[liczba_wierszy_uv];
		string *ptr_vn_string = new string[liczba_wierszy_normal];

		file.clear();
		file.seekg(0);//przechodzimy na pocz�tek danych pliku .obj

		for (int index = 0; index < numer_pierwszego_wiersza_poz_vert; index++)//pierwsze 3 wiersze w .obj nas nie interesuj� dlatego przechodzimy przez nie do pierwszego vertexa w .obj
		{
			getline(file, wiersz);
		}

		//pobieranie z .obj wierszy z danymi pozycji  w formie string i umieszczanie ich w tablicy typu string ptr_v_string
		for (int index = 0; index < liczba_wierszy_pos; index++)//b�d�c na pozycji pierwszego wiersza pozycji vertexa w.obj pobieram wiersz po wierszu 
		{
			getline(file, ptr_v_string[index]);
		}

		//pobieranie z .obj wierszy z danymi uv  w formie string i umieszczanie ich w tablicy typu string ptr_uv_string
		for (int index = 0; index < liczba_wierszy_uv; index++)// to samo co z wierszami pozyccji vertex�w robimy z wierszami uv
		{
			getline(file, ptr_uv_string[index]);
		}

		//pobieranie z .obj wierszy z danymi normalnymi  w formie string i umieszczanie ich w tablicy typu string ptr_vn_string
		for (int index = 0; index < liczba_wierszy_normal; index++)
		{
			getline(file, ptr_vn_string[index]);
		}

		getline(file, wiersz);
		for (int index = 0; index < liczba_vertexow; index += 3)
		{
			//pobranie z .obj wiersza z numerami wierszy dla pozycji, uv-ki, wektora normalnego trzech vertex�w danego tr�jk�ta
			getline(file, wiersz);
			stringstream trojkat_string(wiersz);//pobieranie z pobranego wiersza samych numer�w wierszy dla pozycji, uv-ki, wektora normalnego trzech vertex�w danego tr�jk�ta
			getline(trojkat_string, niepotrzebny, ' ');
			getline(trojkat_string, wiersz_poz_1vert, '/');
			getline(trojkat_string, wiersz_uv_1vert, '/');
			getline(trojkat_string, wiersz_normalnego_1vert, ' ');
			getline(trojkat_string, wiersz_poz_2vert, '/');
			getline(trojkat_string, wiersz_uv_2vert, '/');
			getline(trojkat_string, wiersz_normalnego_2vert, ' ');
			getline(trojkat_string, wiersz_poz_3vert, '/');
			getline(trojkat_string, wiersz_uv_3vert, '/');
			getline(trojkat_string, wiersz_normalnego_3vert, '\n');

			//1 vertex tr�jk�ta
			ptr_numery_wierszy_poz[index] = stoi(wiersz_poz_1vert) - 1;//odejmujemy 1, poniewa� pozycje wierszy w .obj liczone s� od 1, a my b�dziemy u�ywa� tych pozycji jako index�w w tablicach
			ptr_numery_wierszy_uv[index] = stoi(wiersz_uv_1vert) - 1;// funkcja stoi konwertuje string na int
			ptr_numery_wierszy_norm[index] = stoi(wiersz_normalnego_1vert) - 1;

			//2 vertex tr�jk�ta
			ptr_numery_wierszy_poz[index + 1] = stoi(wiersz_poz_2vert) - 1;
			ptr_numery_wierszy_uv[index + 1] = stoi(wiersz_uv_2vert) - 1;
			ptr_numery_wierszy_norm[index + 1] = stoi(wiersz_normalnego_2vert) - 1;

			//3 vertex tr�jk�ta
			ptr_numery_wierszy_poz[index + 2] = stoi(wiersz_poz_3vert) - 1;
			ptr_numery_wierszy_uv[index + 2] = stoi(wiersz_uv_3vert) - 1;
			ptr_numery_wierszy_norm[index + 2] = stoi(wiersz_normalnego_3vert) - 1;
		}

		for (int index = 0; index < liczba_vertexow; index++)//dla kazdego vertexa
		{
			stringstream v_string(ptr_v_string[ptr_numery_wierszy_poz[index]]);//tutaj wida� dlaczego odejmowali�my 1 od wszystkich pozycji danych 
			v_string >> niepotrzebny >> x >> y >> z;// ze v_string przenosimy kolejno dane do niepotrzebny  x  y  z. Dane s� przenoszone po kolei st�d konieczno�� zmiennej "niepotrzebny"
			ptr_dane_do_renderu[(index * 8)] = stof(x);// funkcja stof konwertuje string na float
			ptr_dane_do_renderu[(index * 8) + 1] = stof(y);
			ptr_dane_do_renderu[(index * 8) + 2] = stof(z);

			stringstream uv_string(ptr_uv_string[ptr_numery_wierszy_uv[index]]);
			uv_string >> niepotrzebny >> u >> v;
			ptr_dane_do_renderu[(index * 8) + 3] = stof(u);
			ptr_dane_do_renderu[(index * 8) + 4] = stof(v);

			stringstream vn_string(ptr_vn_string[ptr_numery_wierszy_norm[index]]);
			vn_string >> niepotrzebny >> xn >> yn >> zn;
			ptr_dane_do_renderu[(index * 8) + 5] = stof(xn);
			ptr_dane_do_renderu[(index * 8) + 6] = stof(yn);
			ptr_dane_do_renderu[(index * 8) + 7] = stof(zn);
		}
		delete[] ptr_numery_wierszy_poz;
		delete[] ptr_numery_wierszy_uv;
		delete[] ptr_numery_wierszy_norm;
		delete[] ptr_v_string;
		delete[] ptr_uv_string;
		delete[] ptr_vn_string;

		file.seekg(0);
		file.close();

		ilosc_danych = liczba_wszyst_floatow_do_renderu * 4;
	}

	void Load_Texture(const char *fname, int id)//wczytywanie tekstury
	{
		glGenTextures(1, &textures[id]);
		tex[id] = &textures[id];

		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width = 0, height = 0, nrChannels = 0;
		unsigned char *data = stbi_load(fname, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void draw_mesh(int id)
	{	
		int draw_vertices = ilosc_danych / 32;
		glBufferData(GL_ARRAY_BUFFER, ilosc_danych, ptr_dane_do_renderu, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, *tex[id]);
		glDrawArrays(GL_TRIANGLES, 0, draw_vertices);
	}
};








