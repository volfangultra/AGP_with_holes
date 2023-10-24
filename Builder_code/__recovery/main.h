//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <vector>
#include "poligon.h"
//---------------------------------------------------------------------------
class Tagp_aplication : public TForm
{
__published:	// IDE-managed Components
	TEdit *input_num_vertices_polygon;
	TButton *generate;
	TRadioButton *radio_draw_hole;
	TRadioButton *radio_draw_polygon;
	TRadioButton *radio_add_camera;
	TButton *button_finish_object;
	TImage *image;
	TButton *button_clean;
	TEdit *text_coordinate;
	TLabel *label_coordinates;
	TLabel *label_surface_area_covered;
	TEdit *text_surface_area_covered;
	TButton *Button1;
	void __fastcall image_mouse_down(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall image_mouse_up(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall image_mouse_move(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall button_clean_click(TObject *Sender);
	void __fastcall button_finish_object_click(TObject *Sender);
	void __fastcall PolygonsIntersectionClick(TObject *Sender);
private:	// User declarations
	Simple_polygon outside_polygon;
	vector<Simple_polygon> holes{Simple_polygon()};
    vector<Camera> cameras;


public:		// User declarations
	__fastcall Tagp_aplication(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tagp_aplication *agp_aplication;

//---------------------------------------------------------------------------
#endif
