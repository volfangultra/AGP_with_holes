//---------------------------------------------------------------------------

#ifndef poligonH
#define poligonH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <vector>

using namespace std;

struct Dot {
	double x, y;
    const int SIZE = 2;
	Dot(double x = 0, double y = 0): x(x), y(y){}
	void draw(TImage*, TColor color = clRed);
	Dot& operator=(const Dot &);
    Dot rotate(Dot, float);
};

//---------------------------------------------------------------------------

bool operator<(Dot, Dot);
bool operator==(Dot, Dot);
int orientation(Dot, Dot, Dot);

//---------------------------------------------------------------------------

struct Segment {
    Dot A,B;
	Segment(Dot A, Dot B): A(A), B(B) {
		if (B < A) {
			swap(this->A, this->B);
		}
	}
	void draw(TImage*, TColor color = clBlue);
};

//---------------------------------------------------------------------------

struct Trougao {
    Dot A, B, C;
    Trougao(Dot A, Dot B, Dot C): A(A), B(B), C(C) {
        if (orientation(A, B, C) > 0) {
            swap(B, C);
        }
    }

};

//---------------------------------------------------------------------------

class Simple_polygon{
	vector<Dot> vertices;
public:
	void draw(TImage*, TColor color = clBlue);
	void fill_color(TImage*, TColor boja = clAqua);
	double surface_area();//Za implementirat
    Simple_polygon intersect(Simple_polygon); //Za implementirat
	friend class Tagp_aplication;
};

//---------------------------------------------------------------------------

class Camera{
    Simple_polygon view;
	const float FILL_ANGLE = 1;
	const int RADIUS = 50;
	const int ACCURACY = 4;
public:
	Camera(Simple_polygon v):view(v){};
	void draw(TImage*, TColor vertice_color=clGreen, TColor fill_color=clYellow);
    friend class Tagp_aplication;

};


//---------------------------------------------------------------------------
#endif