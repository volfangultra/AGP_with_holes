//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include <string>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tagp_aplication *agp_aplication;
int start_x, start_y;
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

	Dot nova(X, Y);
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
		Dot center(start_x, start_y);
		int direction_x = X - start_x;
		int direction_y = Y - start_y;

		Simple_polygon view;
		Camera c(view);

		double alpha = c.RADIUS/sqrt(direction_x*direction_x + direction_y*direction_y);
		Dot end(start_x + alpha * direction_x, start_y + alpha * direction_y);

		c.view.vertices.push_back(center);

		Dot temp = end.rotate(center, c.FILL_ANGLE /2);
		for(int i = 0; i < c.ACCURACY; i++){
            c.view.vertices.push_back(temp);
			temp = temp.rotate(center, -1 * c.FILL_ANGLE/c.ACCURACY);

		}

		c.view.vertices.push_back(temp);

        //Presjec view sa svim ostalim kamerama i rupama

		c.view.draw(image);
        c.view.fill_color(image, clYellow);

		cameras.push_back(Camera(view));
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
	holes = vector<Simple_polygon>{Simple_polygon()};

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

