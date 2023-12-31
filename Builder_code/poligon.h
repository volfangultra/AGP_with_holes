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
    double distance_to(Vertex&);
};
//---------------------------------------------------------------------------
bool operator<(Vertex, Vertex);
bool operator==(Vertex, Vertex);
bool operator!=(Vertex, Vertex);
int orientation(Vertex, Vertex, Vertex);
//---------------------------------------------------------------------------
struct Segment {
	Vertex A,B;
    Segment():A(), B() {};
	Segment(Vertex A, Vertex B): A(A), B(B) {
		if (B < A) {
			swap(this->A, this->B);
		}
	}
	void draw(TImage*, TColor color = clBlue);
};
//---------------------------------------------------------------------------
struct Triangle {
	Vertex A, B, C;
    Triangle(Vertex A, Vertex B, Vertex C): A(A), B(B), C(C) {
        if (orientation(A, B, C) > 0) {
            swap(B, C);
        }
	}
	void draw(TImage*, TColor line_color=clBlue);
	double calculateArea();
	static bool compareByArea(Triangle t1, Triangle t2);
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
    friend void intersect(Simple_polygon&, Simple_polygon&);
	friend class Tagp_aplication;
    friend class Camera;
};
//---------------------------------------------------------------------------
class Camera{
	Simple_polygon view;
	float FILL_ANGLE;
	int RADIUS;
	const int ACCURACY = 100;
public:
	Camera(Simple_polygon v):view(v),FILL_ANGLE((2./3)*3.141592), RADIUS(160) {};
	Camera(Simple_polygon v, float fill_angle, int radius):view(v),FILL_ANGLE(fill_angle), RADIUS(radius) {};
	void draw(TImage*, TColor vertice_color=clGreen, TColor fill_color=clYellow);
	void fill_color(TImage*, TColor boja = clYellow);
	void hit(Simple_polygon&);
	void iscrtajKameru(int direction_x, int direction_y, Vertex center, TColor color, TImage* image, Simple_polygon& outside_polygon, vector<Simple_polygon>& holes);
	friend class Tagp_aplication;

};

//---------------------------------------------------------------------------
#endif
