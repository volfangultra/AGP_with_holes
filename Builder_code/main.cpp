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
#include <unordered_map>
#include <set>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tagp_aplication *agp_aplication;
int start_x, start_y;
using namespace std;
//---------------------------------------------------------------------------
double covered_surface_area(TImage* image, Simple_polygon& poly, vector<Simple_polygon>& holes) {
	int covered_area = 0;
	double blue_area = poly.surface_area();
	for(auto hole:holes)
		blue_area -= hole.surface_area();

	Graphics::TBitmap* bitmap = new Graphics::TBitmap;
	bitmap->Assign(image->Picture->Bitmap);

	int imageWidth = bitmap->Width;
	int imageHeight = bitmap->Height;
	//#pragma omp parallel for reduction(+:covered_area) AKO STIGNETE PROBAJTE NEKAKO UPALIT PARALELIZIRANJE
	for (int y = 0; y < imageHeight; y++) {
		TRGBQuad* row = reinterpret_cast<TRGBQuad*>(bitmap->ScanLine[y]);
		for (int x = 0; x < imageWidth; x++) {
			if (row[x].rgbReserved == 0 && row[x].rgbBlue == 0 && row[x].rgbGreen == 255 && row[x].rgbRed == 255) {
				covered_area += 1;
			}
		}
	}

	delete bitmap;

	if (blue_area == 0) {
		return 0;
	}

	return static_cast<double>(covered_area) / static_cast<double>(blue_area);
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
		Camera c(view);  //OVdje mozes c(view, 6.28, 150) ako zelis krug
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
		text_num_cameras->Text = to_string(cameras.size() + 1).c_str();
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
	delaunay_triangles = vector<vector<Vertex>>();
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
	ifstream inputFileCampus("C:/Users/nedim/Desktop/AGP_2algor/AGP_with_holes/Builder_code/campus.txt");
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
	ifstream inputFileHoles("C:/Users/nedim/Desktop/AGP_2algor/AGP_with_holes/Builder_code/holes.txt");
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
		delaunay_triangles.push_back({all_vertices[A], all_vertices[B], all_vertices[C]});
        Triangle(all_vertices[A], all_vertices[B], all_vertices[C]).draw(image);
	}
}
//---------------------------------------------------------------------------
void __fastcall Tagp_aplication::coverArtGalleryClick(TObject *Sender)
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
		delaunay_triangles.push_back({all_vertices[A], all_vertices[B], all_vertices[C]});
	}
	for (auto trougao : delaunay_triangles){
		Vertex center;
		Vertex center2;
		int direction_x2;
		int direction_y2;
		int direction_x;
		int direction_y;
		double povrsina = 0;
		povrsina = Simple_polygon({trougao[0], trougao[1], trougao[2]}).surface_area();
		if(povrsina > 1500){
			center.x = trougao[0].x;
			center.y = trougao[0].y;
			direction_x = (trougao[1].x + trougao[2].x)/2;
			direction_y = (trougao[1].y + trougao[2].y)/2;
		}
		if(povrsina >= 6000){
			center2.x = trougao[2].x;
			center2.y = trougao[2].y;
			direction_x2 = (trougao[1].x + trougao[0].x)/2;
			direction_y2 = (trougao[1].y + trougao[0].y)/2;
		}

		if(povrsina > 1500){
			Simple_polygon view;
			Camera c(view);
			direction_x -= center.x;
			direction_y -= center.y;
			double alpha = c.RADIUS/sqrt(direction_x*direction_x + direction_y*direction_y);
			Vertex end(center.x + alpha * direction_x, center.y + alpha * direction_y);
			center.x += 0.02*alpha * direction_x;
			center.y += 0.02*alpha * direction_y;
			c.view.vertices.push_back(center);

			Vertex temp = end.rotate(center, c.FILL_ANGLE /2);
			for(int i = 0; i < c.ACCURACY; i++){
				c.view.vertices.push_back(temp);
				temp = temp.rotate(center, -1 * c.FILL_ANGLE/c.ACCURACY);
			}
			c.view.vertices.push_back(temp);

			for(auto rupa:holes)
				c.hit(rupa);
			c.hit(outside_polygon);
			c.fill_color(image);
			c.draw(image, clYellow, clYellow);
			cameras.push_back(c);
		}
		if(povrsina >= 6000){
			Simple_polygon view;
			Camera c(view);
			direction_x2 -= center2.x;
			direction_y2 -= center2.y;
			double alpha = c.RADIUS/sqrt(direction_x2*direction_x2 + direction_y2*direction_y2);
			Vertex end(center2.x + alpha * direction_x2, center2.y + alpha * direction_y2);
			center2.x += 0.02*alpha * direction_x2;
			center2.y += 0.02*alpha * direction_y2;
			c.view.vertices.push_back(center2);

			Vertex temp = end.rotate(center2, c.FILL_ANGLE /2);
			for(int i = 0; i < c.ACCURACY; i++){
				c.view.vertices.push_back(temp);
				temp = temp.rotate(center2, -1 * c.FILL_ANGLE/c.ACCURACY);
			}
			c.view.vertices.push_back(temp);

			for(auto rupa:holes)
				c.hit(rupa);
			c.hit(outside_polygon);
			c.fill_color(image);
			c.draw(image, clYellow, clYellow);
			cameras.push_back(c);
		}
		text_num_cameras->Text = to_string(cameras.size()).c_str();
	}

}
//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::button_color_verticesClick(TObject *Sender)
{
	unordered_map<Vertex, int> vertexColors;

	for (auto trougao : delaunay_triangles) {
		// Color the first vertex of the triangle with color 0
		//if (vertexColors.find(trougao[0]) == vertexColors.end()) {
		//	vertexColors[trougao[0]] = 1;
		//}
		// Check the other two vertices of the triangle
		for (int i = 0; i < 3; ++i) {
			Vertex vertex = trougao[i];
			// If the vertex is not colored, color it with a color different from its neighbors
			if (vertexColors.find(vertex) == vertexColors.end()) {
				set<int> neighborColors;
				for (Vertex neighbor : trougao) {
					if (neighbor != vertex  && vertexColors.find(neighbor) != vertexColors.end()) {
						neighborColors.insert(vertexColors[neighbor]);
					}
				}
				// Assign the first available color not used by neighbors
				for (int color = 0; color < 3; ++color) {
					if (neighborColors.find(color) == neighborColors.end()) {
					vertexColors[vertex] = color;
						break;
					}
				}
			}
		}
	}
	int crveni=0,zuti=0,zeleni=0;
	for (auto trougao : delaunay_triangles){
		for (int i = 0; i < 3; ++i){
			if(vertexColors[trougao[i]] == 0){
			   trougao[i].draw(image,clRed);
			   crveni++;
			}
			else if(vertexColors[trougao[i]] == 1){
			   trougao[i].draw(image,clYellow);
			   zuti++;
			}
			else{
			   trougao[i].draw(image,clGreen);
			   zeleni++;
			 }
		}
	}
	string text = "crveni=" + to_string(crveni) + "\nzuti=" + to_string(zuti) + "\nzeleni=" + to_string(zeleni);
	ShowMessage(text.c_str());

}
//---------------------------------------------------------------------------

double calculateArea(Vertex A, Vertex B, Vertex C) {
	return 0.5 * abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)));
}

bool compareByArea(vector<Vertex> t1, vector<Vertex> t2) {
	return calculateArea(t1[0], t1[1], t1[2]) > calculateArea(t2[0], t2[1], t2[2]);
}

void Camera::iscrtajKameru(int direction_x, int direction_y, Vertex center, TColor color, TImage* image, Simple_polygon& outside_polygon, vector<Simple_polygon>& holes) {
	direction_x -= center.x;
	direction_y -= center.y;
	double alpha = RADIUS/sqrt(direction_x*direction_x + direction_y*direction_y);
	Vertex end(center.x + alpha * direction_x, center.y + alpha * direction_y);
	center.x += 0.02*alpha * direction_x;
	center.y += 0.02*alpha * direction_y;
	view.vertices.push_back(center);

	Vertex temp = end.rotate(center, FILL_ANGLE /2);
	for(int i = 0; i < ACCURACY; i++){
		view.vertices.push_back(temp);
		temp = temp.rotate(center, -1 * FILL_ANGLE/ACCURACY);
	}
	view.vertices.push_back(temp);

	for(auto rupa:holes) {
		hit(rupa);
	}

	hit(outside_polygon);
	fill_color(image, color);
	draw(image, color, color);

}

double Tagp_aplication::pokrij(){
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	outside_polygon = Simple_polygon();
	holes = vector<Simple_polygon>();
	cameras.pop_back();
	ButtonDrawCampusClick(this);
	image->Canvas->Brush->Color = clYellow;
	for(auto c : cameras) {
	   c.fill_color(image, clYellow);
	   c.draw(image, clYellow, clYellow);
	}
	image->Canvas->Brush->Color = clWhite;
	return covered_surface_area(image, outside_polygon, holes);

}

void __fastcall Tagp_aplication::generateClick(TObject *Sender)
{
	int num_cameras = inputNumCameras->Text.ToInt();
	sort(delaunay_triangles.begin(), delaunay_triangles.end(), compareByArea);
	int counter = 0;
	double generalna_pokrivenost = 0;
	double epsilon = 0;
	for (auto trougao : delaunay_triangles){

		if (counter >= num_cameras)
			break;

		Vertex center;
		int direction_x;
		int direction_y;

		double pokrivenost = 0;
		int najbolja_pozicija = 0;
		for (int i = 0; i < 3; i++) {
			center.x = trougao[i].x;
			center.y = trougao[i].y;
			direction_x = (trougao[(i+1)%3].x + trougao[(i+2)%3].x)/2;
			direction_y = (trougao[(i+1)%3].y + trougao[(i+2)%3].y)/2;
			Simple_polygon view, view2;
			Camera c(view), c2(view2);
			c.iscrtajKameru(direction_x, direction_y, center, clYellow, image, outside_polygon, holes);
			cameras.push_back(c);
			double trenutna_pokrivenost = covered_surface_area(image, outside_polygon, holes);
			if (pokrivenost < trenutna_pokrivenost) {
				pokrivenost = trenutna_pokrivenost;
				najbolja_pozicija = i;
			}
			double odnos = pokrij();
		}
		ShowMessage(pokrivenost);
		ShowMessage(generalna_pokrivenost);

		if(pokrivenost - generalna_pokrivenost > 0.103-epsilon) {
			//ShowMessage(cameras.size());
			//epsilon+=0.004;

		//ShowMessage("usao u if");

		Simple_polygon view;
		Camera c(view);
		center.x = trougao[najbolja_pozicija].x;
		center.y = trougao[najbolja_pozicija].y;
		direction_x = (trougao[(najbolja_pozicija+1)%3].x + trougao[(najbolja_pozicija+2)%3].x)/2;
		direction_y = (trougao[(najbolja_pozicija+1)%3].y + trougao[(najbolja_pozicija+2)%3].y)/2;
		c.iscrtajKameru(direction_x, direction_y, center, clYellow, image, outside_polygon, holes);
		//c.fill_color(image, clYellow);
		cameras.push_back(c);
		counter++;
		generalna_pokrivenost = pokrivenost;
		}
		 epsilon+=0.004;


		text_num_cameras->Text = to_string(cameras.size()).c_str();
	}
}
//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::generate2Click(TObject *Sender){
		int num_cameras = inputNumCameras->Text.ToInt();
		sort(delaunay_triangles.begin(), delaunay_triangles.end(), compareByArea);
		int counter = 0;

		for (auto trougao : delaunay_triangles){
			Vertex center;
			Vertex center2;
			Vertex center3;
			Vertex center4;
			int direction_x2;
			int direction_y2;
			int direction_x;
			int direction_y;
			int direction_x3;
			int direction_y3;
			int direction_x4;
			int direction_y4;
			double povrsina = 0;
			povrsina = Simple_polygon({trougao[0], trougao[1], trougao[2]}).surface_area();
			if(povrsina >= 1000 && povrsina < 6000){
				center3.x = (trougao[0].x + trougao[1].x + trougao[2].x)/3;
				center3.y = (trougao[0].y + trougao[1].y + trougao[2].y)/3;
				direction_x3 = (trougao[1].x + trougao[0].x)/2;
				direction_y3 = (trougao[1].y + trougao[0].y)/2;
			}

			if(povrsina >= 8000){
				center.x = trougao[2].x;
				center.y = trougao[2].y;
				direction_x = (trougao[1].x + trougao[0].x)/2;
				direction_y = (trougao[1].y + trougao[0].y)/2;
			}

			if(povrsina >= 12000){
				center2.x = trougao[1].x;
				center2.y = trougao[1].y;
				direction_x2 = (trougao[2].x + trougao[0].x)/2;
				direction_y2 = (trougao[2].y + trougao[0].y)/2;
			}
			if(povrsina >= 16000){
				center4.x = trougao[0].x;
				center4.y = trougao[0].y;
				direction_x4 = (trougao[2].x + trougao[1].x)/2;
				direction_y4 = (trougao[2].y + trougao[1].y)/2;
			}

			if(povrsina >= 1000 && povrsina <= 6000){
				Simple_polygon view;
				Camera circled_c(view, 2*3.14159265359, 64);
				direction_x3 -= center3.x;
				direction_y3 -= center3.y;
				double alpha = circled_c.RADIUS/sqrt(direction_x3*direction_x3 + direction_y3*direction_y3);
				Vertex end2(center3.x + alpha * direction_x3, center3.y + alpha * direction_y3);
				center3.x += 0.02*alpha * direction_x2;
				center3.y += 0.02*alpha * direction_y2;
				circled_c.view.vertices.push_back(center3);

				Vertex temp2 = end2.rotate(center3, circled_c.FILL_ANGLE /2);
				for(int i = 0; i < circled_c.ACCURACY; i++){
					circled_c.view.vertices.push_back(temp2);
					temp2 = temp2.rotate(center3, -1 * circled_c.FILL_ANGLE/circled_c.ACCURACY);
				}
				circled_c.view.vertices.push_back(temp2);

				for(auto rupa:holes) {
					circled_c.hit(rupa);
				}

				circled_c.hit(outside_polygon);
				circled_c.fill_color(image, clYellow);
				circled_c.draw(image, clYellow, clYellow);
				cameras.push_back(circled_c);

				counter++;
				if (counter == num_cameras)
					break;
			}
			if(povrsina >= 8000){
				Simple_polygon view;
				Camera c(view);

				direction_x -= center.x;
				direction_y -= center.y;
				double alpha = c.RADIUS/sqrt(direction_x*direction_x + direction_y*direction_y);
				Vertex end(center.x + alpha * direction_x, center.y + alpha * direction_y);
				center.x += 0.02*alpha * direction_x;
				center.y += 0.02*alpha * direction_y;
				c.view.vertices.push_back(center);

				Vertex temp = end.rotate(center, c.FILL_ANGLE /2);
				for(int i = 0; i < c.ACCURACY; i++){
					c.view.vertices.push_back(temp);
					temp = temp.rotate(center, -1 * c.FILL_ANGLE/c.ACCURACY);
				}
				c.view.vertices.push_back(temp);

				for(auto rupa:holes) {
					c.hit(rupa);
				}

				c.hit(outside_polygon);
				c.fill_color(image);
				c.draw(image, clYellow, clYellow);

				cameras.push_back(c);
				counter++;
				if (counter == num_cameras)
					break;
			}
			if(povrsina >= 12000){
				Simple_polygon view;
				Camera c(view);

				direction_x2 -= center2.x;
				direction_y2 -= center2.y;
				double alpha = c.RADIUS/sqrt(direction_x2*direction_x2 + direction_y2*direction_y2);
				Vertex end(center2.x + alpha * direction_x2, center2.y + alpha * direction_y2);
				center2.x += 0.02*alpha * direction_x2;
				center2.y += 0.02*alpha * direction_y2;
				c.view.vertices.push_back(center2);

				Vertex temp = end.rotate(center2, c.FILL_ANGLE /2);
				for(int i = 0; i < c.ACCURACY; i++){
					c.view.vertices.push_back(temp);
					temp = temp.rotate(center2, -1 * c.FILL_ANGLE/c.ACCURACY);
				}
				c.view.vertices.push_back(temp);

				for(auto rupa:holes) {
					c.hit(rupa);
				}

				c.hit(outside_polygon);
				c.fill_color(image);
				c.draw(image, clYellow, clYellow);

				cameras.push_back(c);
				counter++;
				if (counter == num_cameras)
					break;
			}
			if(povrsina >= 16000){
				Simple_polygon view;
				Camera c(view);

				direction_x4 -= center4.x;
				direction_y4 -= center4.y;
				double alpha = c.RADIUS/sqrt(direction_x4*direction_x4 + direction_y4*direction_y4);
				Vertex end(center4.x + alpha * direction_x4, center4.y + alpha * direction_y4);
				center4.x += 0.02*alpha * direction_x4;
				center4.y += 0.02*alpha * direction_y4;
				c.view.vertices.push_back(center4);

				Vertex temp = end.rotate(center4, c.FILL_ANGLE /2);
				for(int i = 0; i < c.ACCURACY; i++){
					c.view.vertices.push_back(temp);
					temp = temp.rotate(center4, -1 * c.FILL_ANGLE/c.ACCURACY);
				}
				c.view.vertices.push_back(temp);

				for(auto rupa:holes) {
					c.hit(rupa);
				}

				c.hit(outside_polygon);
				c.fill_color(image);
				c.draw(image, clYellow, clYellow);

				cameras.push_back(c);
				counter++;
				if (counter == num_cameras)
					break;

			}
		}
		text_num_cameras->Text = to_string(cameras.size()).c_str();
}
//---------------------------------------------------------------------------

