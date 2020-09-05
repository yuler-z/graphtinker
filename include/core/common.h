#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <glog/logging.h>
using std::vector;

// #define EN_CAL    // Coarse Adjacency List
// #define EN_DCI //delete and crumple in 

#define WORK_BLOCK_HEIGHT 4 
#define EDGE_PROPERTY_NUM 1
#define BATCH_SIZE 1048576

#define DIRECTEDGRAPH 0
#define UNDIRECTEDGRAPH 1
#define REVERSE_DIRECTED_GRAPH 2

#define ZERO 0
#define NA 999999999 //not applicable
#define NAv 555555555 //not available
#define NULLL 888
#define NONE 666
#define FIRST 111

// insert_report.exittype
#define LOAD_INTO_EMPTY_BUCKET 0
#define FOUNDANDUPDATEDITSELF 1
#define PASSEDTHROUGH 2
#define PASSEDTHROUGHANDSWAPPEDATSOMEPOINT 3

#define SOMELARGENO 100000000

#define VALID 5
#define INVALID 6
#define METAVALID 7
#define DELETED 8

#define NO 0
#define YES 1

#define OFF 0
#define ON 1

// module unit mode 
#define FIND_MODE 0
#define INSERT_MODE 1

// verdict after inference
#define CONTINUE_FROM_FIRST_GENERATION 0
#define CONTINUE_IN_LOWER_GENERATION 1
#define CONTINUE_IN_CURRENT_GENERATION 2
#define QUIT_TO_NEXT_EDGE 3

#define LLEDGEBLOCKSIZE 512
#define LVACOARSENESSWIDTH 2048 //should be a power of 2

#define DELETEEDGE 0
#define INSERTEDGE 1

#define SELF 1
#define OTHER 2

typedef unsigned int vertexid_t;
typedef unsigned int edgeid_t;
typedef unsigned int bucket_t;
typedef unsigned int edge_type_t;
typedef unsigned int edge_weight_t;
typedef unsigned int flag_t;
typedef double vertexdata_t;
typedef unsigned int clusterptr_t;
typedef unsigned int id_t;

/** sv_ptr : pointer to a supervertex
	NB: many subblocks can (and should) have the same sv_ptr. means they all are 1st borns in a descendancy tree
*/
typedef struct{
	char key[10];
	char value[10];
}edge_property_t;


typedef struct {
	clusterptr_t data;
	flag_t flag;
	uint sv_ptr;
} clusterinfo_t;

typedef struct {
	flag_t flag; 
} edgeinfo_t;

// edge structure in edge block
typedef struct {
	vertexid_t adjvtx_id;
	bucket_t initial_bucket;
	edge_type_t type;
	edge_weight_t weight;
	edge_property_t properties[EDGE_PROPERTY_NUM];
    flag_t flag; // VALID, DELETED
	#ifdef EN_CAL
	vertexid_t cal_localbaseaddrptr;
	vertexid_t cal_localaddrptr;
	#endif
} edge_tt;

typedef struct {
	edgeinfo_t edgeinfo;
	clusterinfo_t clusterinfo;
	edge_tt edges[WORK_BLOCK_HEIGHT];
} workblock_t;

// edge in CAL
typedef struct {
	vertexid_t vtx_id;
	vertexid_t adjvtx_id;
	edge_type_t type;
	edge_weight_t weight;
	edge_property_t properties[EDGE_PROPERTY_NUM];
	flag_t flag;
	#ifdef EN_CAL
	int heba_hvtx_id;
	int heba_workblockid;
	int heba_loffset;
	int which_gen_is_the_main_copy_located;
	#endif
} edge_t;


//------------------unit flow----------------------
typedef struct {
	uint mode;
} module_unit_cmd_t;

typedef struct {
	uint rolledover;  //Y/N // traversal info
	uint clustered; //Y/N // cluster info
    int cptr;	
	edge_type_t type;
	vertexid_t adjvtx_id; // edge info
	edge_weight_t weight;
	#ifdef EN_CAL
	vertexid_t cal_localbaseaddrptr;
	vertexid_t cal_localaddrptr;
	#endif	
} module_params_t;

typedef struct {
	uint load;
} load_unit_cmd_t;

typedef struct {	
	vertexid_t adjvtx_id; // edge info
	bucket_t initial_bucket;
	edge_type_t type;
	edge_weight_t weight;	
	bool is_start_blk; // additional info
} find_params_t;

typedef struct {
	bucket_t local_offset;
	bool is_found;
	bool is_deleted;
	bool is_reach_max_prob_length;
	bool is_empty;
} find_report_t;

typedef struct {
	vertexid_t adjvtx_id; // edge info
	bucket_t initial_bucket;
	edge_type_t type;
	edge_weight_t weight;
	bool is_start_blk; // additional info	
} insert_params_t;

typedef struct {
	uint exittype;
	bucket_t validbuckets_incr;
	bucket_t overflowbkt;
	bucket_t lastbktloadedinto;
} insert_report_t;

typedef struct {
	uint writeback;
	uint addr;
	uint markasclustered;
	uint subblockid;
	uint workblockid;
} writeback_unit_cmd_t;

typedef struct {
	uint verdict;
} interval_unit_cmd_t;

typedef struct {
	bool is_search_stop;
	bool is_search_success;
} search_report_t;

typedef struct {
	bucket_t top;
	bucket_t bottom;
} margin_t;
/// trackers
typedef struct {
	uint mark;
} tracker_t;

typedef struct {
	uint marker;
} markertracker_t;

// scatter-gather hash
typedef struct {
	vertexid_t globalvid;
	vertexid_t localvid;
	flag_t flag;
	flag_t lflag;
} vertex_translator_t;

// vertex property array
typedef struct {
	uint indegree;
	uint outdegree;
	vertexdata_t data;
	flag_t flag;
} vertex_property_t;

/** 
- except stated otherwise, when used, the array of this struct is indexed by the *** raw local vertex ids ***  
- hvtx_id is a hashed vertex id value (calculated from the raw local vertex id) AND (w.r.t the given cluster!) ...
- ... this is done like this because each cluster exist as its own independent edge_tblockArray ...
- ... so the hvtx_id is just like that used in GT
- ... therefore, multiple entries in the *vertexlink_t can have the same hvtx_id, but these entry's cluster id (cid) must be all different
- status (VALID/INVALID) indicates whether or not this struct entry is valid and points to an edge cluster
- vprop is the property of the vertexlink */
typedef struct {
	vertexid_t hvtx_id; 
	uint cid;
	flag_t status;
	vertex_property_t vprop;
} vertexlink_t;

/** 
- vertexid iteratior type : used by powergraph in GTC
- a cluster of edges is retiieved
- says what vertices own these edges, and the offsets of their ownerships
uint offset; // offset from beginning of list
uint size; // length of vertex's edges
uint lvid; // vertex value */
typedef struct {
	uint offset; 
	uint size; 
	uint lvid; 
} vid_it_t; 

//----------------------Enable Delete and crumple in--------------------
#ifdef EN_CAL
// cal commands
#define NOCMD 0
#define INSERTCMD 5 
#define UPDATECMD 6
#define DELETECMD 7
#define DELETEANDCRUMPLEINCMD 8
#define UPDATEEDGEPTRSCMD 9

typedef struct {
	uint verdict;
} cal_unit_cmd_t;

typedef struct {
	uint edgecount;
	uint nextcptr;
	uint currcptr;
	uint prevcptr;
} cal_edgeblock_metadata_t;

typedef struct {
	edge_t cal_edgeblock[LLEDGEBLOCKSIZE];
	cal_edgeblock_metadata_t metadata;
} cal_edgeblock_t;

typedef struct {
	uint ptraddr;
} cal_edgeblock_tracker_t;

typedef struct {
	uint baseaddr;
	uint lastlocalbaseaddr;
	uint lastlocaladdr;
	uint totaledgecount;
	uint flag;
} cal_logical_vertex_entity_t;
#endif

//----------------------Enable Delete and crumple in------------------
#ifdef EN_DCI

// dci (deleteandcrumplein commands)
#define DCI_NOCMD 5
#define DCI_JUSTQUITCMD 6
#define DCI_CRUMPLEINCMD 7 

typedef struct {
	uint gen_of_parent;
	vertexid_t vtx_id; //***^
	uint subblockid;
	flag_t flag;
} edgeblock_parentinfo_t;

/** used in the crumpling-in deletion functionality
hvtx_id specifies an index to an edgeblock in the edge_tblockArray */
typedef struct {
	vector<uint> hvtx_ids;
	uint geni_ofparentsubblock;
} supervertex_t;

typedef struct {
	uint verdict;
} dci_cmd_t;
#endif

#endif

