#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <glog/logging.h>
using std::vector;

#define EN_CAL 0    // Coarse Adjacency List
// #define EN_CRUMPLE_IN //crumple in on delete 

#define WORK_BLOCK_HEIGHT 4 
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

#define LOADEDINTOEMPTYBUCKET 0
#define FOUNDANDUPDATEDITSELF 1
#define PASSEDTHROUGH 2
#define PASSEDTHROUGHANDSWAPPEDATSOMEPOINT 3

#define SOMESMALLNO 10
#define SOMELARGENO 100000000

#define VALID 5
#define INVALID 6
#define METAVALID 7
#define DELETED 8

#define SUCCESSFUL 1
#define UNSUCCESSFUL 0

#define NO 0
#define YES 1

#define OFF 0
#define ON 1

#define READ 5 
#define WRITE 6
#define NEITHER_READ_NOR_WRITE 7

//
#define FIND_MODE 0
#define INSERT_MODE 1

#define GRAPHGEN_TOTAL_VERTEX_COUNT 10
#define GRAPHGEN_NO_OF_DSTV_PER_SRCV 10

// verdict after inference
#define CONTINUE_FROM_FIRST_GENERATION 0
#define CONTINUE_IN_LOWER_GENERATION 1
#define CONTINUE_IN_CURRENT_GENERATION 2
#define QUIT_TO_NEXT_EDGE 3

#define LLEDGEBLOCKSIZE 512
#define LVACOARSENESSWIDTH 2048 //should be a power of 2

// ll commands
#define NOCMD 0
#define INSERTCMD 5 
#define UPDATECMD 6
#define DELETECMD 7
#define DELETEANDCRUMPLEINCMD 8
#define UPDATEEDGEPTRSCMD 9

// dci (deleteandcrumplein commands)
#define DCI_NOCMD 5
#define DCI_JUSTQUITCMD 6
#define DCI_CRUMPLEINCMD 7 

#define NO_OF_VERTEX_PARTITIONS  //8
#define MAX_INDEX_FINDABLE_IN_A_PARTITION 
#define SIZE_OF_EACH_VERTEX_PARTITION 
#define SIZE_OF_EACH_VERTEX_SUBPARTITION 

#define SRC_DST 5
#define DST_SRC 6

#define INEDGE 5
#define OUTEDGE 6
#define NOEDGE 7

#define INSERTEDGE 5
#define DELETEEDGE 6

#define HIGH_DEGREE_VERTEX 5
#define LOW_DEGREE_VERTEX 6

#define SELF 1
#define OTHER 2

typedef unsigned int vertexid_t;
typedef unsigned int edgeid_t;
typedef unsigned int bucket_t;
typedef unsigned int edgeweight_t;
typedef unsigned int flag_t;
typedef double vertexdata_t;
typedef unsigned int clusterptr_t;
typedef unsigned int id_t;

/// struct declarations


/** sv_ptr : pointer to a supervertex
	NB: many subblocks can (and should) have the same sv_ptr. means they all are 1st borns in a descendancy tree
*/
typedef struct {
	clusterptr_t data;
	flag_t flag;
	uint sv_ptr;
} clusterinfo_t;

typedef struct {
	flag_t flag; 
} edgeinfo_t;

typedef struct {
	vertexid_t adjvtx_id;
	bucket_t initialbucket;
	edgeweight_t edge_weight;
    flag_t flag; // VALID, DELETED
	#ifdef EN_CAL
	vertexid_t ll_localbaseaddrptr;
	vertexid_t ll_localaddrptr;
	#endif
} edge_tt;

typedef struct {
	edgeinfo_t edgeinfo;
	clusterinfo_t clusterinfo;
	edge_tt edges[WORK_BLOCK_HEIGHT];
} work_block_t;

typedef struct {
	uint gen_of_parent;
	vertexid_t vtx_id; //***^
	uint subblockid;
	flag_t flag;
} edgeblock_parentinfo_t;

typedef struct {
	vertexid_t vtx_id;
	vertexid_t adjvtx_id;
	edgeweight_t edgew;
	flag_t flag;
	#ifdef EN_CAL
	int heba_hvtx_id;
	int heba_workblockid;
	int heba_loffset;
	int which_gen_is_the_main_copy_located;
	#endif
} edge_t;

typedef struct {
	uint mode;
} module_unit_cmd_t;

typedef struct {
	uint rolledover;  //Y/N // traversal info
	uint clustered; //Y/N // cluster info
    int cptr;	
	vertexid_t adjvtx_id; // edge info
	edgeweight_t edge_weight;
	#ifdef EN_CAL
	vertexid_t ll_localbaseaddrptr_x;
	vertexid_t ll_localaddrptr_x;
	#endif	
} module_params_t;

typedef struct {
	uint load;
} load_unit_cmd_t;

typedef struct {
	uint verdict;
} cal_unit_cmd_t;

typedef struct {
	uint verdict;
} crumple_in_cmd_t;

typedef struct {
	vertexid_t adjvtx_id; // edge info
	bucket_t initial_bucket;
	edgeweight_t edge_weight;
	bool is_start_blk; // additional info	
} insert_params_t;

typedef struct {
	uint exittype;
	bucket_t validbuckets_incr;
	bucket_t overflowbkt;
	bucket_t lastbktloadedinto;
} insert_report_t;

typedef struct {	
	vertexid_t adjvtx_id; // edge info
	bucket_t initial_bucket;
	edgeweight_t edge_weight;	
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
	bucket_t top;
	bucket_t bottom;
} margin_t;

typedef struct {
	bool search_stop;
	bool search_success;
} search_report_t;

typedef struct {
	uint search;
	uint insert;
} encontrol_t;

/// trackers
typedef struct {
	uint mark;
} tracker_t;

typedef struct {
	uint marker;
} markertracker_t;

/// profiling

typedef struct {
	vertexid_t A;
	vertexid_t B;
} tuple_vid_t;

typedef struct {
	uint A;
	uint B;
} tuple_t;

typedef struct {
	int A;
	int B;
} tuple_int_t;

typedef struct {
	vertexid_t A;
	vertexid_t B;
	vertexid_t C;
} triple_vid_t; 

typedef struct {
	vertexid_t globalvid;
	vertexid_t localvid;
	flag_t flag;
	flag_t lflag;
} vertex_translator_t;

/// LL data structures
typedef struct {
	uint edgecount;
	uint nextcptr;
	uint currcptr;
	uint prevcptr;
} cal_edgeblock_metadata_t;

typedef struct {
	edge_t ll_edgeblock[LLEDGEBLOCKSIZE];
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

typedef struct {	
	uint indegree;
	uint outdegree;
	vertexdata_t data;
	flag_t flag;
} vertex_property_t;

/** 
- except stated otherwise, when used, the array of this struct is indexed by the *** raw local vertex ids ***  
- hvtx_id is a hashed vertex id value (calculated from the raw local vertex id) AND (w.r.t the given cluster!) ...
- ... this is done like this because each cluster exist as its own independent EdgeblockArray ...
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

/** used in the crumpling-in deletion functionality
hvtx_id specifies an index to an edgeblock in the EdgeblockArray */
typedef struct {
	vector<uint> hvtx_ids;
	uint geni_ofparentsubblock;
} supervertex_t;
#endif

