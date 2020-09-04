#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "core/vertices.h" // Courtesy : Jinja 2.0
;

namespace graphtinker
{
	Vertices::Vertices(){}
	Vertices::Vertices(uint num_vertices, float init_value){
	
		DLOG(INFO) << "Vertices::Vertices : Vertices constructor called"  ;
		num_vertices_ = num_vertices;
		vertex_properties_.resize(num_vertices_);
		InitVerticesPropertyArray(init_value);
	}
	Vertices::~Vertices() {}

	void Vertices::InitVerticesPropertyArray(float init_value)
	{
		DLOG(INFO) << "initializing Vertices (Vertices) "  ;
		for (uint i = 0; i < num_vertices_; i++)
		{
			vertex_properties_[i].data = init_value;
			vertex_properties_[i].indegree = 0;
			vertex_properties_[i].outdegree = 0;
			vertex_properties_[i].flag = INVALID;
		}
		return;
	}
	vertexdata_t Vertices::ReadData(vertexid_t vertexid)
	{
		if (vertexid >= num_vertices_)
		{
			LOG(ERROR) << "Vertices::ReadData : out of range4. vertexid : " << vertexid << ", num_vertices_ : " << num_vertices_  ;
		}
		return vertex_properties_[vertexid].data;
	}

	void Vertices::WriteData(vertexid_t vertexid, vertexdata_t vertexdata)
	{
		if (vertexid >= num_vertices_)
		{
			LOG(ERROR) << "Vertices::WriteData : out of range5. vertexid : " << vertexid << ", num_vertices_ : " << num_vertices_  ;
		}
		vertex_properties_[vertexid].data = vertexdata;
		return;
	}

	vertex_property_t Vertices::ReadProperty(vertexid_t vertexid)
	{
		if (vertexid >= num_vertices_)
		{
			LOG(ERROR) << "Vertices::ReadProperty : out of range6. vertexid : " << vertexid << ", num_vertices_ : " << num_vertices_  ;
		}
		return vertex_properties_[vertexid];
	}

	void Vertices::WriteProperty(vertexid_t vertexid, vertex_property_t vertexproperty)
	{
		if (vertexid >= num_vertices_)
		{
			LOG(ERROR) << "Vertices::WriteProperty : out of range7. vertexid : " << vertexid << ", num_vertices_ : " << num_vertices_  ;
		}
		vertex_properties_[vertexid] = vertexproperty;
		return;
	}

	void Vertices::IncreaseOutdegree(vertexid_t vertexid)
	{
		if (vertexid >= num_vertices_)
		{
			LOG(ERROR) << "Vertices::IncreaseOutdegree : out of range5. vertexid : " << vertexid << ", num_vertices_ : " << num_vertices_  ;
		}
		vertex_properties_[vertexid].outdegree += 1;
		return;
	}

	void Vertices::UpdateVertexProperty(vertexid_t vtx_id, vertexid_t adjvtx_id, bool is_insert_edge, bool is_directed)
	{
		if (is_insert_edge)
		{
			if (is_directed)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].outdegree += 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].indegree += 1;
				}
			}
			else if (is_directed == UNDIRECTEDGRAPH)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].outdegree += 1;
				}
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].indegree += 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].outdegree += 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].indegree += 1;
				}
			}
			else if (is_directed == REVERSE_DIRECTED_GRAPH)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].indegree += 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].outdegree += 1;
				}
			}
			else
			{
				LOG(ERROR) << "Vertices::UpdateVertexProperty : error1"  ;
			}
		}
		else if (!is_insert_edge) /*delete edge*/
		{
			if (is_directed)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].outdegree -= 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].indegree -= 1;
				}
			}
			else if (is_directed == UNDIRECTEDGRAPH)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].outdegree -= 1;
				}
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].indegree -= 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].outdegree -= 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].indegree -= 1;
				}
			}
			else if (is_directed == REVERSE_DIRECTED_GRAPH)
			{
				if (vtx_id < num_vertices_)
				{
					vertex_properties_[vtx_id].indegree -= 1;
				}
				if (adjvtx_id < num_vertices_)
				{
					vertex_properties_[adjvtx_id].outdegree -= 1;
				}
			}
			else
			{
				LOG(ERROR) << "Vertices::UpdateVertexProperty : error2"  ;
			}
		}
		return;
	}

	void Vertices::PrintVerticesProperty(uint n)
	{
		for (uint i = 0; i < n; i++)
		{
			LOG(INFO) << "vertexid : " << i << " ";
			LOG(INFO) << "data : " << vertex_properties_[i].data << " ";
			LOG(INFO) << "indegree : " << vertex_properties_[i].indegree << " ";
			LOG(INFO) << "outdegree : " << vertex_properties_[i].outdegree << " ";
			LOG(INFO) << "flag : " << vertex_properties_[i].flag << " ";
			LOG(INFO)  ;
		}
		return;
	}

	void Vertices::PrintNthVertexProperty(uint n)
	{
		LOG(INFO) << "vertexid : " << n << " ";
		LOG(INFO) << "data : " << vertex_properties_[n].data << " ";
		LOG(INFO) << "indegree : " << vertex_properties_[n].indegree << " ";
		LOG(INFO) << "outdegree : " << vertex_properties_[n].outdegree << " ";
		LOG(INFO) << "flag : " << vertex_properties_[n].flag << " ";
		LOG(INFO)  ;
		return;
	}

	// getter 
	const vector<vertex_property_t> &Vertices::vertex_properties() const { return vertex_properties_; }
	const uint Vertices::num_vertices() const { return num_vertices_; }
} // namespace graphtinker