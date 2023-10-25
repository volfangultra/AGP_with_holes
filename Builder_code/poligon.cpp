//---------------------------------------------------------------------------

#pragma hdrstop

#include "poligon.h"

//---------------------------------------------------------------------------

bool operator<(Dot A, Dot B) {
    if(A.x < B.x)
        return true;
    else if(A.x > B.x)
        return false;
    return A.y < B.y;
}

//---------------------------------------------------------------------------

bool operator==(Dot A, Dot B) {
	return A.x == B.x && A.y == B.y;
}

//---------------------------------------------------------------------------

void Dot::draw(TImage* slika, TColor color) {
	slika->Canvas->Brush->Color = color;
	slika->Canvas->Ellipse(x-SIZE, y-SIZE, x+SIZE, y+SIZE);
    slika->Canvas->Brush->Color = clWhite;
}

//---------------------------------------------------------------------------

Dot& Dot::operator=(const Dot &kopija) {
	this->x = kopija.x;
	this->y = kopija.y;
    return *this;
}

//---------------------------------------------------------------------------

Dot Dot::rotate(Dot center, float angle){
	Dot result;

	result.x = (x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x;
	result.y = (x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y;

	return result;
}


void Segment::draw(TImage* slika, TColor boja) {
	slika->Canvas->Pen->Color = boja;
	slika->Canvas->MoveTo(A.x, A.y);
	slika->Canvas->LineTo(B.x, B.y);
	slika->Canvas->Pen->Color = clBlack;
}

//---------------------------------------------------------------------------

int orientation(Dot A, Dot B, Dot C) {
	double area = A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y);
	if (area > 0)
        return -1;
	else if (area < 0)
        return 1;
    else
        return 0;
}

//---------------------------------------------------------------------------

bool do_segments_intersect(Segment d1, Segment d2) {
	bool condition1 = orientation(d1.A, d1.B, d2.A) != orientation(d1.A, d1.B, d2.B);
	bool condition2 = orientation(d2.A, d2.B, d1.A) != orientation(d2.A, d2.B, d1.B);

	return condition1 && condition2;
}

Dot line_intersection(Segment d1, Segment d2) {
	Dot A = d1.A, B=d1.B;
	Dot C = d2.A, D=d2.B;

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
		return Dot(x, y);
	}
}

//---------------------------------------------------------------------------

bool is_a_dot_in_triangle(Dot T, Trougao tr) {
	int o1 = orientation(tr.A, tr.B, T);
	int o2 = orientation(tr.B, tr.C, T);
	int o3 = orientation(tr.C, tr.A, T);

    return (o1 == o2) && (o2 == o3);
}

//---------------------------------------------------------------------------

void Simple_polygon::draw(TImage* slika, TColor color) {
	slika->Canvas->Pen->Color = color;
	slika->Canvas->MoveTo(vertices[0].x, vertices[0].y);
	for(int i=1; i<vertices.size(); i++) {
		slika->Canvas->LineTo(vertices[i].x, vertices[i].y);
	}
	slika->Canvas->LineTo(vertices[0].x, vertices[0].y);
	slika->Canvas->Pen->Color = clBlack;
}
//---------------------------------------------------------------------------

void Simple_polygon::fill_color(TImage* slika, TColor color){
	slika->Canvas->Brush->Color = color;
	TPoint polygon_points[vertices.size()];
	for(int i = 0; i < vertices.size(); i++)
		polygon_points[i] = Point(vertices[i].x, vertices[i].y);
	slika->Canvas->Polygon(polygon_points, vertices.size() - 1);

	for(int i = 0; i < vertices.size(); i++)
        vertices[i].draw(slika);

}
//---------------------------------------------------------------------------

#pragma package(smart_init)
