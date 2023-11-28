//---------------------------------------------------------------------------

#pragma hdrstop

#include "poligon.h"
#include <algorithm>
#include <string>

//---------------------------------------------------------------------------

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
	return A.x == B.x && A.y == B.y;
}

//---------------------------------------------------------------------------

void Vertex::draw(TImage* slika, TColor color) {
	slika->Canvas->Brush->Color = color;
	slika->Canvas->Ellipse(x-SIZE, y-SIZE, x+SIZE, y+SIZE);
    slika->Canvas->Brush->Color = clWhite;
}

//---------------------------------------------------------------------------

Vertex& Vertex::operator=(const Vertex &kopija) {
	this->x = kopija.x;
	this->y = kopija.y;
    return *this;
}

//---------------------------------------------------------------------------

Vertex Vertex::rotate(Vertex center, float angle){
	Vertex result;

	result.x = (x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x;
	result.y = (x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y;

	return result;
}

double Vertex::distance_to(Vertex& point){
	return (x - point.x)*(x - point.x)+ (y - point.y)*(y - point.y);
}


void Segment::draw(TImage* slika, TColor boja) {
	slika->Canvas->Pen->Color = boja;
	slika->Canvas->MoveTo(A.x, A.y);
	slika->Canvas->LineTo(B.x, B.y);
	slika->Canvas->Pen->Color = clBlack;
}

//---------------------------------------------------------------------------

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

bool do_segments_intersect(Segment d1, Segment d2) {
	bool condition1 = orientation(d1.A, d1.B, d2.A) != orientation(d1.A, d1.B, d2.B);
	bool condition2 = orientation(d2.A, d2.B, d1.A) != orientation(d2.A, d2.B, d1.B);

	return condition1 && condition2;
}

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

bool is_a_Vertex_in_triangle(Vertex T, Trougao tr) {
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

double Simple_polygon::surface_area(){
	double sum1 = 0, sum2 = 0;
	if(vertices.size() == 0)
        return 0;
	for(int i=0; i< vertices.size()-1; i++) {
		sum1 += vertices[i].x * vertices[i+1].y;
		sum2 += vertices[i].y * vertices[i+1].x;
	}
	sum1 += vertices[vertices.size()-1].x * vertices[0].y;
	sum2 += vertices[0].x * vertices[vertices.size()-1].y;

    return abs(sum1 - sum2)/2;

}

//---------------------------------------------------------------------------

bool Simple_polygon::is_convex(){

    //Implementirati is_convex();

    return false;
}

void Camera::draw(TImage* slika, TColor vertex_color, TColor segment_color){
	slika->Canvas->Pen->Color = clYellow;
	slika->Canvas->MoveTo(view.vertices[0].x, view.vertices[0].y);
	for(int i=1; i<view.vertices.size(); i++) {
		slika->Canvas->LineTo(view.vertices[i].x, view.vertices[i].y);
	}
	slika->Canvas->LineTo(view.vertices[0].x, view.vertices[0].y);
	slika->Canvas->Pen->Color = clBlack;

}

//---------------------------------------------------------------------------

void Camera::fill_color(TImage* slika, TColor color){


}

//---------------------------------------------------------------------------

Vertex get_intersection_point(Vertex l1p1, Vertex l1p2, Vertex l2p1, Vertex l2p2) {
	double A1 = l1p2.y - l1p1.y;
	double B1 = l1p1.x - l1p2.x;
	double C1 = A1 * l1p1.x + B1 * l1p1.y;
	double A2 = l2p2.y - l2p1.y;
	double B2 = l2p1.x - l2p2.x;
	double C2 = A2 * l2p1.x + B2 * l2p1.y;

    // Lines are parallel
    double det = A1 * B2 - A2 * B1;
	if (is_equal(det, 0.0)) {
		return Vertex(0.0, 0.0);  // Parallel lines
	} else {
		double x = (B2 * C1 - B1 * C2) / det;
        double y = (A1 * C2 - A2 * C1) / det;
		bool online1 = ((min(l1p1.x, l1p2.x) < x || is_equal(min(l1p1.x, l1p2.x), x))
			&& (max(l1p1.x, l1p2.x) > x || is_equal(max(l1p1.x, l1p2.x), x))
			&& (min(l1p1.y, l1p2.y) < y || is_equal(min(l1p1.y, l1p2.y), y))
			&& (max(l1p1.y, l1p2.y) > y || is_equal(max(l1p1.y, l1p2.y), y))
		);
		bool online2 = ((min(l2p1.x, l2p2.x) < x || is_equal(min(l2p1.x, l2p2.x), x))
			&& (max(l2p1.x, l2p2.x) > x || is_equal(max(l2p1.x, l2p2.x), x))
			&& (min(l2p1.y, l2p2.y) < y || is_equal(min(l2p1.y, l2p2.y), y))
			&& (max(l2p1.y, l2p2.y) > y || is_equal(max(l2p1.y, l2p2.y), y))
		);
        if (online1 && online2)
			return Vertex(x, y);
	}
	return Vertex(0.0, 0.0);  // Intersection is out of at least one segment
}

//---------------------------------------------------------------------------

bool Simple_polygon::is_point_inside_poly(Vertex test)
{
	int i;
	int j;
	bool result = false;
	for (i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
	{
		if ((vertices[i].y > test.y) != (vertices[j].y > test.y) &&
			(test.x < (vertices[j].x - vertices[i].x) * (test.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
		{
			result = !result;
		}
	}
	return result;
}

//---------------------------------------------------------------------------

vector<Vertex> Simple_polygon::get_intersection_points(Vertex l1p1, Vertex l1p2)
{
	vector<Vertex> intersection_points;;
	for (int i = 0; i < vertices.size(); i++)
	{
		int next = (i + 1 == vertices.size()) ? 0 : i + 1;
		Vertex ip = get_intersection_point(l1p1, l1p2, vertices[i], vertices[next]);
		if (ip.x != 0.0 && ip.y != 0.0) intersection_points.push_back(ip);
	}
	return intersection_points;
}

//---------------------------------------------------------------------------

void add_points(vector<Vertex>& pool, vector<Vertex> new_points)
{
	for(int i=0;i<new_points.size();i++)
	{
		bool found = false;
		for(int j=0;j<pool.size();j++)
		{
			if (is_equal(pool[j].x, new_points[i].x) && is_equal(pool[j].y, new_points[i].y))
			{
				found = true;
				break;
			}
		}
		if (!found) pool.push_back(new_points[i]);
	}
}

//---------------------------------------------------------------------------

vector<Vertex> order_clockwise(vector<Vertex> points)
{
	double mX = 0;
	double mY = 0;

	for (const Vertex p : points) {
		mX += p.x;
		mY += p.y;
	}

	mX /= points.size();
	mY /= points.size();

	sort(points.begin(), points.end(), [&](const Vertex& a, const Vertex& b) {
		double angleA = std::atan2(a.y - mY, a.x - mX);
		double angleB = std::atan2(b.y - mY, b.x - mX);
		return angleA < angleB;
	});

	return points;
}

//---------------------------------------------------------------------------

vector<Vertex> Simple_polygon::get_intersection_of_polygons(Simple_polygon &poly2)
{
	vector<Vertex> clipped_corners;
	//Add  the corners of poly1 which are inside poly2
	for (int i = 0; i < vertices.size(); i++)
    {
		if (poly2.is_point_inside_poly(vertices[i]))
			add_points(clipped_corners, vector<Vertex> { vertices[i] });
	}
    //Add the corners of poly2 which are inside poly1
	for (int i = 0; i < poly2.vertices.size(); i++)
	{
		if (is_point_inside_poly(poly2.vertices[i]))
			add_points(clipped_corners, vector<Vertex>{ poly2.vertices[i]});
	}
	//Add  the intersection points
	for (int i = 0, next = 1; i < vertices.size(); i++, next = (i + 1 == vertices.size()) ? 0 : i + 1)
	{
		if(clipped_corners.size()!=0)
			add_points(clipped_corners, poly2.get_intersection_points(vertices[i], vertices[next]));
	}

	if(clipped_corners.size()==0) return vector<Vertex>();

	return order_clockwise(clipped_corners);
}

//---------------------------------------------------------------------------

void Camera::hit(Simple_polygon& poly){
	if(poly.vertices.size() == 0)
		return;

	for(int i = 1; i < view.vertices.size(); i++){
		Segment ray = Segment(view.vertices[0], view.vertices[i]);
		for(int j = 0; j < poly.vertices.size(); j++){
			Segment temp = Segment(poly.vertices[j], poly.vertices[(j + 1)%poly.vertices.size()]);
			if(do_segments_intersect(ray, temp)){
				Vertex intersection = line_intersection(ray, temp);
				if(view.vertices[0].distance_to(intersection) < view.vertices[0].distance_to(view.vertices[i]))
                    view.vertices[i] = intersection;

			};

		}

	}
}


bool isPointInsideTriangle(const Vertex p, const Vertex a, const Vertex b, const Vertex c) {
	double areaABC = 0.5 * ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
	double areaPBC = 0.5 * ((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y));
	double areaPCA = 0.5 * ((c.x - p.x) * (a.y - p.y) - (a.x - p.x) * (c.y - p.y));
	double areaPAB = 0.5 * ((a.x - p.x) * (b.y - p.y) - (b.x - p.x) * (a.y - p.y));
	return (areaABC == areaPBC + areaPCA + areaPAB);
}

/*vector<vector<Vertex>> Simple_polygon:: earClipping() {
	vector<vector<Vertex>> triangles;
	vector<int> ears;
	int n = vertices.size();

for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        int k = (i + 2) % n;
		if (isEar(vertices, i, j, k)) {
			ears.push_back(j);
        }
	}
	while (!ears.empty()) {
		int earIndex = ears.back();
        ears.pop_back();

        int i = (earIndex - 1 + n) % n;
        int j = earIndex;
        int k = (earIndex + 1) % n;

        // Add the triangle (i, j, k) to the list
		triangles.push_back({vertices[i], vertices[j], vertices[k]});

        // Remove vertex j from the polygon and update ears list
		vertices.erase(vertices.begin() + j);
        n--;

		// Update the ears list after removing vertex j
        int prevIndex = (i - 1 + n) % n;
        int nextIndex = (i + 1) % n;
		if (isEar(vertices, prevIndex, i, nextIndex)) {
            ears.push_back(i);
		}
		if (isEar(vertices, prevIndex, nextIndex, nextIndex + 1)) {
            ears.push_back(nextIndex);
        }
    }

    // Add the last triangle to the list
	triangles.push_back(vertices);
	return triangles;
}*/

bool isEar(int i, vector<Vertex> &vertices) {
    int n = vertices.size();

    Vertex prev = vertices[(i - 1 + n) % n];
    Vertex current = vertices[i];
    Vertex next = vertices[(i + 1) % n];

    for (int j = 0; j < n; ++j) {
        if (j != i && j != (i - 1 + n) % n && j != (i + 1) % n) {
            Vertex p = vertices[j];
            if (isPointInsideTriangle(prev, current, next, p)) {
                return false;
            }
		}
	}
    return true;
}

// Ear clipping triangulation algorithm
/*vector<vector<Vertex>> Simple_polygon::earClipping() {
	Simple_polygon remainingPolygon = *this;
	vector<vector<Vertex>> triangles;
	while (remainingPolygon.vertices.size() > 2) {

		size_t n = remainingPolygon.vertices.size();
		for (size_t i = 0; i < n; ++i) {
			const Vertex& a = remainingPolygon.vertices[i];
			const Vertex& b = remainingPolygon.vertices[(i + 1) % n];
			const Vertex& c = remainingPolygon.vertices[(i + 2) % n];
			if (is_a_Vertex_in_triangle(b, Trougao(a, c, remainingPolygon.vertices[(i + 3) % n]))) {
				bool isEar = true;
				for (size_t j = 0; j < n; ++j) {
					if (j != i && j != (i + 1) % n && j != (i + 2) % n) {
						const Vertex& testPoint = remainingPolygon.vertices[j];
						if (is_a_Vertex_in_triangle(testPoint, Trougao(a, b, c))) {
							isEar = false;
							break;
						}
					}
				}
				if (isEar) {
					triangles.push_back({a, b, c});
					remainingPolygon.vertices.erase(remainingPolygon.vertices.begin() + (i + 1) % n);
					break;
				}
            }
		}
    }
	triangles.push_back(remainingPolygon.vertices);
	return triangles;
}*/


int findEar(vector<Vertex> &vertices) {
		int n = vertices.size();

        for (int i = 0; i < n; ++i) {
			if (isEar(i, vertices)) {
                return i;
            }
        }

        // If no ear is found, return an arbitrary vertex
        return 1;
	}

bool isPointInsideTriangle(const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& p) {
        double detT = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
        double alpha = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / detT;
        double beta = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / detT;
        double gamma = 1.0 - alpha - beta;

        return alpha >= 0 && beta >= 0 && gamma >= 0;
	}


vector<vector<Vertex>> Simple_polygon :: earClipping() {
		vector<vector<Vertex>> triangles;

        while (vertices.size() > 3) {
			int earIndex = findEar(vertices);
            triangles.push_back({vertices[earIndex - 1], vertices[earIndex], vertices[earIndex + 1]});
			vertices.erase(vertices.begin() + earIndex);
        }

        // Last triangle
		triangles.push_back(vertices);
		return triangles;
}

//---------------------------------------------------------------------------




#pragma package(smart_init)
