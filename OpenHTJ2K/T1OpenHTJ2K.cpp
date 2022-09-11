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
#include "coding_units.hpp"
#include "ht_block_decoding.hpp"
#include "ht_block_encoding.hpp"
#include "T1OpenHTJ2K.h"
#include "grk_includes.h"

const uint8_t grk_cblk_dec_compressed_data_pad_ht = 8U;

namespace openhtj2k
{
T1OpenHTJ2K::T1OpenHTJ2K(bool isCompressor, [[maybe_unused]] grk::TileCodingParams* tcp,
						 uint32_t maxCblkW, uint32_t maxCblkH)
	: coded_data_size(isCompressor ? 0 : (uint32_t)(maxCblkW * maxCblkH * sizeof(int32_t))),
	  coded_data(isCompressor ? nullptr : new uint8_t[coded_data_size]),
	  unencoded_data_size(maxCblkW * maxCblkH), unencoded_data(new int32_t[unencoded_data_size])
{}
T1OpenHTJ2K::~T1OpenHTJ2K()
{
	delete[] coded_data;
	delete[] unencoded_data;
}
void T1OpenHTJ2K::preCompress([[maybe_unused]] grk::CompressBlockExec* block,
							  [[maybe_unused]] grk::Tile* tile)
{
	auto cblk = block->cblk;
	uint16_t w = (uint16_t)cblk->width();
	uint16_t h = (uint16_t)cblk->height();
	uint32_t tile_width =
		(tile->comps + block->compno)->getWindow()->getResWindowBufferHighestStride();
	auto tileLineAdvance = (int32_t)(tile_width - w);
	uint32_t cblk_index = 0U;

	// convert to sign-magnitude
	if((int8_t)block->qmfbid == 1)
	{
		auto tiledp = block->tiledp;
		for(auto j = 0U; j < h; ++j)
		{
			for(auto i = 0U; i < w; ++i)
			{
				unencoded_data[cblk_index] = (int32_t)*tiledp++;
				cblk_index++;
			}
			tiledp += tileLineAdvance;
		}
	}
	else
	{
		auto tiledp = block->tiledp;
		for(auto j = 0U; j < h; ++j)
		{
			for(auto i = 0U; i < w; ++i)
			{
				unencoded_data[cblk_index] = (int32_t)((float)*tiledp++ * block->inv_step_ht);
				cblk_index++;
			}
			tiledp += tileLineAdvance;
		}
	}
}
bool T1OpenHTJ2K::compress(grk::CompressBlockExec* block)
{
	preCompress(block, block->tile);
	auto cblk = block->cblk;
	uint32_t idx;
	uint16_t numlayers = 1;
	uint8_t codelbock_style = (uint8_t)block->cblk_sty;
	const element_siz p0;
	const element_siz p1;
	const element_siz s(cblk->width(), cblk->height());
	auto j2k_block =
		new j2k_codeblock(idx, block->bandOrientation, 0, 0, 0, 0, cblk->width(), /*unencoded_data,*/
						  (uint32_t*)unencoded_data, 0, numlayers, codelbock_style, p0, p1, s);
	auto len = htj2k_cleanup_encode(j2k_block, 0);
	cblk->numPassesTotal = 1;
	cblk->passes[0].len = (uint16_t)len;
	cblk->passes[0].rate = (uint16_t)len;
	cblk->numbps = 1;
	assert(cblk->paddedCompressedStream);
	memcpy(cblk->paddedCompressedStream, j2k_block->get_compressed_data(), (size_t)len);
	delete j2k_block;

	return true;
}

bool T1OpenHTJ2K::decompress(grk::DecompressBlockExec* block)
{
	auto cblk = block->cblk;
	if(!cblk->area())
		return true;
	if(!cblk->seg_buffers.empty())
	{
		size_t total_seg_len = cblk->getSegBuffersLen();
		if(coded_data_size < total_seg_len)
		{
			delete[] coded_data;
			coded_data = new uint8_t[total_seg_len];
			coded_data_size = (uint32_t)total_seg_len;
		}
		size_t offset = 0;
		for(auto& b : cblk->seg_buffers)
		{
			memcpy(coded_data + offset, b->buf, b->len);
			offset += b->len;
		}

		size_t num_passes = 0;
		for(uint32_t i = 0; i < cblk->getNumSegments(); ++i)
		{
			auto sgrk = cblk->getSegment(i);
			num_passes += sgrk->numpasses;
		}

		if(num_passes && offset)
		{
			auto cblk = block->cblk;
			uint32_t idx;
			uint16_t numlayers = 1;
			uint8_t codelbock_style = (uint8_t)block->cblk_sty;
			const element_siz p0;
			const element_siz p1;
			const element_siz s(cblk->width(), cblk->height());
			auto j2k_block =
				new j2k_codeblock(idx, block->bandOrientation, (uint8_t)(block->k_msbs + 1U), block->R_b,
								  block->qmfbid, block->stepsize, cblk->width(), /*unencoded_data,*/
								  (uint32_t*)unencoded_data, 0, numlayers, codelbock_style, p0, p1, s);
            j2k_block->num_passes = static_cast<uint8_t>(num_passes);
            //j2k_block->layer_passes[0] = static_cast<uint8_t>(j2k_block->layer_passes[0]);
            j2k_block->num_ZBP = static_cast<uint8_t>(block->k_msbs);
            j2k_block->length = static_cast<unsigned int>(offset);
            j2k_block->pass_length[0] = static_cast<unsigned int>(offset);
            j2k_block->pass_length[1] = 0;
            j2k_block->pass_length[2] = 0;
            j2k_block->set_compressed_data(coded_data, static_cast<uint16_t>(offset));

            int32_t Lcup = static_cast<int32_t>(j2k_block->pass_length[0]);
            uint8_t *Dcup = j2k_block->get_compressed_data();
            const int32_t Scup = static_cast<int32_t>((Dcup[Lcup - 1] << 4) + (Dcup[Lcup - 2] & 0x0F));
            Dcup[Lcup - 1] = 0xFF;
            Dcup[Lcup - 2] |= 0x0F;
            const int32_t Pcup = static_cast<int32_t>(Lcup - Scup);

            ht_cleanup_decode(j2k_block, static_cast<uint8_t>(30 - (block->k_msbs)), Lcup, 0, 0);
            delete j2k_block;
		}
		else
		{
			memset(unencoded_data, 0, (uint16_t)cblk->width() * cblk->height() * sizeof(int32_t));
		}
	}

	block->tilec->postProcessHT(unencoded_data, block, (uint16_t)cblk->width());
	return true;
}
} // namespace openhtj2k
