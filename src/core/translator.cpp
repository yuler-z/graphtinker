#include <string.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "core/translator.h"
using namespace std;
namespace graphtinker
{
	Translator::Translator(vertexid_t vertex_range)
	{
		vertex_range_ = vertex_range;
		vertex_translator_ = new vertex_translator_t[vertex_range];
		for (uint i = 0; i < vertex_range; i++)
		{
			vertex_translator_[i].globalvid = 0;
			vertex_translator_[i].localvid = 0;
			vertex_translator_[i].flag = INVALID;
			vertex_translator_[i].lflag = INVALID;
		}
		translator_tracker_.mark = 0;
	}

	Translator::Translator() {}

	Translator::~Translator() {
		delete[] vertex_translator_;
		translator_tracker_.mark = 0;
	}

	vertexid_t Translator::GetLocalVid(vertexid_t globalvid)
	{
		if (globalvid > vertex_range_)
		{
			LOG(ERROR) << "translator::GetLocalVid : bug, out of range5. globalvid : " << globalvid << ", vertex_range : " << vertex_range_  ;
		}
		if (translator_tracker_.mark > vertex_range_)
		{
			LOG(ERROR) << "translator::GetLocalVid : bug, out of range6. globalvid : " << globalvid << ", vertex_range : " << vertex_range_  ;
		}
		if (vertex_translator_[globalvid].flag != VALID)
		{
			vertex_translator_[translator_tracker_.mark].globalvid = globalvid;
			vertex_translator_[translator_tracker_.mark].lflag = VALID;
			vertex_translator_[globalvid].localvid = translator_tracker_.mark;
			vertex_translator_[globalvid].flag = VALID;
			translator_tracker_.mark += 1;
		}
		return vertex_translator_[globalvid].localvid;
	}

	vertexid_t Translator::GetGlobalVid(vertexid_t localvid)
	{
		if (localvid > vertex_range_)
		{
			LOG(ERROR) << "translator::GetGlobalVid : bug. out of range7 (localvid > vertex_range). localvid : " << localvid << ", vertex_range : " << vertex_range_  ;
		}
		/// if(vertex_translator[localvid].lflag != VALID){ LOG(ERROR)<<"translator::GetGlobalVid : bug. out of range8 (lflag != VALID). localvid : "<<localvid<<", vertex_range : "<<vertex_range<<", vertex_translator["<<localvid<<"].globalvid : "<<vertex_translator[localvid].globalvid ; }
		return vertex_translator_[localvid].globalvid;
	}

	void Translator::PrintItems(){
		PrintItems(vertex_range_);
	}

	void Translator::PrintItems(uint n)
	{
		LOG(INFO) << "translator::print_first_n_items_of_vertex_translator : n : " << n  ;
		if (n > vertex_range_)
		{
			LOG(INFO) << "bug : n > vertex_range."  ;
			return;
		}

		for (uint i = 0; i < n; i++)
		{
			LOG(INFO) << i << ": [";
			LOG(INFO) << vertex_translator_[i].globalvid;
			LOG(INFO) << ", " << vertex_translator_[i].localvid;
			LOG(INFO) << ", " << vertex_translator_[i].flag;
			LOG(INFO) << ", " << vertex_translator_[i].lflag;
			LOG(INFO) << "]"  ;
		}
		return;
	}

	// getter and setter
	const vertexid_t& Translator::vertex_range() const{
		return vertex_range_;
	}
	const vertex_translator_t* Translator::vertex_translator() const{
		return vertex_translator_;

	}
	const tracker_t& Translator::translator_tracker() const{
		return translator_tracker_;
	}
	const uint& Translator::translator_tracker_mark() const{
		return translator_tracker_.mark;
	}
	void Translator::translator_tracker(const tracker_t& tracker){
		translator_tracker_ = tracker;
	}
} // namespace graphtinker