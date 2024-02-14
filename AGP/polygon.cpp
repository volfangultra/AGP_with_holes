//---------------------------------------------------------------------------

#pragma hdrstop
#include "polygon.h"
#include <algorithm>
#include <string>
//---------------------------------------------------------------------------
//VERTEX IMPLEMENTATION
//---------------------------------------------------------------------------
//Test wether two real numbers are equal
bool is_equal(double a, double b, double epsilon = 1e-9) {
	return std::abs(a - b) <= epsilon;
}

//---------------------------------------------------------------------------
bool operator<(Vertex A, Vertex B) {
	if(A.x < B.x)
		return true;
	else if(A.x > B.x)
		return false;
	return A.y < B.y;
}
//---------------------------------------------------------------------------
bool operator==(Vertex A, Vertex B) {
	return is_equal(A.x, B.x) && is_equal(A.y, B.y);
}
bool operator!=(Vertex A, Vertex B) {
	return !(A==B);
}
//---------------------------------------------------------------------------
void Vertex::draw(TImage* image, TColor color) {
	image->Canvas->Brush->Color = color;
	image->Canvas->Pen->Color = color;
	image->Canvas->Ellipse(x-SIZE, y-SIZE, x+SIZE, y+SIZE);
	image->Canvas->Brush->Color = clWhite;
    image->Canvas->Pen->Color = clBlack;
}
//---------------------------------------------------------------------------
Vertex& Vertex::operator=(const Vertex &copy) {
	this->x = copy.x;
	this->y = copy.y;
    return *this;
}
//---------------------------------------------------------------------------
Vertex Vertex::rotate(Vertex center, float angle){
	Vertex result;
	result.x = (x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x;
	result.y = (x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y;
	return result;
}
//---------------------------------------------------------------------------
double Vertex::distance_to(Vertex& point){
	return (x - point.x)*(x - point.x)+ (y - point.y)*(y - point.y);
}
int orientation(Vertex A, Vertex B, Vertex C) {
	double area = A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y);
	if (area > 0)
		return -1;
	else if (area < 0)
		return 1;
	else
		return 0;
}
//---------------------------------------------------------------------------
//SEGMENT IMPLEMENTATION
//---------------------------------------------------------------------------
void Segment::draw(TImage* image, TColor color) {
	image->Canvas->Pen->Color = color;
	image->Canvas->MoveTo(A.x, A.y);
	image->Canvas->LineTo(B.x, B.y);
	image->Canvas->Pen->Color = clBlack;
}
//---------------------------------------------------------------------------
bool do_segments_intersect(Segment d1, Segment d2) {
	bool condition1 = orientation(d1.A, d1.B, d2.A) != orientation(d1.A, d1.B, d2.B);
	bool condition2 = orientation(d2.A, d2.B, d1.A) != orientation(d2.A, d2.B, d1.B);
	return condition1 && condition2;
}
//---------------------------------------------------------------------------
Vertex line_intersection(Segment d1, Segment d2) {
	Vertex A = d1.A, B=d1.B;
	Vertex C = d2.A, D=d2.B;
	double a1 = B.y - A.y;
	double b1 = A.x - B.x;
	double c1 = a1*(A.x) + b1*(A.y);
	double a2 = D.y - C.y;
	double b2 = C.x - D.x;
	double c2 = a2*(C.x)+ b2*(C.y);
    double determinant = a1*b2 - a2*b1;
    if (determinant == 0){
        throw "Lines do not intersect";
	}
	else{
        double x = (b2*c1 - b1*c2)/determinant;
        double y = (a1*c2 - a2*c1)/determinant;
		return Vertex(x, y);
	}
}
//---------------------------------------------------------------------------
//TRIANGLE IMPLEMENTATION
//---------------------------------------------------------------------------
bool is_a_Vertex_in_triangle(Vertex T, Triangle tr) {
	int o1 = orientation(tr.A, tr.B, T);
	int o2 = orientation(tr.B, tr.C, T);
	int o3 = orientation(tr.C, tr.A, T);
	return (o1 == o2) && (o2 == o3);
}
//---------------------------------------------------------------------------
double calculateArea(Vertex A, Vertex B, Vertex C) {
	return 0.5 * abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)));
}
//---------------------------------------------------------------------------
bool compareByArea(Triangle t1, Triangle t2) {
	return calculateArea(t1.A, t1.B, t1.C) > calculateArea(t2.A, t2.B, t2.C);
}
//---------------------------------------------------------------------------
void Triangle::draw(TImage* image, TColor line_color){
	Segment(A,B).draw(image, line_color);
	Segment(A,C).draw(image, line_color);
	Segment(B,C).draw(image, line_color);
}
//---------------------------------------------------------------------------
//POLYGON IMPLEMENTATION
//---------------------------------------------------------------------------
void Simple_polygon::draw(TImage* image, bool show_vertices,  TColor segment_color, TColor vertex_color) {
	image->Canvas->Pen->Color = segment_color;
	image->Canvas->MoveTo(vertices[0].x, vertices[0].y); //We move to the first point
	for(int i=1; i<vertices.size(); i++) {
		image->Canvas->LineTo(vertices[i].x, vertices[i].y); //We iterate through all points to create Polygon
	}
	image->Canvas->LineTo(vertices[0].x, vertices[0].y); //We color said Polygon
	image->Canvas->Pen->Color = clBlack;
	if(show_vertices == true)
		for(int i = 0; i < vertices.size(); i++)
		vertices[i].draw(image, vertex_color);
}
//---------------------------------------------------------------------------
void Simple_polygon::fill_with_color(TImage* image, bool show_vertices, TColor color, TColor vertex_color){
	image->Canvas->Brush->Color = color;
    image->Canvas->Pen->Color = color;
	TPoint polygon_points[vertices.size()];
	for(int i = 0; i < vertices.size(); i++)
		polygon_points[i] = Point(vertices[i].x, vertices[i].y);
	image->Canvas->Polygon(polygon_points, vertices.size() - 1);
	if(show_vertices == true)
		for(int i = 0; i < vertices.size(); i++)
		vertices[i].draw(image, vertex_color);
}
//---------------------------------------------------------------------------
//Calculating surface area is based on shoelace formula, also known as
//Gauss's area formula for simple polygon
double Simple_polygon::surface_area(){
	double sum1 = 0, sum2 = 0;
	if(vertices.size() == 0)
		return 0;	for(int i=0; i< vertices.size()-1; i++) {
		sum1 += vertices[i].x * vertices[i+1].y;
		sum2 += vertices[i].y * vertices[i+1].x;
	}
	sum1 += vertices[vertices.size()-1].x * vertices[0].y;
	sum2 += vertices[0].x * vertices[vertices.size()-1].y;
    return abs(sum1 - sum2)/2;
}
//---------------------------------------------------------------------------
//Sorting polygon vertices by angle
void Simple_polygon::order_self()
{
	double mX = 0;
	double mY = 0;
	for (const Vertex p : vertices) {
		mX += p.x;
		mY += p.y;
	}
	mX /= vertices.size();
	mY /= vertices.size();
	sort(vertices.begin(), vertices.end(), [&](const Vertex& a, const Vertex& b) {
		double angleA = std::atan2(a.y - mY, a.x - mX);
		double angleB = std::atan2(b.y - mY, b.x - mX);
        //Order is ascending
		return angleA < angleB;
	});
}
//---------------------------------------------------------------------------
//CAMERA POLYGON IMPLEMENTATION
//---------------------------------------------------------------------------
//Konstructor takes a point where the camera is positioned, what direction it looks,
//coverage radius and what polygons it needs to intersect
Camera::Camera(Vertex origin, Vertex direction, vector<Simple_polygon> polygons_to_hit,
 bool put_in_vertex, bool put_in_center, float fill_angle, int radius,
int accuracy): radius(radius), accuracy(accuracy), fill_angle(fill_angle),
put_in_vertex(true), put_in_center(false), view(){

	double alpha = radius/sqrt(direction.x*direction.x + direction.y*direction.y);
	origin.x += 0.02*alpha * direction.x;
	origin.y += 0.02*alpha * direction.y;
	//This point is at a distance of radius from the origin in direction of given direction
	Vertex end(origin.x + alpha * direction.x, origin.y + alpha * direction.y);
	view.vertices.push_back(origin);

	Vertex temp = end.rotate(origin, fill_angle /2); //We rotate accuracy number of points around the origin
	for(int i = 0; i < accuracy; i++){
		view.vertices.push_back(temp);
		temp = temp.rotate(origin, -1 * fill_angle/accuracy);
	}
	view.vertices.push_back(temp);

	for(auto polygon:polygons_to_hit) //We intersect all the polygons with the camera
		hit(polygon);
}
void Camera::draw(TImage* image, bool show_vertices, TColor vertex_color, TColor segment_color){
	image->Canvas->Pen->Color = segment_color;
	image->Canvas->MoveTo(view.vertices[0].x, view.vertices[0].y); //We move to the origin of the camera
	for(int i=1; i<view.vertices.size(); i++) {
		image->Canvas->LineTo(view.vertices[i].x, view.vertices[i].y);//We iterate through all points and draw a line
	}
	image->Canvas->LineTo(view.vertices[0].x, view.vertices[0].y);
	image->Canvas->Pen->Color = clBlack;
    if(show_vertices == true)
		for(int i = 0; i < view.vertices.size(); i++)  //We draw vertices
			view.vertices[i].draw(image);
}
//---------------------------------------------------------------------------
void Camera::fill_color(TImage* image, TColor color){
	view.fill_with_color(image, false, color, color);

}
//---------------------------------------------------------------------------
void Camera::hit(Simple_polygon& poly){
	if(poly.vertices.size() == 0)
		return;
	for(int i = 1; i < view.vertices.size(); i++){
		Segment ray = Segment(view.vertices[0], view.vertices[i]); //ray used for ray casting
		for(int j = 0; j < poly.vertices.size(); j++){
			Segment temp = Segment(poly.vertices[j], poly.vertices[(j + 1)%poly.vertices.size()]);
			if(do_segments_intersect(ray, temp)){//if a ray intersects a segment
				Vertex intersection = line_intersection(ray, temp);
				if(view.vertices[0].distance_to(intersection) < view.vertices[0].distance_to(view.vertices[i]))
					//if that intersection point is closer to the origin of a camera
					//than to the original vertex we exchange that vertex with the intersection
					view.vertices[i] = intersection;
			};
		}
	}
}


#pragma package(smart_init)
