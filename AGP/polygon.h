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

//For the program to work correctly camera and polygon must have different colors.
//These are color constants for visualization
const TColor DEFUALT_VERTEX_COLOR = clRed;
const TColor DEFUALT_SEGMENT_COLOR = clAqua;
const TColor DEFUALT_POLYGON_FILL_COLOR = clAqua;
const TColor DEFUALT_CAMERA_FILL_COLOR = clYellow;
const TColor DEFUALT_TRIANGLE_COLOR = clGreen;


//A class for Vertex
struct Vertex {
	double x, y;
	const int SIZE = 2; //Size of a ball in visulaziation
	Vertex(double x = 0, double y = 0): x(x), y(y){}
	void draw(TImage*, TColor color = DEFUALT_VERTEX_COLOR);
	Vertex& operator=(const Vertex &);
	Vertex rotate(Vertex, float); //rotates a point around this point for a given radians
	double distance_to(Vertex&); //calculates a distance from this point to a given point
};
//---------------------------------------------------------------------------

//We define < on Vertexes a and b to be true if point a has a smaller x coordinate
//than point b, if x coordinates are the same then we take a must have a smaller y coordinate
bool operator<(Vertex, Vertex);
bool operator==(Vertex, Vertex);
bool operator!=(Vertex, Vertex);
int orientation(Vertex, Vertex, Vertex);
//---------------------------------------------------------------------------

//A class for segment connecting two vertices
struct Segment {
	Vertex A,B;
	Segment():A(), B() {};
	Segment(Vertex A, Vertex B): A(A), B(B) {
		if (B < A) {
			swap(this->A, this->B);
		}
	}
	void draw(TImage*, TColor color = DEFUALT_SEGMENT_COLOR);
};
//---------------------------------------------------------------------------

//A class for a triangle that is always negativly oriented
struct Triangle {
	Vertex A, B, C;
	Triangle(Vertex A, Vertex B, Vertex C): A(A), B(B), C(C) {
		if (orientation(A, B, C) > 0) {
			swap(B, C);
		}
	}
	Vertex get_centroid(){return Vertex((A.x + B.x + C.x)/3, (B.y + B.y + C.y)/3);}
	void draw(TImage*, TColor line_color=DEFUALT_TRIANGLE_COLOR);
};
double calculateArea(Vertex A, Vertex B, Vertex C);
bool compareByArea(Triangle t1, Triangle t2);
//---------------------------------------------------------------------------

//A class for a polygon containing a vector of vertices
class Simple_polygon{
	vector<Vertex> vertices;
public:
	Simple_polygon():vertices(){};
	Simple_polygon(vector<Vertex> v):vertices(v){};
	void order_self();//This positivley orients the polygon
	void draw(TImage*, bool show_vertices=true, TColor segment_color = DEFUALT_SEGMENT_COLOR, TColor vertex_color = DEFUALT_VERTEX_COLOR);
	void fill_with_color(TImage*, bool show_vertices=true, TColor fill_color = DEFUALT_POLYGON_FILL_COLOR, TColor vertex_color = DEFUALT_VERTEX_COLOR);
	double surface_area(); //calculates the surface area
	bool is_point_inside_poly(Vertex); //tests if a given point is inside this polygon
	friend class Tgui;
	friend class Camera;
};
//---------------------------------------------------------------------------

//A class for a camera
class Camera{
	Simple_polygon view;
	double fill_angle;
	int radius;
	int accuracy;
	bool put_in_vertex;//If this is true algorithms will put these cameras in vertices
	bool put_in_center;//If this is true algorithms will put these cameras in centorids
public:
	//constructor makes a camera based on origin, direction fill_angle and radius
	Camera():view(), fill_angle(0), radius(0), accuracy(0), put_in_vertex(true), put_in_center(false){};
	Camera(bool put_in_vertex, bool put_in_center, double fill_angle, int radius, int accuracy):
	fill_angle(fill_angle), radius(radius), accuracy(accuracy), put_in_vertex(put_in_vertex), put_in_center(put_in_center){};
	Camera(Vertex origin, Vertex direction, vector<Simple_polygon> polygons_to_hit, bool put_in_vertex=true,
	bool put_in_center=false, float fill_angle=1, int radius=150, int accuracy=100);
	void draw(TImage*, bool show_vertices=false, TColor vertex_color=DEFUALT_VERTEX_COLOR, TColor segment_color=DEFUALT_CAMERA_FILL_COLOR);
	void fill_color(TImage*, TColor fill_color = DEFUALT_CAMERA_FILL_COLOR);
	void hit(Simple_polygon&);//Intersects a camera with a polygon using raycasting
	friend class Tgui;

};

//---------------------------------------------------------------------------
#endif
