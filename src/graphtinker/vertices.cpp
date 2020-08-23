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
#include "graphtinker/vertices.h" // Courtesy : Jinja 2.0
using namespace std;

namespace gt
{
	Vertices::Vertices(){}
	Vertices::Vertices(uint num_vertices, float init_value){
	
		LOG(INFO) << "Vertices::Vertices : Vertices constructor called"  ;
		num_vertices_ = num_vertices;
		vertex_properties_.resize(num_vertices_);
		InitVerticesPropertyArray(init_value);
	}
	Vertices::~Vertices() {}

	void Vertices::InitVerticesPropertyArray(float init_value)
	{
		LOG(INFO) << "initializing Vertices (Vertices) "  ;
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

	void Vertices::UpdateVertexProperty(vertexid_t xvtx_id, vertexid_t xadjvtx_id, uint edge_update_cmd, uint graphdirectiontype)
	{
		if (edge_update_cmd == INSERTEDGE)
		{
			if (graphdirectiontype == DIRECTEDGRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].outdegree += 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].indegree += 1;
				}
			}
			else if (graphdirectiontype == UNDIRECTEDGRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].outdegree += 1;
				}
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].indegree += 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].outdegree += 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].indegree += 1;
				}
			}
			else if (graphdirectiontype == REVERSE_DIRECTED_GRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].indegree += 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].outdegree += 1;
				}
			}
			else
			{
				LOG(ERROR) << "Vertices::UpdateVertexProperty : error1"  ;
			}
		}
		else if (edge_update_cmd == DELETEEDGE)
		{
			if (graphdirectiontype == DIRECTEDGRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].outdegree -= 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].indegree -= 1;
				}
			}
			else if (graphdirectiontype == UNDIRECTEDGRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].outdegree -= 1;
				}
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].indegree -= 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].outdegree -= 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].indegree -= 1;
				}
			}
			else if (graphdirectiontype == REVERSE_DIRECTED_GRAPH)
			{
				if (xvtx_id < num_vertices_)
				{
					vertex_properties_[xvtx_id].indegree -= 1;
				}
				if (xadjvtx_id < num_vertices_)
				{
					vertex_properties_[xadjvtx_id].outdegree -= 1;
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
} // namespace gt