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
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tagp_aplication *agp_aplication;
int start_x, start_y;
using namespace std;
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
		nova.draw(image, clGreen);
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

		c.draw(image);
        c.fill_color(image);

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
	double sum = 0;
	for (int i=0; i<holes.size(); i++) {
	   sum += holes[i].surface_area();
	}

	ShowMessage(outside_polygon.surface_area() - sum);
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
	ifstream inputFileCampus("C:/Users/ACER/Desktop/AGP/Builder_code/campus.txt");
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

	ifstream inputFileHoles("C:/Users/ACER/Desktop/AGP/Builder_code/holes.txt");
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
		inputFileCampus.close();
	}
}
//---------------------------------------------------------------------------


