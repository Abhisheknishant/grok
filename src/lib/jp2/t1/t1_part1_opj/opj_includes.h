#pragma once

#include "grok.h"
#include <stdbool.h>

#include "shared.h"
#include <math.h>
#include <assert.h>
#include <string.h>
#include "grok_malloc.h"
#include "opj_common.h"
#include "mqc.h"

#define T1_NMSEDEC_BITS 7
#define T1_NMSEDEC_FRACBITS (T1_NMSEDEC_BITS-1)

/* Not a C99 compiler */
#if defined(__GNUC__)
#define OPJ_RESTRICT __restrict__
#else
#define OPJ_RESTRICT /* restrict */
#endif



/* Type to use for bit-fields in internal headers */
typedef unsigned int OPJ_BITFIELD;


/**
FIXME DOC
*/
typedef struct opj_tcd_pass {
    uint32_t rate;
    double distortiondec;
    uint32_t len;
    OPJ_BITFIELD term : 1;
} opj_tcd_pass_t;

/**
FIXME DOC
*/
typedef struct opj_tcd_layer {
    uint32_t numpasses;       /* Number of passes in the layer */
    uint32_t len;             /* len of information */
    double disto;          /* add for index (Cfr. Marcela) */
    uint8_t *data;             /* data */
} opj_tcd_layer_t;

/**
FIXME DOC
*/
typedef struct opj_tcd_cblk_enc {
    uint8_t* data;               /* Data */
    opj_tcd_layer_t* layers;      /* layer information */
    opj_tcd_pass_t* passes;       /* information about the passes */
    int32_t x0, y0, x1,
              y1;     /* dimension of the code-blocks : left upper corner (x0, y0) right low corner (x1,y1) */
    uint32_t numbps;
    uint32_t numlenbits;
    uint32_t data_size;         /* Size of allocated data buffer */
    uint32_t
    numpasses;         /* number of pass already done for the code-blocks */
    uint32_t numpassesinlayers; /* number of passes in the layer */
    uint32_t totalpasses;       /* total number of passes */
} opj_tcd_cblk_enc_t;


/** Chunk of codestream data that is part of a code block */
typedef struct opj_tcd_seg_data_chunk {
    /* Point to tilepart buffer. We don't make a copy !
       So the tilepart buffer must be kept alive
       as long as we need to decode the codeblocks */
    uint8_t * data;
    uint32_t len;                 /* Usable length of data */
} opj_tcd_seg_data_chunk_t;

/** Segment of a code-block.
 * A segment represent a number of consecutive coding passes, without termination
 * of MQC or RAW between them. */
typedef struct opj_tcd_seg {
    uint32_t len;      /* Size of data related to this segment */
    /* Number of passes decoded. Including those that we skip */
    uint32_t numpasses;
    /* Number of passes actually to be decoded. To be used for code-block decoding */
    uint32_t real_num_passes;
    /* Maximum number of passes for this segment */
    uint32_t maxpasses;
} opj_tcd_seg_t;

/** Code-block for decoding */
typedef struct opj_tcd_cblk_dec {
    opj_tcd_seg_t* segs;            /* segments information */
    opj_tcd_seg_data_chunk_t* chunks; /* Array of chunks */
    /* position of the code-blocks : left upper corner (x0, y0) right low corner (x1,y1) */
    int32_t x0, y0, x1, y1;
    uint32_t numbps;
    /* number of segments, including those of packet we skip */
    uint32_t numsegs;
    /* number of segments, to be used for code block decoding */
    uint32_t real_num_segs;
    uint32_t m_current_max_segs;  /* allocated number of segs[] items */
    uint32_t numchunks;           /* Number of valid chunks items */
    uint32_t numchunksalloc;      /* Number of chunks item allocated */
    /* Decoded code-block. Only used for subtile decoding. Otherwise tilec->data is directly updated */
    int32_t* decoded_data;
} opj_tcd_cblk_dec_t;


#include "t1.h"
