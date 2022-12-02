/*
 *    Copyright (C) 2016-2022 Grok Image Compression Inc.
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
#include "simd.h"
#include "ojph_mem.h"
#include "ojph_block_decoder.h"
#include "ojph_block_encoder.h"
#include "ojph_mem.h"
#include "T1OJPH.h"

#include "grk_includes.h"


/*void memset32(uint32_t *dest, uint32_t val, uint32_t count)
{
    while (count--)
        *dest++ = val;
}*/

const uint8_t grk_cblk_dec_compressed_data_pad_ht = 8U;

namespace ojph
{
T1OJPH::T1OJPH(bool isCompressor, [[maybe_unused]] grk::TileCodingParams* tcp, uint32_t maxCblkW,
			   uint32_t maxCblkH)
	: coded_data_size(isCompressor ? 0 : (uint32_t)(maxCblkW * maxCblkH * sizeof(int32_t))),
	  coded_data(isCompressor ? nullptr : new uint8_t[coded_data_size]),
	  unencoded_data_size(maxCblkW * maxCblkH), unencoded_data(new int32_t[unencoded_data_size]),
	  allocator(new mem_fixed_allocator), elastic_alloc(new mem_elastic_allocator(1048576))
{
	if(!isCompressor)
		memset(coded_data, 0, grk_cblk_dec_compressed_data_pad_ht);
}
T1OJPH::~T1OJPH()
{
	delete[] coded_data;
	delete[] unencoded_data;
	delete allocator;
	delete elastic_alloc;
}
void T1OJPH::preCompress([[maybe_unused]] grk::CompressBlockExec* block,
						 [[maybe_unused]] grk::Tile* tile)
{
	auto cblk = block->cblk;
	uint32_t w = cblk->width();
	uint32_t h = cblk->height();
	uint32_t tile_width =
		(tile->comps + block->compno)->getWindow()->getResWindowBufferHighestStride();
	auto tileLineAdvance = (int32_t)(tile_width - w);
	uint32_t cblk_index = 0;
	int32_t shift = (int32_t)(31U - (block->k_msbs + 1U));

	// convert to sign-magnitude
	if(block->qmfbid == 1)
	{
		auto tiledp = block->tiledp;
		for(auto j = 0; j < h; ++j)
		{
			for(auto i = 0; i < w; ++i)
			{
				int32_t temp = *tiledp++;
				int32_t val = temp >= 0 ? temp : -temp;
				int32_t sign = (int32_t)((temp >= 0) ? 0 : 0x80000000);
				int32_t res = sign | (val << shift);
				unencoded_data[cblk_index] = res;
				cblk_index++;
			}
			tiledp += tileLineAdvance;
		}
	}
	else
	{
		auto tiledp = block->tiledp;
		for(auto j = 0; j < h; ++j)
		{
			for(auto i = 0; i < w; ++i)
			{
				int32_t t = *tiledp++ * (int32_t)(block->inv_step_ht) * (1 << shift);
				int32_t val = t >= 0 ? t : -t;
				int32_t sign = (int32_t)((t >= 0) ? 0 : 0x80000000);
				int32_t res = sign | val;
				unencoded_data[cblk_index] = res;
				cblk_index++;
			}
			tiledp += tileLineAdvance;
		}
	}
}
bool T1OJPH::compress(grk::CompressBlockExec* block)
{
	preCompress(block, block->tile);

	coded_lists* next_coded = nullptr;
	auto cblk = block->cblk;
	cblk->numbps = 0;
	// optimization below was causing errors in compressing
	// if (maximum >= (uint32_t)1<<(31U - (block->k_msbs + 1U)))
	uint32_t w = cblk->width();
	uint32_t h = cblk->height();

	uint32_t pass_length[2] = {0, 0};
	// Encoder OJPH 0.9.1 works with numpasses 1. Converter doesn't include std::jthread C++20.
	ojph::local::ojph_encode_codeblock((uint32_t*)unencoded_data, (uint32_t)(block->k_msbs), 1, w, h, w,
									   pass_length, elastic_alloc, next_coded);

	cblk->numPassesTotal = 1;
	cblk->passes[0].len = (uint32_t)pass_length[0];
	cblk->passes[0].rate = (uint32_t)pass_length[0];
	cblk->numbps = 1;
	assert(cblk->paddedCompressedStream);
	memcpy(cblk->paddedCompressedStream, next_coded->buf, (size_t)pass_length[0]);

	return true;
}
bool T1OJPH::decompress(grk::DecompressBlockExec* block)
{
	auto cblk = block->cblk;
	if(!cblk->area())
		return true;
	//uint16_t stride = (uint16_t)cblk->width();
	if(!cblk->seg_buffers.empty())
	{
		size_t total_seg_len = 2 * grk_cblk_dec_compressed_data_pad_ht + cblk->getSegBuffersLen();
		if(coded_data_size < (uint32_t)total_seg_len)
		{
			delete[] coded_data;
			coded_data = new uint8_t[total_seg_len];
			coded_data_size = (uint32_t)total_seg_len;
			memset(coded_data, 0, grk_cblk_dec_compressed_data_pad_ht);
		}
		memset(coded_data + grk_cblk_dec_compressed_data_pad_ht + (uint8_t)cblk->getSegBuffersLen(), 0,
			   grk_cblk_dec_compressed_data_pad_ht);
		uint8_t* actual_coded_data = coded_data + grk_cblk_dec_compressed_data_pad_ht;
		size_t offset = 0;
		for(auto& b : cblk->seg_buffers)
		{
			memcpy(actual_coded_data + offset, b->buf, b->len);
			offset += b->len;
		}

		size_t num_passes = 0;
		for(uint32_t i = 0; i < cblk->getNumSegments(); ++i)
		{
			auto sgrk = cblk->getSegment(i);
			num_passes += (uint8_t)sgrk->numpasses;
		}

		bool rc = false;
		if(num_passes && offset)
		{
		    // Decoder OJPH 0.9.1 doesn't work with numpasses 1.
			rc = ojph::local::ojph_decode_codeblock(
				actual_coded_data, (uint32_t*)unencoded_data, (uint32_t)(block->k_msbs), (uint32_t)num_passes,
				(uint32_t)offset, 0, cblk->width(), cblk->height(), cblk->width(), false);
        }
		else
		{
			memset(unencoded_data, 0, cblk->width() * cblk->height() * sizeof(int32_t));
		}
		if(!rc)
		{
			grk::GRK_ERROR("Error in HT block coder");
			return false;
		}
	}

	block->tilec->postProcessHT(unencoded_data, block, cblk->width());

	return true;
}
} // namespace ojph
