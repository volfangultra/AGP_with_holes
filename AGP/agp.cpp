//---------------------------------------------------------------------------

#pragma hdrstop

#include "agp.h"
using namespace std;
//---------------------------------------------------------------------------
//COLOR VERTICES
//---------------------------------------------------------------------------
void Tgui::color_vertices()
{
	unordered_map<Vertex, int> vertexColors;
	int red=0,yellow=0,green=0;

	for (auto triangle : delaunay_triangles) {
		for (int i = 0; i < 3; ++i) {
			Vertex vertex = (i == 0) ? triangle.A : ((i == 1) ? triangle.B : triangle.C);

			// If the vertex is not colored, color it with a color different from its neighbors
			if (vertexColors.find(vertex) == vertexColors.end()) {
				set<int> neighborColors;
				Vertex fst_neighbor = (i == 0) ? triangle.B : ((i == 1) ? triangle.C : triangle.A);
				Vertex snd_neighbor = (i == 0) ? triangle.C : ((i == 1) ? triangle.A : triangle.B);
				if (vertexColors.find(fst_neighbor) != vertexColors.end())
					neighborColors.insert(vertexColors[fst_neighbor]);
				if (vertexColors.find(snd_neighbor) != vertexColors.end())
					neighborColors.insert(vertexColors[snd_neighbor]);

				// Assign the first available color not used by neighbors
				for (int color = 0; color < 3; ++color) {
					if (neighborColors.find(color) == neighborColors.end()) {
						if(color == 0) red++;
						else if (color == 1) yellow++;
						else green++;
						vertexColors[vertex] = color;
						break;
					}
				}
			}
		}
	}

	//Determine the color with minimal repetitions
	int minimal = min(red, min(yellow, green));
	int minColor;
	if(minimal == red)
		minColor = 0;
	else if(minimal == yellow)
		minColor = 1;
	else minColor = 2;

	vector<Vertex> used_vertex;

	//Now we just draw cameras
	vector<Simple_polygon> polygons_to_hit = make_polygons_to_hit();
	for (auto triangle : delaunay_triangles) {
		for (int i = 0; i < 3; ++i) {
			Vertex vertex = (i == 0) ? triangle.A : ((i == 1) ? triangle.B : triangle.C);
			Vertex fst_neighbor = (i == 0) ? triangle.B : ((i == 1) ? triangle.A : triangle.A);
			Vertex snd_neighbor = (i == 0) ? triangle.C : ((i == 1) ? triangle.C : triangle.B);
			//Draw a camera if the vertex has the minimal color and it's not already used
			if (vertexColors[vertex] == minColor && find(used_vertex.begin(), used_vertex.end(), vertex)==used_vertex.end()) {
				used_vertex.push_back(vertex);
				draw_camera(vertex, fst_neighbor, snd_neighbor, polygons_to_hit);

			}
		}
	}

	num_cameras->Text = to_string(cameras.size()).c_str();
}
//---------------------------------------------------------------------------
//AREA BASED
//---------------------------------------------------------------------------
void Tgui::area_based()
{
	//Iterate over each triangle in the Delaunay triangulation
	for (auto triangle : delaunay_triangles) {

		//Calculate the area of the current triangle
		double area = Simple_polygon({triangle.A, triangle.B, triangle.C}).surface_area();
		vector<Simple_polygon> polygons_to_hit = make_polygons_to_hit();

		//Place cameras based on the area of the triangle
		if(area > 1500){
			//If the area is greater than 1500, place a camera at vertex A
			draw_camera(triangle.A, triangle.B, triangle.C, polygons_to_hit);
		}
		if(area >= 6000){
			//If the area is greater than or equal to 6000, place a camera at vertex C
			draw_camera(triangle.C, triangle.B, triangle.A, polygons_to_hit);
		}
		num_cameras->Text = to_string(cameras.size()).c_str();
	}
};
//---------------------------------------------------------------------------
//ADVANCED AREA BASED
//---------------------------------------------------------------------------
void Tgui::advanced_area_based(vector<Camera>& available_cameras)
{
    //If there is no input for number of cameras, default number is 10
	int num_of_cameras = inputNumCameras->Text != "" ?
							inputNumCameras->Text.ToInt() :
							10;
	vector<Simple_polygon> polygons_to_hit = make_polygons_to_hit();
	int counter = 0;

	//Same logic as "area based" algorithm, just with more types of cameras
	for (auto triangle : delaunay_triangles){
		double area = 0;
		area = Simple_polygon({triangle.A, triangle.B, triangle.C}).surface_area();

		//For smaller trinagles we put circled camera
		if(area >= 1000 && area <= 6000){
			Vertex center((triangle.A.x + triangle.B.x + triangle.C.x)/3, (triangle.A.y + triangle.B.y + triangle.C.y)/3);
			draw_camera(center, triangle.B, triangle.A, polygons_to_hit, true);
			counter++;
			if (counter == num_of_cameras)
				break;
		}
		//For larger triangles, place cameras at specific vertices
		if(area >= 8000){
			draw_camera(triangle.C, triangle.B, triangle.A, polygons_to_hit);
			counter++;
			if (counter == num_of_cameras)
				break;
		}
		if(area >= 12000){
			draw_camera(triangle.B, triangle.C, triangle.A, polygons_to_hit);
			counter++;
			if (counter == num_of_cameras)
				break;
		}
		if(area >= 16000){
			draw_camera(triangle.A, triangle.C, triangle.B, polygons_to_hit);
			counter++;
			if (counter == num_of_cameras)
				break;

		}
	}
	num_cameras->Text = to_string(cameras.size()).c_str();
};
//---------------------------------------------------------------------------
//CHECK POSITIONS
//---------------------------------------------------------------------------
void Tgui::check_position(vector<Camera>& available_cameras)
{
	//If there is no input for number of cameras, default number is 10
	int num_of_cameras = inputNumCameras->Text != "" ?
							inputNumCameras->Text.ToInt() :
							10;
	int counter = 0;
	double general_coverage = 0;
	double epsilon = 0;
	vector<Simple_polygon> polygons_to_hit = make_polygons_to_hit();
	for (auto triangle : delaunay_triangles){
		//Break the loop if the desired number of cameras is reached
		if (counter >= num_of_cameras)
			break;

		double coverage = 0;
		int best_position = 0;
		//Checking every (4) possible position for placing the camera and remembering best position
		for (int i = 0; i < 4; i++) {
			if(i != 3) {
				//These camera positions are trinagle vertices
				Vertex vertex = (i == 0) ? triangle.A : ((i == 1) ? triangle.B : triangle.C);
				Vertex fst_neighbor = (i == 0) ? triangle.B : ((i == 1) ? triangle.A : triangle.A);
				Vertex snd_neighbor = (i == 0) ? triangle.C : ((i == 1) ? triangle.C : triangle.B);
				draw_camera(vertex, fst_neighbor, snd_neighbor, polygons_to_hit);
			} else {
				//This camera position is triangle centroid
				Vertex center;
				center.x = (triangle.A.x + triangle.B.x + triangle.C.x)/3;
				center.y = (triangle.A.y + triangle.B.y + triangle.C.y)/3;
				draw_camera(center, triangle.B, triangle.A, polygons_to_hit, true);
			}

			//Calculate the coverage after placing the camera
			double current_coverage = covered_surface_area();
			//Update coverage and best position if necessary
			if (coverage < current_coverage) {
				coverage = current_coverage;
				best_position = i;
			}

			//Clear and draw the scene for the next camera placement
			double state = clear_and_draw();
		}
		//Placing camera in best position if camera makes progress in covering campus
		if((coverage - general_coverage > 0.1-epsilon && best_position!=3) || (coverage - general_coverage > 0.045-epsilon && best_position==3)) {
			if(best_position != 3) {
				Vertex vertex = (best_position == 0) ? triangle.A : ((best_position == 1) ? triangle.B : triangle.C);
				Vertex fst_neighbor = (best_position == 0) ? triangle.B : ((best_position == 1) ? triangle.A : triangle.A);
				Vertex snd_neighbor = (best_position == 0) ? triangle.C : ((best_position == 1) ? triangle.C : triangle.B);
				draw_camera(vertex, fst_neighbor, snd_neighbor, polygons_to_hit);
			} else {
				Vertex center;
				center.x = (triangle.A.x + triangle.B.x + triangle.C.x)/3;
				center.y = (triangle.A.y + triangle.B.y + triangle.C.y)/3;
				draw_camera(center, triangle.B, triangle.A, polygons_to_hit, true);
			}

            //Increment the camera counter and update the general coverage
			counter++;
			general_coverage = coverage;
		}
		//in every loop step we reduce constraint for putting camera by increasing hyperparameter epsilon
		epsilon+=0.003;


		num_cameras->Text = to_string(cameras.size()).c_str();
	}
};
//---------------------------------------------------------------------------
//GENETIC
//---------------------------------------------------------------------------
void Tgui::genetic(vector<Camera> available_cameras, int population_size, int num_generations,
double cut_off=0.5, int minimum_surface_area=100)
{

	vector<float> array_of_coverages;
	vector<Triangle> important_triangles;
    //we only look at triangles that are big enough decided by minimum_surface_area
	for (auto triangle:delaunay_triangles)
		if(Simple_polygon({triangle.A, triangle.B, triangle.C}).surface_area() > minimum_surface_area)
			important_triangles.push_back(triangle);

	//We randomly place cameras in triangles
	auto population = initilize_population(population_size, available_cameras, important_triangles);

	//we calculate the coverage for each memeber of a population
	for (int i = 0; i < num_generations; i++){
		for (int i = 0; i < population_size; i++){
			population[i].second = fill_cameras(population[i].first, available_cameras);
		}

		//We keep the best of each generation in case of troubleshooting and looking at convergence
		float current_best = 0;
		for(int i = 0; i < population.size(); i++){
			if(population[i].second > current_best)
				current_best = population[i].second;
		}
		array_of_coverages.push_back(current_best);

		//We sort the population to find the top members
		sort(population.begin(), population.end(),
		[](auto a, auto b) {return a.second > b.second; });

		if(i == num_generations - 1)
			break;

		//We create a new population by combining the best members of the old population
        int int_cut_off = int(1.0/cut_off); //This represents where the cut_off is for a population
		vector<pair<vector<pair<Triangle, int>>, double>> new_population;
		while(new_population.size() < population.size()){
			auto parent_1 = rand()%(population.size() / int_cut_off);
			auto parent_2 = rand()%(population.size() / int_cut_off);
			auto intersection = rand()%(available_cameras.size());
			vector<pair<Triangle, int>> temp;
			for(int i = 0; i < intersection; i++)
				temp.push_back(population[parent_1].first[i]);

			for(int i = intersection; i < available_cameras.size(); i++)
				temp.push_back(population[parent_2].first[i]);

			new_population.push_back({temp, 0.0});
		}

		population = new_population;

	}

	fill_cameras(population[0].first, available_cameras);
	string res = "";
	for(int i = 0; i < array_of_coverages.size(); i++)
		res += to_string(array_of_coverages[i]) + " ";

};
//---------------------------------------------------------------------------
#pragma package(smart_init)

