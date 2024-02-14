//---------------------------------------------------------------------------

#pragma hdrstop

#include "helpers.h"
#include <iostream>
#include <fstream>
#include <sstream>
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
//DRAWING SIMPLE POLYGON
//---------------------------------------------------------------------------
void __fastcall Tgui::load_campus(TObject *Sender, string polygon_source, string holes_source) //ALMA OBJASNI
{
	std::ifstream inputFileCampus("./Campuses/simple_polygon.txt");
	if (inputFileCampus.is_open()) {
		string line;
		while (getline(inputFileCampus, line)) {
			double x, y;
			std::stringstream lineStream(line);
			lineStream>>x>>y;
			Vertex vertex(x,y);
			outside_polygon.vertices.push_back(vertex);
			vertex.draw(image);
		}
		int n(outside_polygon.vertices.size());
		Segment(outside_polygon.vertices[n - 1], outside_polygon.vertices[0]).draw(image);
		outside_polygon.fill_with_color(image);
		inputFileCampus.close();
	}
	std::ifstream inputFileHoles("C:/Users/eminm/OneDrive/Desktop/Projects/Adis Projekti/AGP_with_holes-programing-expert/Builder_code/holes.txt");
	if (inputFileHoles.is_open()) {
		string line;
		int broj_rupa(holes.size());
		while (getline(inputFileHoles, line)) {
			double x, y;
			if (line.empty()) {
				int n(holes[broj_rupa - 1].vertices.size());
				Segment(holes[broj_rupa - 1].vertices[n - 1], holes[broj_rupa - 1].vertices[0]).draw(image);
				holes[broj_rupa - 1].fill_with_color(image, clWhite);
				broj_rupa++;
				holes.push_back(Simple_polygon());
			} else {
				std::stringstream lineStream(line);
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

#pragma package(smart_init)
