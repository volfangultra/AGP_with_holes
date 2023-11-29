//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <filesystem>
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
    segmenti_rupa.clear();

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
	ifstream inputFileCampus("C:/Users/nedim/Desktop/AGP_campus/AGP_with_holes/Builder_code/campus2.txt");
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

	/*ifstream inputFileHoles("C:/Users/nedim/Desktop/AGP_campus/AGP_with_holes/Builder_code/holes.txt");
	if (inputFileHoles.is_open()) {
		vector<Vertex> every_vertex;
		string line;
		int broj_rupa(holes.size());
		while (getline(inputFileHoles, line)) {
			double x, y;
			if (line.empty()) {
				for(int i=0;i<every_vertex.size()-1;i++){
					segmenti_rupa.push_back(Segment(every_vertex[i],every_vertex[i+1]));
				}
				int n(holes[broj_rupa - 1].vertices.size());
				Segment(holes[broj_rupa - 1].vertices[n - 1], holes[broj_rupa - 1].vertices[0]).draw(image);
				segmenti_rupa.push_back(Segment(holes[broj_rupa - 1].vertices[n - 1], holes[broj_rupa - 1].vertices[0]));
				every_vertex.clear();
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
				every_vertex.push_back(vertex);
			}


		}
		inputFileHoles.close();
	}*/
}
//---------------------------------------------------------------------------


template <typename TipListe>
void pomjeriIteratorNaprijed(typename TipListe::iterator &it, TipListe &lista) {
	it++;
	if (it == lista.end())
		it = lista.begin();
}

//---------------------------------------------------------------------------

template <typename TipListe>
void pomjeriIteratorNazad(typename TipListe::iterator &it, TipListe &lista) {
	if (it == lista.begin())
		it = lista.end();
	it--;
}

//---------------------------------------------------------------------------

void __fastcall Tagp_aplication::TriangulationButtonClick(TObject *Sender)
{
	// pretvorit vektor tacaka u listu i garantuj negativnu orijentaciju (da je unutrasnjost
	// uvijek sa desne strane)
	vector<Vertex> poligon = outside_polygon.vertices;
	vector<pair<int, int>> dijagonale_triangulacije;
	vector<vector<Vertex>> trouglovi;
	unordered_map<Vertex, int> vertexColors;
	list<int> lista_tacaka;
	for(int i=0; i<poligon.size();i++)
		lista_tacaka.push_back(i);

	auto trenutni = lista_tacaka.begin();
	auto prethodni = trenutni;
	pomjeriIteratorNazad<list<int>>(prethodni, lista_tacaka);

	auto naredni = trenutni;
	pomjeriIteratorNaprijed<list<int>>(naredni, lista_tacaka);

	while(lista_tacaka.size() > 3) {

		if (orientation(poligon[*prethodni], poligon[*trenutni], poligon[*naredni]) < 0) {
			// potencijalno uho
			auto provjera(naredni);
			Trougao t(poligon[*prethodni], poligon[*trenutni], poligon[*naredni]);
			pomjeriIteratorNaprijed(provjera, lista_tacaka);
			bool uho(true);
			// prodji kroz sve tacke iz liste i vidi da li je neka
			// od njih u trouglu kojeg trenutno provjeravam, ako jeste
			// onda to nije uho, a inace jeste
			while(provjera != prethodni) {
				if(is_a_Vertex_in_triangle(poligon[*provjera], t)) {
					uho = false;
					break;
				}
				pomjeriIteratorNaprijed(provjera, lista_tacaka);
			}
			if (uho) {
				// ako smo nasli uho, spasi tu dijagonalu i vrati iteratore za 1 mjesto
				dijagonale_triangulacije.push_back({*prethodni, *naredni});
				trouglovi.push_back({poligon[*prethodni], poligon[*trenutni], poligon[*naredni]});
				lista_tacaka.erase(trenutni);
				trenutni = prethodni;
				pomjeriIteratorNazad<list<int>>(prethodni, lista_tacaka);
				continue;
			}
		}
		// ako nismo nasli uho, nastavi dalje
		prethodni = trenutni;
		trenutni = naredni;
		pomjeriIteratorNaprijed(naredni, lista_tacaka);
	}

	for(auto &par_indeksa: dijagonale_triangulacije) {
		Segment(poligon[par_indeksa.first], poligon[par_indeksa.second]).draw(image, clGreen);
	}

	for (const vector<Vertex>& trougao : trouglovi) {
		// Color the first vertex of the triangle with color 0
		if (vertexColors.find(trougao[0]) == vertexColors.end()) {
			vertexColors[trougao[0]] = 0;
		}

		// Check the other two vertices of the triangle
		for (int i = 1; i < 3; ++i) {
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
	for (vector<Vertex>& trougao : trouglovi){
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
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

