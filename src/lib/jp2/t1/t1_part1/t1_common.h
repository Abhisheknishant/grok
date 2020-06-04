/*
 *    Copyright (C) 2016-2020 Grok Image Compression Inc.
 *
 *    This source code is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This source code is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#pragma once

#include "grok.h"
#include <cstdint>

#ifndef INLINE
#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif /* defined(<Compiler>) */
#endif /* INLINE */

/////////////////
// buffer padding

// decode
/**< Space for a fake FFFF marker */
const uint8_t grk_cblk_dec_compressed_data_pad_right = 2;

// encode
const uint8_t grk_cblk_enc_compressed_data_pad_left = 2;
////////////////////////////////////////////////////////

#include <math.h>
#include <assert.h>
#include <string.h>
#include <t1_common.h>
#include "grok_malloc.h"
#include "mqc.h"

namespace grk {

#define T1_NMSEDEC_BITS 7
#define T1_NMSEDEC_FRACBITS (T1_NMSEDEC_BITS-1)

typedef struct pass {
    uint32_t rate;
    double distortiondec;
    uint32_t len;
    bool term;
} pass_t;

typedef struct cblk_enc {
    uint8_t* data;
    uint32_t data_size;
    pass_t* passes;
    uint32_t x0, y0, x1, y1;
    uint32_t numbps;
    uint32_t totalpasses;
} cblk_enc_t;

typedef struct seg_data_chunk {
    uint8_t * data;
    uint32_t len;
} seg_data_chunk_t;

typedef struct seg {
    uint32_t len;
    uint32_t numpasses;
    uint32_t real_num_passes;
    uint32_t maxpasses;
} seg_t;

typedef struct cblk_dec {
    seg_t* segs;
    seg_data_chunk_t* chunks;
    uint32_t x0, y0, x1, y1;
    uint32_t numbps;
    uint32_t numsegs;
    uint32_t real_num_segs;
    uint32_t numchunksalloc;
} cblk_dec_t;

}

#include "t1.h"
