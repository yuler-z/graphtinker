#ifndef VERTICES_H
#define VERTICES_H

#include <iostream>
#include <vector>
#include "common.h"
using namespace std;

namespace gt
{
	class vertices
	{
	public:
		vertices();
		~vertices();

		uint num_vertices;

		void initialize(uint _num_vertices, float initvalue); // very important!

		vector<vertexproperty_t> vertex_properties;

		vertexdata_t readdata(vertexid_t vertexid);
		void writedata(vertexid_t vertexid, vertexdata_t vertexdata);
		vertexproperty_t readproperty(vertexid_t vertexid);
		void writeproperty(vertexid_t vertexid, vertexproperty_t vertexproperty);
		void increment_outdegree(vertexid_t vertexid);
		void update_vertex_property(vertexid_t xvtx_id, vertexid_t xadjvtx_id, uint edgeupdatecmd, uint graphdirectiontype);
		void initialize(float initvalue);
		void print_first_n(uint n);
		void print_nth_vertex(uint n);
		vector<vertexproperty_t> &get_vertex_properties();
		uint get_num_vertices();

	private:
	};
}

#endif