// Copyright (c) 2019 - 2021, Osamu Watanabe
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "coding_units.hpp"


/********************************************************************************
 * j2k_codeblock
 *******************************************************************************/

j2k_codeblock::j2k_codeblock(const uint32_t &idx, uint8_t orientation, uint8_t M_b, uint8_t R_b,
                             uint8_t transformation, float stepsize, uint32_t band_stride, uint32_t *ibuf,
                             /*float *fbuf,*/ uint32_t offset, const uint16_t &numlayers,
                             const uint8_t &codeblock_style, const element_siz &p0, const element_siz &p1,
                             const element_siz &s)
    : j2k_region(p0, p1),
      // public
      size(s),
      // private
      compressed_data(nullptr),
      current_address(nullptr),
      band(orientation),
      M_b(M_b),
      index(idx),
      //  public
      i_samples(ibuf + offset),
      band_stride(band_stride),
      R_b(R_b),
      transformation(transformation),
      stepsize(stepsize),
      num_layers(numlayers),
      length(0),
      Cmodes(codeblock_style),
      num_passes(0),
      num_ZBP(0),
      fast_skip_passes(0),
      Lblock(0),
      already_included(false),
      refsegment(false) {
  const uint32_t QWx2 = round_up(size.x, 8U);  // TODO: needs padding?
  const uint32_t QHx2 = round_up(size.y, 8U);  // TODO: needs padding?
  blksampl_stride = QWx2;
  blkstate_stride = QWx2 + 2;
  block_states    = MAKE_UNIQUE<uint8_t[]>(static_cast<size_t>(QWx2 + 2) * (QHx2 + 2));
  memset(block_states.get(), 0, static_cast<size_t>(QWx2 + 2) * (QHx2 + 2));
  sample_buf = MAKE_UNIQUE<int32_t[]>(static_cast<size_t>(QWx2 * QHx2));
  memset(sample_buf.get(), 0, sizeof(int32_t) * QWx2 * QHx2);
  this->layer_start  = MAKE_UNIQUE<uint8_t[]>(num_layers);
  this->layer_passes = MAKE_UNIQUE<uint8_t[]>(num_layers);
  if ((Cmodes & 0x40) == 0) this->pass_length.reserve(109);
  this->pass_length = std::vector<uint32_t>(num_layers, 0);  // critical section
}

uint8_t j2k_codeblock::get_Mb() const { return this->M_b; }

uint8_t *j2k_codeblock::get_compressed_data() { return this->compressed_data.get(); }

void j2k_codeblock::set_compressed_data(uint8_t *const buf, const uint16_t bufsize, const uint16_t Lref) {
  if (this->compressed_data != nullptr) {
    if (!refsegment) {
      printf(
          "ERROR: illegal attempt to allocate codeblock's compressed data but the data is not "
          "null.\n");
    } else {
      // if we are here, this function has been called to copy Dref[]
      memcpy(this->current_address + this->pass_length[0], buf, bufsize);
      return;
    }
  }
  this->compressed_data = MAKE_UNIQUE<uint8_t[]>(static_cast<size_t>(bufsize + Lref * (refsegment)));
  memcpy(this->compressed_data.get(), buf, bufsize);
  this->current_address = this->compressed_data.get();
}
