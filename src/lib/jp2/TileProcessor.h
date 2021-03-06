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
 *
 *    This source code incorporates work covered by the following copyright and
 *    permission notice:
 *
 * The copyright in this software is being made available under the 2-clauses
 * BSD License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2008, 2011-2012, Centre National d'Etudes Spatiales (CNES), FR
 * Copyright (c) 2012, CS Systemes d'Information, France
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include "testing.h"
#include <vector>

namespace grk {

// code segment (code block can be encoded into multiple segments)
struct grk_seg {
	grk_seg();

	void clear();
	uint32_t dataindex;		      // segment data offset in contiguous memory block
	uint32_t numpasses;		    	// number of passes in segment
	uint32_t len;               // total length of segment
	uint32_t maxpasses;			  	// maximum number of passes in segment
	uint32_t numPassesInPacket;	  // number of passes contributed by current packet
	uint32_t numBytesInPacket;      // number of bytes contributed by current packet
};

struct grk_packet_length_info {
	grk_packet_length_info(uint32_t mylength, uint32_t bits);
	grk_packet_length_info();
	bool operator==(grk_packet_length_info &rhs) const;
	uint32_t len;
	uint32_t len_bits;
};

// encoding/decoding pass
struct grk_pass {
	grk_pass();
	uint32_t rate;
	double distortiondec;
	uint32_t len;
	uint8_t term;
	uint16_t slope;  //ln(slope) in 8.8 fixed point
};

//quality layer
struct grk_layer {
	grk_layer();
	uint32_t numpasses; /* Number of passes in the layer */
	uint32_t len; /* len of information */
	double disto; /* add for index (Cfr. Marcela) */
	uint8_t *data; /* data buffer (points to code block data) */
};

struct grk_cblk {
    grk_cblk(const grk_cblk &rhs);
    grk_cblk();
    virtual ~grk_cblk(){}
	uint32_t x0, y0, x1, y1; /* dimension of the code block : left upper corner (x0, y0) right low corner (x1,y1) */
	uint8_t *compressedData; /* data buffer*/
	uint32_t compressedDataSize; /* size of allocated data buffer */
	bool owns_data;	// true if code block manages data buffer, otherwise false
	uint32_t numbps;
	uint32_t numlenbits;
	uint32_t numPassesInPacket; /* number of passes encoded in current packet */
#ifdef DEBUG_LOSSLESS_T2
	uint32_t included;
	std::vector<grk_packet_length_info>* packet_length_info;
#endif
};

// encoder code block
struct grk_cblk_enc : public grk_cblk {
	grk_cblk_enc();
	~grk_cblk_enc();
	bool alloc();
	bool alloc_data(size_t nominalBlockSize);
	void cleanup();
	uint8_t *paddedCompressedData; /* data buffer*/
	grk_layer *layers;
	grk_pass *passes;
	uint32_t numPassesInPreviousPackets; /* number of passes in previous packetss */
	uint32_t numPassesTotal; /* total number of passes in all layers */
	uint32_t *contextStream;
};

//decoder code block
struct grk_cblk_dec: public grk_cblk {
	grk_cblk_dec();
	~grk_cblk_dec();
	grk_cblk_dec(const grk_cblk_dec &rhs);
	void init();
	bool alloc();
	void cleanup();
	grk_vec seg_buffers;
	grk_seg *segs; /* information on segments */
	uint32_t numSegments; /* number of segment in block*/
	uint32_t numSegmentsAllocated; // number of segments allocated for segs array

};

// precinct
struct grk_precinct {
	grk_precinct();
	void initTagTrees();
	void deleteTagTrees();
	void cleanupEncodeBlocks() ;
	void cleanupDecodeBlocks();

	uint32_t x0, y0, x1, y1; /* dimension of the precinct : left upper corner (x0, y0) right low corner (x1,y1) */
	uint32_t cw, ch; /* number of precinct in width and height */
	union { /* code-blocks information */
		grk_cblk_enc *enc;
		grk_cblk_dec *dec;
		void *blocks;
	} cblks;
	uint64_t block_size; /* size taken by cblks (in bytes) */
	TagTree *incltree; /* inclusion tree */
	TagTree *imsbtree; /* IMSB tree */
};

// band
struct grk_band {
	grk_band();
	grk_band(const grk_band &rhs);

	bool isEmpty() ;
	/* dimension of the subband : left upper corner (x0, y0) right low corner (x1,y1) */
	uint32_t x0, y0, x1, y1;
	// 0 for first band of lowest resolution, otherwise equal to 1,2 or 3
	uint8_t bandno;
	grk_precinct *precincts; /* precinct information */
	uint64_t numPrecincts;
	size_t numAllocatedPrecincts;
	uint32_t numbps;
	float stepsize;
	// inverse step size in 13 bit fixed point
	uint32_t inv_step;

};

// resolution
struct grk_resolution {
	grk_resolution();

	/* dimension of the resolution level in tile coordinates */
	uint32_t x0, y0, x1, y1;
	uint32_t pw, ph;
	uint32_t numbands; /* number sub-band for the resolution level */
	grk_band bands[3]; /* subband information */

    /* dimension of the resolution limited to window of interest.
     *  Only valid if tcd->whole_tile_decoding is set */
	uint32_t win_x0;
	uint32_t win_y0;
	uint32_t win_x1;
	uint32_t win_y1;

};

struct TileComponent;

// tile
struct grk_tile {
	uint32_t x0, y0, x1, y1; /* dimension of the tile : left upper corner (x0, y0) right low corner (x1,y1) */
	uint32_t numcomps; /* number of components in tile */
	TileComponent *comps; /* Components information */
	uint64_t numpix;
	double distotile;
	double distolayer[100];
	uint64_t packno; /* packet number */
};

struct PacketTracker{
	PacketTracker();
	~PacketTracker();
	void init(uint32_t numcomps, uint32_t numres, uint64_t numprec, uint32_t numlayers);
	void clear(void);
	void packet_encoded(uint32_t comps, uint32_t res, uint64_t prec, uint32_t layer);
	bool is_packet_encoded(uint32_t comps, uint32_t res, uint64_t prec, uint32_t layer);
private:
	uint8_t *bits;

	uint32_t m_numcomps;
	uint32_t m_numres;
	uint64_t m_numprec;
	uint32_t m_numlayers;

	uint64_t get_buffer_len(uint32_t numcomps, uint32_t numres, uint64_t numprec, uint32_t numlayers);
	uint64_t index(uint32_t comps, uint32_t res, uint64_t prec, uint32_t layer);
};

/**
 Tile coder/decoder
 */
struct TileProcessor {

	TileProcessor(bool isDecoder) ;
	~TileProcessor();

	/**
	 * Initialize the tile coder
	 * @param	p_image		image
	 * @param	p_cp		coding parameters
	 *
	 * @return true if the encoding values could be set (false otherwise).
	 */
	bool init(grk_image *p_image, CodingParams *p_cp);

	/**
	 * Allocates memory for decoding a specific tile.
	 *
	 * @param	output_image output image - stores the decompress region of interest
	 * @param	tile_no	the index of the tile received in sequence. This not necessarily lead to the
	 * tile at index tile_no.
	 *
	 * @return	true if the remaining data is sufficient.
	 */
	 bool init_tile(uint16_t tile_no,
			grk_image *output_image, bool isEncoder);

	/**
	 * Sets the given area to be decoded. This function should be called right after grk_read_header
	 * and before any tile header reading.
	 *
	 * @param	codeStream		JPEG 2000 code stream
	 * @param	p_image     FIXME DOC
	 * @param	start_x		the left position of the rectangle to decompress (in image coordinates).
	 * @param	start_y		the up position of the rectangle to decompress (in image coordinates).
	 * @param	end_x		the right position of the rectangle to decompress (in image coordinates).
	 * @param	end_y		the bottom position of the rectangle to decompress (in image coordinates).

	 *
	 * @return	true			if the area could be set.
	 */
	bool set_decompress_area(CodeStream *codeStream,
						grk_image *p_image,
						uint32_t start_x,
						uint32_t start_y,
						uint32_t end_x,
						uint32_t end_y);

	/**
	 * Compress a tile from a raw image into stream.
	 * @param	tile_no		Index of the tile to compress.
	 * @param	stream		stream
	 * @param	tile_bytes_written	number of bytes written to stream
	 * @param	p_cstr_info		Code stream information structure
	 * @return  true if the coding is successful.
	 */
	bool compress_tile_part(uint16_t tile_no, BufferedStream *stream,
			uint32_t *tile_bytes_written, grk_codestream_info  *p_cstr_info);

	/**
	 Decode a tile from a buffer
	 @param src_buf Source buffer
	 @param tileno Number that identifies one of the tiles to be decoded
	 @return true if successful
	 */
	bool decompress_tile(ChunkBuffer *src_buf, uint16_t tileno);

	/**
	 * Copies tile data from the system onto the given memory block.
	 */
	bool composite_tile(uint8_t *p_dest,
			uint64_t dest_length);


	uint64_t get_uncompressed_tile_size(bool reduced);

	/**
	 * Copies tile data from the given memory block onto the system.
	 */
	bool copy_image_data_to_tile(uint8_t *p_src, uint64_t src_length);


	bool needs_rate_control();

	bool copy_decompressed_tile_to_output_image(uint8_t *p_data,
			grk_image *p_output_image, bool clearOutputOnInit);

	void copy_image_to_tile();

	bool read_marker(BufferedStream *stream, uint16_t *val);

	/** index of the tile to decompress (used in get_tile);
	 *  !!! initialized to -1 !!! */
	int32_t m_tile_ind_to_dec;

	/** index of tile being currently coded/decoded */
	uint16_t m_current_tile_index;

	/** position of the tile part flag in progression order*/
	uint32_t tp_pos;

	/** tile part index, regardless of poc.
	 *  for each new poc, tp is reset to 0*/
	uint8_t m_current_poc_tile_part_index;

	/** index of tile part being currently coding, taking into account POC.
	 *  m_current_tile_part_index holds the total number of tile parts
	 *   while encoding the last tile part.*/
	uint8_t m_current_tile_part_index;

	/** TNsot correction : see issue 254 **/
	bool m_nb_tile_parts_correction_checked;
	bool m_nb_tile_parts_correction;

	uint32_t tile_part_data_length;

	/** Total number of tile parts of the current tile*/
	uint8_t cur_totnum_tp;
	/** Current packet iterator number */
	uint32_t cur_pino;
	/** info on image tile */
	grk_tile *tile;
	/** image header */
	grk_image *image;
	grk_plugin_tile *current_plugin_tile;

    /** Only valid for decoding. Whether the whole tile is decoded, or just the region in win_x0/win_y0/win_x1/win_y1 */
    bool   whole_tile_decoding;

	uint8_t *m_marker_scratch;
	uint16_t m_marker_scratch_size;

	PacketLengthMarkers *plt_markers;

	/** coding parameters */
	CodingParams *m_cp;

	PacketTracker m_packetTracker;
private:

	/** coding/decoding parameters common to all tiles */
	TileCodingParams *m_tcp;
	/** current encoded tile (not used for decompress) */
	uint16_t m_tileno;

	 bool t2_decode(uint16_t tile_no, ChunkBuffer *src_buf,
			uint64_t *p_data_read);

	 bool is_whole_tilecomp_decoding( uint32_t compno);

	 bool mct_decode();

	 bool dc_level_shift_decode();

	 bool dc_level_shift_encode();

	 bool mct_encode();

	 bool dwt_encode();

	 void t1_encode();

	 bool t2_encode(BufferedStream *stream,
			uint32_t *packet_bytes_written,
			 grk_codestream_info  *p_cstr_info);

	 bool rate_allocate( grk_codestream_info  *p_cstr_info);

	 bool layer_needs_rate_control(uint32_t layno);

	 bool make_single_lossless_layer();

	 void makelayer_final(uint32_t layno);

	 bool pcrd_bisect_simple(uint32_t *p_data_written);

	 void make_layer_simple(uint32_t layno, double thresh,
			bool final);

	 bool pcrd_bisect_feasible(uint32_t *p_data_written);

	 void makelayer_feasible(uint32_t layno, uint16_t thresh,
			bool final);

};

}
