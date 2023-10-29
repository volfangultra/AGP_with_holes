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

struct Vertex {
	double x, y;
    const int SIZE = 2;
	Vertex(double x = 0, double y = 0): x(x), y(y){}
	void draw(TImage*, TColor color = clRed);
	Vertex& operator=(const Vertex &);
    Vertex rotate(Vertex, float);
};

//---------------------------------------------------------------------------

bool operator<(Vertex, Vertex);
bool operator==(Vertex, Vertex);
int orientation(Vertex, Vertex, Vertex);

//---------------------------------------------------------------------------

struct Segment {
    Vertex A,B;
	Segment(Vertex A, Vertex B): A(A), B(B) {
		if (B < A) {
			swap(this->A, this->B);
		}
	}
	void draw(TImage*, TColor color = clBlue);
};

//---------------------------------------------------------------------------

struct Trougao {
    Vertex A, B, C;
    Trougao(Vertex A, Vertex B, Vertex C): A(A), B(B), C(C) {
        if (orientation(A, B, C) > 0) {
            swap(B, C);
        }
    }

};

//---------------------------------------------------------------------------

class Simple_polygon{
	vector<Vertex> vertices;
public:
	Simple_polygon():vertices(){};
	Simple_polygon(vector<Vertex> v):vertices(v){};
	void draw(TImage*, TColor color = clBlue);
	void fill_color(TImage*, TColor boja = clAqua);
	double surface_area();
	bool is_point_inside_poly(Vertex);
    bool is_convex();
	vector<Vertex> get_intersection_points(Vertex, Vertex);
	vector<Vertex> get_intersection_of_polygons(Simple_polygon&);
	friend class Tagp_aplication;
    friend class Camera;
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
    void fill_color(TImage*, TColor boja = clYellow);
    friend class Tagp_aplication;

};


//---------------------------------------------------------------------------
#endif
