//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "polygon.h"
#include <string>
#include <cstdio>
#include <cmath>
#include "agp.h"
#include "main.h"
#include "help_functions.h"
//---------------------------------------------------------------------------
class Tgui : public TForm
{
__published:	// IDE-managed Components
	TImage *image;
	TRadioButton *drawing_polygon;
	TRadioButton *drawing_hole;
	TRadioButton *drawing_camera;
	TButton *finish_object;
	TButton *draw_simple_campus;
	TButton *draw_complex_campus;
	TButton *color_vertices_agp;
	TButton *area_based_agp;
	TButton *advance_area_based_agp;
	TButton *check_position_agp;
	TButton *clean_screen;
	TButton *draw_delaunay_triangulation;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *coverage_percentage;
	TEdit *num_cameras;
	TLabel *Label3;
	TEdit *camera_radius;
	TLabel *Label4;
	TEdit *camera_fill_angle;
	TLabel *Label5;
	TEdit *camera_accuracy;
	TButton *calculate_coverage;
	TButton *genetic_agp;
	TEdit *inputNumCameras;
	void __fastcall image_mouse_down(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall image_mouse_up(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall clean_screen_click(TObject *Sender);
	void __fastcall finish_object_click(TObject *Sender);
	void __fastcall load_campus(string polygon_source, string holes_source);
	void __fastcall draw_simple_campus_click(TObject *Sender);
	void __fastcall draw_complex_campus_click(TObject *Sender);
	void __fastcall draw_delaunay_triangulation_click(TObject *Sender);
	void __fastcall color_vertices_agp_click(TObject *Sender);
	void __fastcall area_based_agp_click(TObject *Sender);
	void __fastcall advance_area_based_agp_click(TObject *Sender);
	void __fastcall check_position_agp_click(TObject *Sender);
	void __fastcall genetic_agp_click(TObject *Sender);
    void __fastcall calculate_coverage_click(TObject *Sender);
private:	// User declarations
	void do_delaunay_triangulation();
	double covered_surface_area();
	void color_vertices();
	void area_based();
	void advanced_area_based(vector<Camera>& available_cameras);
	void check_position(vector<Camera>& available_cameras);
	void genetic(vector<Camera> available_cameras, int population_size, int num_generations,
	 double cut_off, int minimum_surface_area);
	double fill_cameras(vector<pair<Triangle,int>>& camera_positions, vector<Camera>& available_cameras);
    vector<pair<vector<pair<Triangle, int>>, double>> initilize_population( int population_size,
	vector <Camera>& available_cameras, vector<Triangle>& potential_triangles);
	void draw_camera(int direction_x, int direction_y, Vertex center, TColor color, TImage* image,
	 Simple_polygon& outside_polygon, vector<Simple_polygon>& holes);
	double clear_and_draw();
	vector<Simple_polygon> make_polygons_to_hit(); //EMIN: koristi ovu funkciju za pravljenje polygona za hit
	void draw_camera(Vertex center, Vertex fst_neighbor, Vertex snd_neighbor, vector<Simple_polygon> polygons_to_hit, bool circled=false);
	Simple_polygon outside_polygon;
	vector<Simple_polygon> holes;
	vector<Camera> cameras;
	vector<Triangle> delaunay_triangles;
public:		// User declarations
	__fastcall Tgui(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tgui *gui;
//---------------------------------------------------------------------------
#endif
