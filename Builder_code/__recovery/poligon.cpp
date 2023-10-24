//---------------------------------------------------------------------------

#pragma hdrstop

#include "poligon.h"
#include <algorithm>
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

////bool isInside(Dot point, vector<Dot>& polygon) {
////	int n = polygon.size();
////	bool isInsidePolygon = false;
////
////	for (int i = 0; i < n; i++) {
////		Dot p1 = polygon[i];
////		Dot p2 = polygon[(i + 1) % n];
////
////        if ((p1.y <= point.y && p2.y > point.y) || (p1.y > point.y && p2.y <= point.y)) {
////            double vt = (point.y - p1.y) / (p2.y - p1.y);
////			if (point.x < p1.x + vt * (p2.x - p1.x)) {
////                isInsidePolygon = !isInsidePolygon;
////            }
////		}
////	}
////
////	return isInsidePolygon;
////}//
//
////Simple_polygon Simple_polygon::intersect(Simple_polygon& clipPolygon) {
////	vector<Dot> output;
////	vector<Dot> clip = clipPolygon.vertices;
////
////	for (int i = 0; i < clip.size(); i++) {
////		int clipSize = clip.size();
////		vector<Dot> input = output;
////		output.clear();
////
////		Dot S = clip[i];
////		Dot E = clip[(i + 1) % clipSize];
////
////		for (int j = 0; j < input.size(); j++) {
////			Dot current = input[j];
////			Dot previous = input[(j + input.size() - 1) % input.size()];
////
////			if (isInside(current, clip)) {
////				if (!isInside(previous, clip)) {
////					// Add intersection point
////					int dx = current.x - previous.x;
////					int dy = current.y - previous.y;
////					int t = ((S.x - previous.x) * dy - (S.y - previous.y) * dx) / (dx * (E.y - S.y) - dy * (E.x - S.x));
////					Dot intersection = { previous.x + t * dx, previous.y + t * dy };
////					output.push_back(intersection);
////				}
////				output.push_back(current);
////			} else if (isInside(previous, clip)) {
////				// Add intersection point
////				int dx = current.x - previous.x;
////				int dy = current.y - previous.y;
////				int t = ((S.x - previous.x) * dy - (S.y - previous.y) * dx) / (dx * (E.y - S.y) - dy * (E.x - S.x));
////				Dot intersection = { previous.x + t * dx, previous.y + t * dy };
////				output.push_back(intersection);
////			}
////		}
////	}
////}//
//
//
//bool inside(Dot p, Dot edge1, Dot edge2) {
//	return (edge2.x - edge1.x) * (p.y - edge1.y) > (edge2.y - edge1.y) * (p.x - edge1.x);
//}
//
//Dot intersection(Dot p1, Dot p2, Dot edge1, Dot edge2) {
//	double edge_x = edge2.x - edge1.x;
//	double edge_y = edge2.y - edge1.y;
//	double p1_x = p1.x;
//	double p1_y = p1.y;
//	double p2_x = p2.x;
//	double p2_y = p2.y;
//
//	double denominator = edge_x * (p1_y - p2_y) - edge_y * (p1_x - p2_x);
//
//	if (denominator == 0) {
//		return {0, 0}; // Parallel or coincident
//	}
//
//	double t = ((edge1.y - p1_y) * (p1_x - p2_x) - (edge1.x - p1_x) * (p1_y - p2_y)) / denominator;
//
//	if (t >= 0 && t <= 1) {
//		double intersection_x = p1_x + t * (p2_x - p1_x);
//		double intersection_y = p1_y + t * (p2_y - p1_y);
//		return {intersection_x, intersection_y};
//	} else {
//		return {0, 0}; // No intersection
//	}
//}
//
//Simple_polygon Simple_polygon::intersect(Simple_polygon& clip) {
//	vector<Dot> outputList = vertices;
//	vector<Dot> clipPolygon = clip.vertices;
//
//	for (int i = 0; i < clipPolygon.size(); i++) {
//		vector<Dot> inputList = outputList;
//		outputList.clear();
//		Dot s;
//		if(inputList.size()>0)
//			s = inputList[inputList.size()-1];
//
//
//		for (const Dot& e : inputList) {
//			if (inside(e, clipPolygon[i], clipPolygon[(i + 1) % clipPolygon.size()])) {
//				if (!inside(s, clipPolygon[i], clipPolygon[(i + 1) % clipPolygon.size()])) {
//					Dot intersectionPoint = intersection(s, e, clipPolygon[i], clipPolygon[(i + 1) % clipPolygon.size()]);
//					if (intersectionPoint.x != 0 || intersectionPoint.y != 0) {
//						outputList.push_back(intersectionPoint);
//					}
//				}
//				outputList.push_back(e);
//			} else if (inside(s, clipPolygon[i], clipPolygon[(i + 1) % clipPolygon.size()])) {
//				Dot intersectionPoint = intersection(s, e, clipPolygon[i], clipPolygon[(i + 1) % clipPolygon.size()]);
//				if (intersectionPoint.x != 0 || intersectionPoint.y != 0) {
//					outputList.push_back(intersectionPoint);
//				}
//			}
//			s = e;
//		}
//	}
//	ShowMessage(outputList.size());
//	Simple_polygon sp;
//	sp.vertices = outputList;
//	return sp;
//}bool IsEqual(double a, double b, double epsilon = 1e-9) {    return std::abs(a - b) <= epsilon;
}

Dot GetIntersectionPoint(Dot l1p1, Dot l1p2, Dot l2p1, Dot l2p2) {
	double A1 = l1p2.y - l1p1.y;
	double B1 = l1p1.x - l1p2.x;
	double C1 = A1 * l1p1.x + B1 * l1p1.y;
	double A2 = l2p2.y - l2p1.y;
	double B2 = l2p1.x - l2p2.x;
	double C2 = A2 * l2p1.x + B2 * l2p1.y;

    // Lines are parallel
    double det = A1 * B2 - A2 * B1;
    if (IsEqual(det, 0.0)) {
		return Dot(0.0, 0.0);  // Parallel lines
    } else {
        double x = (B2 * C1 - B1 * C2) / det;
        double y = (A1 * C2 - A2 * C1) / det;
		bool online1 = ((min(l1p1.x, l1p2.x) < x || IsEqual(min(l1p1.x, l1p2.x), x))
			&& (max(l1p1.x, l1p2.x) > x || IsEqual(max(l1p1.x, l1p2.x), x))
			&& (min(l1p1.y, l1p2.y) < y || IsEqual(min(l1p1.y, l1p2.y), y))
			&& (max(l1p1.y, l1p2.y) > y || IsEqual(max(l1p1.y, l1p2.y), y))
		);
		bool online2 = ((min(l2p1.x, l2p2.x) < x || IsEqual(min(l2p1.x, l2p2.x), x))
			&& (max(l2p1.x, l2p2.x) > x || IsEqual(max(l2p1.x, l2p2.x), x))
			&& (min(l2p1.y, l2p2.y) < y || IsEqual(min(l2p1.y, l2p2.y), y))
			&& (max(l2p1.y, l2p2.y) > y || IsEqual(max(l2p1.y, l2p2.y), y))
		);
        if (online1 && online2)
			return Dot(x, y);
	}
	return Dot(0.0, 0.0);  // Intersection is out of at least one segment
}bool Simple_polygon::IsPointInsidePoly(Dot test){
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
}vector<Dot> Simple_polygon::GetIntersectionPoints(Dot l1p1, Dot l1p2){
	vector<Dot> intersectionPoints;;
	for (int i = 0; i < vertices.size(); i++)
	{
		int next = (i + 1 == vertices.size()) ? 0 : i + 1;
		Dot ip = GetIntersectionPoint(l1p1, l1p2, vertices[i], vertices[next]);
		if (ip.x != 0.0 && ip.y != 0.0) intersectionPoints.push_back(ip);
	}
	return intersectionPoints;
}void AddPoints(vector<Dot>& pool, vector<Dot> newpoints){
	for(int i=0;i<newpoints.size();i++)
	{
		bool found = false;
		for(int j=0;j<pool.size();j++)
		{
			if (IsEqual(pool[j].x, newpoints[i].x) && IsEqual(pool[j].y, newpoints[i].y))
			{
				found = true;
				break;
			}
		}
		if (!found) pool.push_back(newpoints[i]);
    }
}vector<Dot> OrderClockwise(vector<Dot> points){
    double mX = 0;
    double mY = 0;

	for (const Dot p : points) {
		mX += p.x;
		mY += p.y;
    }

	mX /= points.size();
    mY /= points.size();

	sort(points.begin(), points.end(), [&](const Dot& a, const Dot& b) {
		double angleA = std::atan2(a.y - mY, a.x - mX);
		double angleB = std::atan2(b.y - mY, b.x - mX);
        return angleA < angleB;
    });

	return points;
}vector<Dot> Simple_polygon::GetIntersectionOfPolygons(Simple_polygon &poly2){
	vector<Dot> clippedCorners;
    //Add  the corners of poly1 which are inside poly2
	for (int i = 0; i < vertices.size(); i++)
    {
		if (poly2.IsPointInsidePoly(vertices[i]))
			AddPoints(clippedCorners, vector<Dot> { vertices[i] });
	}
    //Add the corners of poly2 which are inside poly1
	for (int i = 0; i < poly2.vertices.size(); i++)
    {
		if (IsPointInsidePoly(poly2.vertices[i]))
			AddPoints(clippedCorners, vector<Dot>{ poly2.vertices[i]});
    }
	//Add  the intersection points
	for (int i = 0, next = 1; i < vertices.size(); i++, next = (i + 1 == vertices.size()) ? 0 : i + 1)
	{
		if(clippedCorners.size()!=0)
			AddPoints(clippedCorners, poly2.GetIntersectionPoints(vertices[i], vertices[next]));
	}

	if(clippedCorners.size()==0) return vector<Dot>();

	return OrderClockwise(clippedCorners);
}


#pragma package(smart_init)
