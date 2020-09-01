#ifndef VERTICES_H
#define VERTICES_H

#include <iostream>
#include <vector>
#include "common.h"
;

namespace graphtinker
{
	class Vertices
	{
    private:
		uint num_vertices_;
		vector<vertex_property_t> vertex_properties_;
	public:
		Vertices();
		Vertices(uint num_vertices, float init_value);
		~Vertices();

		void InitVerticesPropertyArray(float init_value);
		vertexdata_t ReadData(vertexid_t vertexid);
		void WriteData(vertexid_t vertexid, vertexdata_t vertexdata);
		vertex_property_t ReadProperty(vertexid_t vertexid);
		void WriteProperty(vertexid_t vertexid, vertex_property_t vertexproperty);
		void IncreaseOutdegree(vertexid_t vertexid);
		void UpdateVertexProperty(vertexid_t vtx_id, vertexid_t adjvtx_id, uint edge_update_cmd, bool is_directed);
		void PrintVerticesProperty(uint n);
		void PrintNthVertexProperty(uint n);

		// getter and setter
		const uint num_vertices() const;
		const vector<vertex_property_t> &vertex_properties() const;

	};
} // namespace graphtinker

#endif