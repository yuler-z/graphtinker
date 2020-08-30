#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include "common.h"
namespace graphtinker
{

	/*
	 *  sgh for vertex
	 */

	class Translator
	{

    private:
		vertexid_t vertex_range_;
		vertex_translator_t* vertex_translator_;
		tracker_t translator_tracker_;

	public:
		Translator();
		Translator(vertexid_t vertex_range);
		~Translator();

		vertexid_t GetLocalVid(vertexid_t globalvid);
		vertexid_t GetGlobalVid(vertexid_t localvid);
		vertexid_t get_localvid2(vertexid_t globalvid);
		void PrintItems();
		void PrintItems(uint n);
		
		// getter and setter
		const vertexid_t& vertex_range() const;
		const vertex_translator_t* vertex_translator() const;
		const tracker_t& translator_tracker() const;
		const uint& translator_tracker_mark() const;
		void translator_tracker(const tracker_t& tracker);
	};

}
#endif