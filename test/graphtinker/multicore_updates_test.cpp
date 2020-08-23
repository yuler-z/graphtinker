#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
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
#include "graphtinker/graphtinker.h"
#include "vertices/vertices.h"
#define ___use_omp___
using namespace std;

class multicore_updates_test
{
public:
	multicore_updates_test();
	~multicore_updates_test();

private:
};

multicore_updates_test::multicore_updates_test() {}
multicore_updates_test::~multicore_updates_test() {}

void insertions(Graphtinker &tinkerA, Graphtinker &tinkerB, vector<tuple_t> &src_dst_pairs)
{
	cout  
		 << "insertions started : src_dst_pairs size : " << src_dst_pairs.size()  ;
#ifdef ___use_omp___
#pragma omp parallel sections
	{
#pragma omp section
		{
			for (uint j = 0; j < src_dst_pairs.size(); j++)
			{
				tinkerA.InsertEdge((uint)src_dst_pairs[j].A, (uint)src_dst_pairs[j].B, 1);
			}
			cout << "number of edges in tinkerA : " << tinkerA.printv_edgecount() << " edges"  ;
		}
#pragma omp section
		{
			for (uint j = 0; j < src_dst_pairs.size(); j++)
			{
				tinkerB.InsertEdge((uint)src_dst_pairs[j].B, (uint)src_dst_pairs[j].A, 1);
			}
			cout << "number of edges in tinkerB : " << tinkerB.printv_edgecount() << " edges"  ;
		}
	}
#else
	tinkerA.create(SELF, SELF, SELF, 0, 524288, 524288, 8380000, DIRECTEDGRAPH, 8, 64);
	for (uint j = 0; j < src_dst_pairs.size(); j++)
	{
		tinkerA.InsertEdge((uint)src_dst_pairs[j].A, (uint)src_dst_pairs[j].B, 1);
	}
	cout << "number of edges in tinkerA : " << tinkerA.printv_edgecount() << " edges"  ;
	tinkerB.create(SELF, SELF, SELF, 0, 524288, 524288, 8380000, DIRECTEDGRAPH, 8, 64);
	for (uint j = 0; j < src_dst_pairs.size(); j++)
	{
		tinkerB.InsertEdge((uint)src_dst_pairs[j].B, (uint)src_dst_pairs[j].A, 1);
	}
	cout << "number of edges in tinkerB : " << tinkerB.printv_edgecount() << " edges"  ;
#endif
	return;
}

int main()
{
#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		cout << " hello world from thread : " << ID  ;
	}
	// return 0;

	Graphtinker tinkerA(SELF, SELF, SELF, 0, 524288, 524288, 8380000, DIRECTEDGRAPH, 8, 64);
	Graphtinker tinkerB(SELF, SELF, SELF, 0, 524288, 524288, 8380000, DIRECTEDGRAPH, 8, 64);

	float timelapse_clock = 0;
	float timelapse_tod = 0;

	struct timeval start1_tod, end1_tod;

	vector<tuple_t> src_dst_pairs;
	src_dst_pairs.clear();

	char path[] = "/net/zf15/oj2zf/datasets/rmat500k8m.edges";
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		cout << "error : cannot find the location of input file rmat500k8m.edges (test_updatesingleedge)"  ;
	}

	// insert edges into tinkers
	uint src = 0;
	uint dst = 0;
	uint ew = 0;
	for (uint i = 0; i < 1000000; i++)
	{
		if (feof(file))
		{
			cout << "finished reading from file (multicore_updates_test)"  ;
			break;
		}
		if (fscanf(file, "%i", &src) < 0)
		{
			cout << "fscan error (multicore_updates_test)"  ;
		}
		if (fscanf(file, "%i", &dst) < 0)
		{
			cout << "fscan error (multicore_updates_test)"  ;
		}

		tuple_t t;
		t.A = src;
		t.B = dst;
		src_dst_pairs.push_back(t);
	}

	gettimeofday(&start1_tod, NULL);
	std::clock_t start = std::clock();

	insertions(tinkerA, tinkerB, src_dst_pairs); ///

	timelapse_clock = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
	gettimeofday(&end1_tod, NULL);
	timelapse_tod = (float)((end1_tod.tv_sec * 1000000 + end1_tod.tv_usec) - (start1_tod.tv_sec * 1000000 + start1_tod.tv_usec)) / 1000;

	cout << "number of edges A : " << tinkerA.printv_edgecount() << " edges"  ;
	cout << "number of edges B : " << tinkerB.printv_edgecount() << " edges"  ;

	cout << "jurassicpark::run_jpasyn : time lapse (clock) : " << timelapse_clock << " ms"  ;
	cout << "jurassicpark::run_jpasyn : time lapse (tod) : " << timelapse_tod << " ms"  ;
	cout << "jurassicpark::run_jpasyn : threads ran : " << omp_get_num_threads() << " threads"  ;
	cout << "finished."  ;
	return 0;
}