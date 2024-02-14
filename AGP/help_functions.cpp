//---------------------------------------------------------------------------

#pragma hdrstop

#include "help_functions.h"
using namespace std;
//---------------------------------------------------------------------------
//DELAUNAY TRIANGULATION
//---------------------------------------------------------------------------
void Tgui::do_delaunay_triangulation(){
	if(delaunay_triangles.size() == 0){ // If a delaunay triangulation was already done no need to do it again
		CDT::Triangulation<double> cdt; //We use a constrained delaunay triangulation
		vector<Vertex> all_vertices = outside_polygon.vertices;
		for(auto hole:holes)
			for(auto vertex:hole.vertices)
				all_vertices.push_back(vertex);
		cdt.insertVertices(
			all_vertices.begin(),
			all_vertices.end(),
			[](const Vertex& p){ return p.x; },
			[](const Vertex& p){ return p.y; }
		); //We insert all vertices in CDT
		vector<pair<int,int>> segments; //WE insert segments by using their indexes in all_vertices
		int n = outside_polygon.vertices.size();
		int brojac = 0;
		for(int i = 0; i < n; i++){
			segments.push_back({i, (i+1)%n});
			brojac++;
		}
		for(int i = 0; i < holes.size(); i++){
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
		); //Inserted all segments in CDT
		cdt.eraseOuterTrianglesAndHoles();
		unordered_set<CDT::Edge> triangle_segments = CDT::extractEdgesFromTriangles(cdt.triangles);
		vector<CDT::Triangle> triangles = cdt.triangles; //We extract Triangles from CDT
		for(auto triangle : triangles){
			auto A = triangle.vertices[0];
			auto B = triangle.vertices[1];
			auto C = triangle.vertices[2];
			delaunay_triangles.push_back(Triangle(all_vertices[A], all_vertices[B], all_vertices[C]));
		}

    }
}
//---------------------------------------------------------------------------
//CALCULATING COVERAGE
//---------------------------------------------------------------------------
double Tgui::covered_surface_area() {
	int covered_area = 0;
	double blue_area = outside_polygon.surface_area();
	for(auto hole:holes)
		blue_area -= hole.surface_area(); //We subtract the holes from the covered area


	Graphics::TBitmap* bitmap = new Graphics::TBitmap;
	bitmap->Assign(image->Picture->Bitmap);
	int imageWidth = bitmap->Width;
	int imageHeight = bitmap->Height;
	//#pragma omp parallel for reduction(+:covered_area) //If OpenMP is setup this would make it faster
	for (int y = 0; y < imageHeight; y++) {
		TRGBQuad* row = reinterpret_cast<TRGBQuad*>(bitmap->ScanLine[y]); //We scan line by line
		for (int x = 0; x < imageWidth; x++) {
			if (row[x].rgbReserved == 0 && row[x].rgbBlue == 0 && row[x].rgbGreen == 255 && row[x].rgbRed == 255) {
				covered_area += 1;
			}//Add 1 for each yellow pixel
		}
	}

	delete bitmap;

	if (blue_area == 0) {
		return 0;
	}

	return static_cast<double>(covered_area) / static_cast<double>(blue_area);
}
//---------------------------------------------------------------------------
//DRAWING POLYGON FROM TEXT FILE
//---------------------------------------------------------------------------
void __fastcall Tgui::load_campus(string polygon_source, string holes_source)
{
	//Open file containing polygon vertices
	//Each line contains one vertex coordinates
	ifstream inputFileCampus(polygon_source);

	if (inputFileCampus.is_open()) {
		string line;
		//Read each line of the file
		while (getline(inputFileCampus, line)) {
			double x, y;
			stringstream lineStream(line);
			//Create a vertex with extracted coordinates
			lineStream>>x>>y;
			Vertex vertex(x,y);
			//Add the vertex to the outside polygon and draw it
			outside_polygon.vertices.push_back(vertex);
			vertex.draw(image);
		}
		int n(outside_polygon.vertices.size());
		Segment(outside_polygon.vertices[n - 1], outside_polygon.vertices[0]).draw(image);
		outside_polygon.fill_with_color(image);
		inputFileCampus.close();
	}

	//Open file containing holes data
	//Each line contains one vertex coordinates
	ifstream inputFileHoles(holes_source);

	if (inputFileHoles.is_open()) {
		string line;
		bool started_hole = true;
		while (getline(inputFileHoles, line)) {
			double x, y;
			//Check if line is empty, indicating the end of a hole
			if (line.empty()) {
				int n(holes[holes.size() - 1].vertices.size());
				Segment(holes[holes.size() - 1].vertices[n - 1], holes[holes.size() - 1].vertices[0]).draw(image);
				holes[holes.size() - 1].fill_with_color(image, true, clWhite);
				started_hole = true;
			} else {
				//Start a new hole if it's the beginning
				if(started_hole)
					 holes.push_back(Simple_polygon());
				std::stringstream lineStream(line);
				//Create a vertex with extracted coordinates
				lineStream>>x>>y;
				Vertex vertex(x,y);
				//Add the vertex to the current hole and draw it
				holes[holes.size() - 1].vertices.push_back(vertex);
				vertex.draw(image);
                started_hole = false;
			}

		}
		inputFileHoles.close();
	}

}
//---------------------------------------------------------------------------
//MAKE POLYGONS THAT HITS WITH CAMERA
//---------------------------------------------------------------------------
vector<Simple_polygon> Tgui::make_polygons_to_hit(){
   vector<Simple_polygon> polygons_to_hit = holes;
   if(outside_polygon.vertices.size() != 0)
	polygons_to_hit.push_back(outside_polygon);

   return polygons_to_hit;
}
//---------------------------------------------------------------------------
//DRAW CAMERA FOR GREEDY METHODS
//---------------------------------------------------------------------------
void Tgui::draw_camera(Vertex center, Vertex fst_neighbor, Vertex snd_neighbor, vector<Simple_polygon> polygons_to_hit, bool circled){
	//Calculate the direction of the camera
	double direction_x = (fst_neighbor.x + snd_neighbor.x)/2;
	double direction_y = (fst_neighbor.y + snd_neighbor.y)/2;
	Vertex direction(direction_x - center.x, direction_y - center.y);

	int radius, accuracy;
	float fill_angle;

	//We extract the camera type
	accuracy = camera_accuracy->Text.ToInt();
	fill_angle = circled ? 2*3.141592 : camera_fill_angle->Text.ToDouble();
	radius = circled ? 64 : camera_radius->Text.ToInt();

    //Create camera object and draw it
	Camera c(center, direction, polygons_to_hit, !circled, circled, fill_angle, radius, accuracy);
	cameras.push_back(c);
	c.fill_color(image);
	c.draw(image);
}



//---------------------------------------------------------------------------
//CLEAR SCREEN AND DRAW AGAIN
//---------------------------------------------------------------------------
double Tgui::clear_and_draw(){
	//Now we clear the screen and draw everything again
	cameras.pop_back();
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	outside_polygon.draw(image);
	outside_polygon.fill_with_color(image);
	 for(auto hole:holes){
		hole.draw(image);
		hole.fill_with_color(image, true, clWhite);
	}
	for(auto camera: cameras){
		camera.fill_color(image);
	}

	return covered_surface_area();
}


//---------------------------------------------------------------------------
//FILL CAMERAS FOR GENETIC ALGORITHM
//---------------------------------------------------------------------------
//This puts cameras in their respective position and measures coverage
double Tgui::fill_cameras(vector<pair<Triangle, int>>& camera_positions, vector<Camera>& available_cameras){
	if(available_cameras.size() != camera_positions.size())
		throw "Bad input in fill cameras";
	cameras = vector<Camera>(); //Empty existing cameras

	for (int i = 0; i < camera_positions.size(); i++){
		int radius = available_cameras[i].radius;
		int accuracy = available_cameras[i].accuracy;
		double fill_angle = available_cameras[i].fill_angle;
		vector<Simple_polygon> polygons_to_hit = holes;
		if(outside_polygon.vertices.size() != 0)
			polygons_to_hit.push_back(outside_polygon);

		if(camera_positions[i].second == -1){ //Camera that goes in the center
			Vertex origin = camera_positions[i].first.get_centroid();
			Vertex direction(1,1);
			cameras.push_back(Camera(origin, direction, polygons_to_hit, false, true, fill_angle, radius, accuracy));
		}else if(camera_positions[i].second == 0){ //Camera at first point of triangle
			Vertex origin = camera_positions[i].first.A;
			double center = (camera_positions[i].first.B.x + camera_positions[i].first.C.x) / 2;
			Vertex direction(center, center);
			cameras.push_back(Camera(origin, direction, polygons_to_hit, true, false, fill_angle, radius, accuracy));
		}else if(camera_positions[i].second == 1){ //Camera at second point of triangle
			Vertex origin = camera_positions[i].first.B;
			double center = (camera_positions[i].first.A.x + camera_positions[i].first.C.x) / 2;
			Vertex direction(center, center);
			cameras.push_back(Camera(origin, direction, polygons_to_hit, true, false, fill_angle, radius, accuracy));
		}else{ //Camera at third point of triangle
			Vertex origin = camera_positions[i].first.C;
			double center = (camera_positions[i].first.A.x + camera_positions[i].first.B.x) / 2;
			Vertex direction(center, center);
			cameras.push_back(Camera(origin, direction, polygons_to_hit, true, false, fill_angle, radius, accuracy));

		}

	}

	//Now we clear the screen and draw everything again
	image->Canvas->Brush->Color = clWhite;
	image->Canvas->FillRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clBlack;
	image->Canvas->FrameRect(Rect(0, 0, image->Width, image->Height));
	image->Canvas->Brush->Color = clWhite;
	outside_polygon.draw(image);
	outside_polygon.fill_with_color(image);
	 for(auto hole:holes){
		hole.draw(image);
		hole.fill_with_color(image, true, clWhite);
	}
	for(auto camera: cameras){
		camera.fill_color(image);
	}

	return covered_surface_area();
}
//---------------------------------------------------------------------------
//INITILIZING POPULATION OF A GENETIC ALGORITHM
//---------------------------------------------------------------------------
//This positions cameras randomly in triangles
vector<pair<vector<pair<Triangle, int>>, double>> Tgui::initilize_population( int population_size,
vector <Camera>& available_cameras, vector<Triangle>& potential_triangles){
	vector<pair<vector<pair<Triangle, int>>, double>> res;
	for(int i = 0; i < population_size;i++){
		vector<pair<Triangle, int>>  temp;
		for(int i = 0; i < available_cameras.size(); i++){
			int rand_triangle = rand()%potential_triangles.size();//We selecet a random triangle
			int rand_position = rand()%3;//We select a random vertices of that triangle
			if (available_cameras[i].put_in_center)
				temp.push_back({potential_triangles[rand_triangle], -1});//This means we put these cameras in centroids
			else
				temp.push_back({potential_triangles[rand_triangle], rand_position});

		}
        res.push_back({temp, 0.0});
	}



	return res;

}
#pragma package(smart_init)
