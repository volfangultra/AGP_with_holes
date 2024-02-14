//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tgui *gui;
int start_x, start_y;
bool started_hole = true;
//---------------------------------------------------------------------------
//Start of GUI
__fastcall Tgui::Tgui(TComponent* Owner): TForm(Owner), outside_polygon(),holes(), cameras(),
delaunay_triangles() {
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	drawing_polygon->Checked = true;
	camera_radius->Text = "150";
	camera_fill_angle->Text="1";
    camera_accuracy->Text="100";
}
//---------------------------------------------------------------------------
//On Mouse Down inside the image
void __fastcall Tgui::image_mouse_down(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
	Vertex new_vertex(X, Y); //We create a new vertex
	if(drawing_polygon->Checked) {//If we are drawing a polygon adding it to a polygon
		outside_polygon.vertices.push_back(new_vertex);
		int n = outside_polygon.vertices.size();
		if(n > 1) {
			Segment(outside_polygon.vertices[n - 2], new_vertex).draw(image);
		}
		new_vertex.draw(image);
	}
	else if(drawing_hole->Checked){//If we are drawing a hole adding it to a hole
		if(started_hole)   //If we started drawing a new hole we need a new polygon
			holes.push_back(Simple_polygon());
		if(holes.size() == 0){
			holes.push_back(Simple_polygon());
		}
		holes[holes.size() - 1].vertices.push_back(new_vertex);
		int n = holes[holes.size() - 1].vertices.size();
		if(n > 1){
		   Segment(holes[holes.size() - 1].vertices[n - 2], new_vertex).draw(image);
		}
		new_vertex.draw(image);
		started_hole=false;
	}
	else if(drawing_camera->Checked){//If we are drawing a camera setting the origin of the camera
		start_x = X;
		start_y = Y;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgui::image_mouse_up(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
	if(drawing_camera->Checked){
		if(start_x == X && start_y == Y)
			return;
		Vertex center(start_x, start_y); //Center of the camera becomes where the mouse was pressed
		Vertex direction(X - start_x, Y - start_y);//Direction becomes where the mouse was released
		vector<Simple_polygon> all_polygons = holes;
		if(outside_polygon.vertices.size() != 0)
			all_polygons.push_back(outside_polygon);

		int radius, accuracy;
		float fill_angle;

		radius = camera_radius->Text.ToInt();
		accuracy = camera_accuracy->Text.ToInt();
		fill_angle = camera_fill_angle->Text.ToDouble();   //We extract the camera type



		Camera c(center, direction, all_polygons, true, false, fill_angle, radius, accuracy);
		cameras.push_back(c);
		c.draw(image);
        c.fill_color(image);
		num_cameras->Text = to_string(cameras.size()).c_str(); //We set the number of cameras
	}
}
//---------------------------------------------------------------------------
//Cleans the screen and resets all variables
void __fastcall Tgui::clean_screen_click(TObject *Sender)
{
    image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	outside_polygon = Simple_polygon();
	holes = vector<Simple_polygon>();
	cameras = vector<Camera>();
	delaunay_triangles = vector<Triangle>();
	num_cameras->Text = "";
    coverage_percentage->Text = "";
}
//---------------------------------------------------------------------------
//Finishes the object that is currently being drawn be it hole or polygon
void __fastcall Tgui::finish_object_click(TObject *Sender){
	if(drawing_polygon->Checked){
		int n(outside_polygon.vertices.size());
		if (n < 3)
			return;
		Segment(outside_polygon.vertices[n - 1], outside_polygon.vertices[0]).draw(image);
		outside_polygon.fill_with_color(image);
	}
	else if(drawing_hole->Checked){
		int broj_rupa(holes.size());
		if(holes.size() < 1)
			return;
		int n(holes[holes.size() - 1].vertices.size());
		if(n < 3)
			return;
		Segment(holes[holes.size() - 1].vertices[n - 1], holes[holes.size() - 1].vertices[0]).draw(image);
		holes[holes.size() - 1].fill_with_color(image, true, clWhite);
		started_hole=true;
	}
}
//---------------------------------------------------------------------------
//Drawing a Simple Campuse
void __fastcall Tgui::draw_simple_campus_click(TObject *Sender)
{
	//Have to change this to your route
	load_campus("C:/Users/ACER/Desktop/AGP Final_2/Campuses/simple_polygon.txt",
				"C:/Users/ACER/Desktop/AGP Final_2/Campuses/simple_holes.txt");
}
//---------------------------------------------------------------------------
//Drawing a Complex Campus
void __fastcall Tgui::draw_complex_campus_click(TObject *Sender)
{
	//Have to change this to your route
	load_campus("C:/Users/ACER/Desktop/AGP Final_2/Campuses/complex_polygon.txt",
				"C:/Users/ACER/Desktop/AGP Final_2/Campuses/complex_holes.txt");
}
//---------------------------------------------------------------------------
//Calculate Coverage
void __fastcall Tgui::calculate_coverage_click(TObject *Sender)
{
	double res = covered_surface_area();
	ShowMessage(res);
	int temp = res * 100;
	string text = to_string(temp) + "%";
	coverage_percentage->Text = text.c_str();
}
//---------------------------------------------------------------------------
//Delaunay Triangulation
void __fastcall Tgui::draw_delaunay_triangulation_click(TObject *Sender)
{
	do_delaunay_triangulation();
	for(auto triangle:delaunay_triangles){
		triangle.draw(image);
	}

}
//---------------------------------------------------------------------------
//Greedy method based on coloring triangle vertices in 3 colors
void __fastcall Tgui::color_vertices_agp_click(TObject *Sender)
{
	do_delaunay_triangulation();
	color_vertices();
}
//---------------------------------------------------------------------------
//Greedy method based on comparing triangles area
void __fastcall Tgui::area_based_agp_click(TObject *Sender)
{
	do_delaunay_triangulation();
	area_based();
}
//---------------------------------------------------------------------------
//Greedy method based on comparing triangles area with more camera types
void __fastcall Tgui::advance_area_based_agp_click(TObject *Sender)
{
	do_delaunay_triangulation();
	//we sort cameras in descending order
	sort(delaunay_triangles.begin(), delaunay_triangles.end(), compareByArea);
	vector<Camera> available_cameras;
	advanced_area_based(available_cameras);
}
//---------------------------------------------------------------------------
//Greedy method based on checking every possible position for cameras and finding best one
void __fastcall Tgui::check_position_agp_click(TObject *Sender)
{
	do_delaunay_triangulation();
    //we sort cameras in descending order
	sort(delaunay_triangles.begin(), delaunay_triangles.end(), compareByArea);
	vector<Camera> available_cameras;
	check_position(available_cameras);
}
//---------------------------------------------------------------------------
//Genetic algorithm that solves the AGP
void __fastcall Tgui::genetic_agp_click(TObject *Sender)
{
	do_delaunay_triangulation();
	vector<Camera> available_cameras;
	//We will make 10 normal cameras
	for (int i = 0; i < 10; i++){
		available_cameras.push_back(Camera(false, true, 2*3.14159265359 / 3, 160, 100));
		//These cameras are just for showing what type to use so they do not have an origin
	}
	//Now we will make 4 circle cameras
	for (int i = 0; i < 4; i++){
		available_cameras.push_back(Camera(false, true, 2*3.14159265359, 64, 100));
		//These cameras are just for showing what type to use so they do not have an origin
	}

	genetic(available_cameras, 5, 5, 0.3, 300);

}
//---------------------------------------------------------------------------

