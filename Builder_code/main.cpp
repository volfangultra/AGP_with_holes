//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "CDT.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tagp_aplication *agp_aplication;
int start_x, start_y;
using namespace std;

//---------------------------------------------------------------------------

double covered_surface_area(TImage* image, Simple_polygon& poly, vector<Simple_polygon>& holes){
	int covered_area = 0;
	double blue_area = poly.surface_area();
	for(auto hole:holes)
		blue_area -= hole.surface_area();

	for (int y = 0; y < image->Height; y++) {
        for (int x = 0; x < image->Width; x++) {
			TColor pixelColor = image->Canvas->Pixels[x][y];
			if (pixelColor == clYellow) {
				covered_area += 1;
			}
			if(pixelColor == clAqua){
				blue_area += 1;
			}
		}
	}
	if(blue_area == 0)
		return 0;

	return covered_area / blue_area;
}

//---------------------------------------------------------------------------

__fastcall Tagp_aplication::Tagp_aplication(TComponent* Owner): TForm(Owner){
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));

	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	radio_draw_polygon->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::image_mouse_move(TObject *Sender, TShiftState Shift, int X, int Y){
	string koordinate = "X: " + to_string(X) + " Y: " + to_string(Y);
	text_coordinate->Text = koordinate.c_str();
}

//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::image_mouse_down(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){

	Vertex nova(X, Y);
	if(radio_draw_polygon->Checked) {
		outside_polygon.vertices.push_back(nova);
		int n = outside_polygon.vertices.size();

		if(n > 1) {
			Segment(outside_polygon.vertices[n - 2], nova).draw(image);
		}
		nova.draw(image);
	}
	else if(radio_draw_hole->Checked){
		int broj_rupa = holes.size();
		if(broj_rupa == 0){
			holes.push_back(Simple_polygon());
			broj_rupa++;
		}
		holes[broj_rupa - 1].vertices.push_back(nova);
		int n = holes[broj_rupa - 1].vertices.size();
		if(n > 1){
		   Segment(holes[broj_rupa - 1].vertices[n - 2], nova).draw(image);
		}
		nova.draw(image);
	}
	else if(radio_add_camera->Checked){
		start_x = X;
		start_y = Y;
		text_num_cameras->Text = to_string(cameras.size() + 1).c_str();
	}
}

//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::image_mouse_up(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){

	if(radio_add_camera->Checked){
		if(start_x == X && start_y == Y)
            return;
		Vertex center(start_x, start_y);
		int direction_x = X - start_x;
		int direction_y = Y - start_y;

		Simple_polygon view;
		Camera c(view);

		double alpha = c.RADIUS/sqrt(direction_x*direction_x + direction_y*direction_y);
		Vertex end(start_x + alpha * direction_x, start_y + alpha * direction_y);

		c.view.vertices.push_back(center);

		Vertex temp = end.rotate(center, c.FILL_ANGLE /2);
		for(int i = 0; i < c.ACCURACY; i++){
            c.view.vertices.push_back(temp);
			temp = temp.rotate(center, -1 * c.FILL_ANGLE/c.ACCURACY);

		}

		c.view.vertices.push_back(temp);

		//Presjec view sa svim ostalim kamerama i rupama
		for(auto rupe:holes)
			c.hit(rupe);
		c.hit(outside_polygon);
		c.fill_color(image);
		c.draw(image, clYellow, clYellow);
		cameras.push_back(c);
	}
}

//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::button_clean_click(TObject *Sender){
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
    image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;

	outside_polygon = Simple_polygon();
	holes = vector<Simple_polygon>();
	cameras.clear();

}
//---------------------------------------------------------------------------


void __fastcall Tagp_aplication::button_finish_object_click(TObject *Sender){

	if(radio_draw_polygon->Checked){
		int n(outside_polygon.vertices.size());
		if (n < 3)
			return;
		Segment(outside_polygon.vertices[n - 1], outside_polygon.vertices[0]).draw(image);
		outside_polygon.fill_color(image);
	}
	else if(radio_draw_hole->Checked){
		int broj_rupa(holes.size());
		if(broj_rupa < 1)
			return;

		int n(holes[broj_rupa - 1].vertices.size());
		if(n < 3)
			return;
		Segment(holes[broj_rupa - 1].vertices[n - 1], holes[broj_rupa - 1].vertices[0]).draw(image);
		holes[broj_rupa - 1].fill_color(image, clWhite);
		holes.push_back(Simple_polygon());

	}

}
//---------------------------------------------------------------------------


void __fastcall Tagp_aplication::ButtonPolygonAreaClick(TObject *Sender)
{

	double area_covered = covered_surface_area(image, outside_polygon, holes);
	UnicodeString rez = FormatFloat("#00.00", (area_covered * 100));

	text_surface_area_covered->Text = rez + "%";

}
//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::CamerasIntersectionClick(TObject *Sender)
{
       for(int i=0;i<cameras.size();i++){
		for(int j=i+1;j<cameras.size();j++){
			vector<Vertex> intersectionPolygon = cameras[i].view.get_intersection_of_polygons(cameras[j].view);
			if(intersectionPolygon.size()!=0){
				for (int k=0; k < intersectionPolygon.size()-1; k++) {
					int x1=intersectionPolygon[k].x;
					int x2=intersectionPolygon[k+1].x;
					int y1=intersectionPolygon[k].y;
					int y2=intersectionPolygon[k+1].y;
					Segment d = Segment(Vertex(x1,y1), Vertex(x2,y2));
					d.draw(image, clRed);
				}
				int x1=intersectionPolygon[0].x;
				int x2=intersectionPolygon[intersectionPolygon.size()-1].x;
				int y1=intersectionPolygon[0].y;
				int y2=intersectionPolygon[intersectionPolygon.size()-1].y;
				Segment d = Segment(Vertex(x1,y1), Vertex(x2,y2));
				d.draw(image, clRed);
				}
			}
	}
}
//---------------------------------------------------------------------------


void __fastcall Tagp_aplication::ButtonDrawCampusClick(TObject *Sender)
{
	ifstream inputFileCampus("C:/Users/eminm/OneDrive/Desktop/Projects/Adis Projekti/AGP_with_holes/Builder_code/campus.txt");
	if (inputFileCampus.is_open()) {
		string line;
		while (getline(inputFileCampus, line)) {
			double x, y;
			stringstream lineStream(line);
			lineStream>>x>>y;
			Vertex vertex(x,y);
			outside_polygon.vertices.push_back(vertex);
			vertex.draw(image);
		}
        int n(outside_polygon.vertices.size());
        Segment(outside_polygon.vertices[n - 1], outside_polygon.vertices[0]).draw(image);
		outside_polygon.fill_color(image);
		inputFileCampus.close();
	}

	ifstream inputFileHoles("C:/Users/eminm/OneDrive/Desktop/Projects/Adis Projekti/AGP_with_holes/Builder_code/holes.txt");
	if (inputFileHoles.is_open()) {
		string line;
		int broj_rupa(holes.size());
		while (getline(inputFileHoles, line)) {
			double x, y;
			if (line.empty()) {
				int n(holes[broj_rupa - 1].vertices.size());
				Segment(holes[broj_rupa - 1].vertices[n - 1], holes[broj_rupa - 1].vertices[0]).draw(image);
				holes[broj_rupa - 1].fill_color(image, clWhite);
				broj_rupa++;
				holes.push_back(Simple_polygon());
			} else {
				stringstream lineStream(line);
				lineStream>>x>>y;
				Vertex vertex(x,y);
				if(broj_rupa == 0){
					holes.push_back(Simple_polygon());
					broj_rupa++;
				}
				holes[broj_rupa - 1].vertices.push_back(vertex);
				vertex.draw(image);
			}


		}
		inputFileHoles.close();
	}
}
//---------------------------------------------------------------------------





void __fastcall Tagp_aplication::ButtonDelaunayTriangulationClick(TObject *Sender)

{
	CDT::Triangulation<double> cdt;
	vector<Vertex> all_vertices = outside_polygon.vertices;
	for(auto hole:holes)
		for(auto vertex:hole.vertices)
			all_vertices.push_back(vertex);

	cdt.insertVertices(
		all_vertices.begin(),
		all_vertices.end(),
		[](const Vertex& p){ return p.x; },
		[](const Vertex& p){ return p.y; }
	);

	vector<pair<int,int>> segments;
	int n = outside_polygon.vertices.size();
	int brojac = 0;
	for(int i = 0; i < n; i++){
		segments.push_back({i, (i+1)%n});
		brojac++;
	}

	for(int i = 0; i < holes.size() - 1; i++){
		for(int j = 0; j < holes[i].vertices.size() - 1; j++){
			segments.push_back({brojac, brojac + 1});
			brojac++;
		}
		segments.push_back({brojac - holes[i].vertices.size() + 1 , brojac});
		brojac++;
	}

	for(int i = 0; i < segments.size(); i++)
		Segment temp = Segment(all_vertices[segments[i].first],all_vertices[segments[i].second]);


    cdt.insertEdges(
		segments.begin(),
		segments.end(),
		[](const pair<int, int>& e){ return e.first; },
		[](const pair<int, int>& e){ return e.second; }
	);

	cdt.eraseOuterTrianglesAndHoles();

	unordered_set<CDT::Edge> triangle_segments = CDT::extractEdgesFromTriangles(cdt.triangles);

	vector<CDT::Triangle> triangles = cdt.triangles;
	for(auto triangle : triangles){
		auto A = triangle.vertices[0];
		auto B = triangle.vertices[1];
		auto C = triangle.vertices[2];
		delaunay_triangles.push_back(Triangle(all_vertices[A], all_vertices[B], all_vertices[C]));

	}

	for(auto triangle: delaunay_triangles){
		triangle.draw(image);
	}

}
//---------------------------------------------------------------------------

