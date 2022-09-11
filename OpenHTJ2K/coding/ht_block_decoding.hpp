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

#pragma once

#include <cstdint>

#if __GNUC__ || __has_attribute(always_inline)
  #define FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
  #define FORCE_INLINE __forceinline
#else
  #define FORCE_INLINE inline
#endif

// LUT for UVLC decoding in initial line-pair
//   index (8bits) : [bit   7] u_off_1 (1bit)
//                   [bit   6] u_off_0 (1bit)
//                   [bit 5-0] LSB bits from VLC codeword
//   the index is incremented by 64 when both u_off_0 and u_off_1 are 0
//
//   output        : [bit 0-2] length of prefix (l_p) for quads 0 and 1
//                 : [bit 3-6] length of suffix (l_s) for quads 0 and 1
//                 : [bit 7-9] ength of suffix (l_s) for quads 0
constexpr uint16_t uvlc_dec_0[256 + 64] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x16ab,
    0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401,
    0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802,
    0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401,
    0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b,
    0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0xa02b, 0x2001,
    0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002,
    0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001,
    0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b,
    0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001,
    0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0x36ac, 0xa42c, 0xa82d,
    0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402, 0x56ac, 0x640c, 0x4804, 0x2402, 0x4c8c, 0x4403, 0x2803, 0x2402,
    0x36ac, 0xa42c, 0x680d, 0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402, 0x56ac, 0x640c, 0x4804, 0x2402, 0x4c8c,
    0x4403, 0x2803, 0x2402, 0x36ac, 0xa42c, 0xa82d, 0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402, 0x56ac, 0x640c,
    0x4804, 0x2402, 0x4c8c, 0x4403, 0x2803, 0x2402, 0x36ac, 0xa42c, 0x680d, 0x2402, 0x2c8c, 0x4403, 0x2803,
    0x2402, 0x56ac, 0x640c, 0x4804, 0x2402, 0x4c8c, 0x4403, 0x2803, 0x2402, 0xfed6, 0xec2c, 0xf02d, 0x6c02,
    0xf4b6, 0x8c03, 0x7003, 0x6c02, 0x7eac, 0xac0c, 0x9004, 0x6c02, 0x748c, 0x8c03, 0x7003, 0x6c02, 0x9ead,
    0xec2c, 0xb00d, 0x6c02, 0x948d, 0x8c03, 0x7003, 0x6c02, 0x7eac, 0xac0c, 0x9004, 0x6c02, 0x748c, 0x8c03,
    0x7003, 0x6c02, 0xbeb6, 0xec2c, 0xf02d, 0x6c02, 0xb496, 0x8c03, 0x7003, 0x6c02, 0x7eac, 0xac0c, 0x9004,
    0x6c02, 0x748c, 0x8c03, 0x7003, 0x6c02, 0x9ead, 0xec2c, 0xb00d, 0x6c02, 0x948d, 0x8c03, 0x7003, 0x6c02,
    0x7eac, 0xac0c, 0x9004, 0x6c02, 0x748c, 0x8c03, 0x7003, 0x6c02};

// LUT for UVLC decoding in non-initial line-pair
//   index (8bits) : [bit   7] u_off_1 (1bit)
//                   [bit   6] u_off_0 (1bit)
//                   [bit 5-0] LSB bits from VLC codeword
//
//   output        : [bit 0-2] length of prefix (l_p) for quads 0 and 1
//                 : [bit 3-6] length of suffix (l_s) for quads 0 and 1
//                 : [bit 7-9] ength of suffix (l_s) for quads 0
constexpr uint16_t uvlc_dec_1[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x16ab,
    0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401,
    0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802,
    0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401,
    0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b,
    0x0401, 0x0802, 0x0401, 0x16ab, 0x0401, 0x0802, 0x0401, 0x0c8b, 0x0401, 0x0802, 0x0401, 0xa02b, 0x2001,
    0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002,
    0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001,
    0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b,
    0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001,
    0x4002, 0x2001, 0xa02b, 0x2001, 0x4002, 0x2001, 0x600b, 0x2001, 0x4002, 0x2001, 0xb6d6, 0xa42c, 0xa82d,
    0x2402, 0xacb6, 0x4403, 0x2803, 0x2402, 0x36ac, 0x640c, 0x4804, 0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402,
    0x56ad, 0xa42c, 0x680d, 0x2402, 0x4c8d, 0x4403, 0x2803, 0x2402, 0x36ac, 0x640c, 0x4804, 0x2402, 0x2c8c,
    0x4403, 0x2803, 0x2402, 0x76b6, 0xa42c, 0xa82d, 0x2402, 0x6c96, 0x4403, 0x2803, 0x2402, 0x36ac, 0x640c,
    0x4804, 0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402, 0x56ad, 0xa42c, 0x680d, 0x2402, 0x4c8d, 0x4403, 0x2803,
    0x2402, 0x36ac, 0x640c, 0x4804, 0x2402, 0x2c8c, 0x4403, 0x2803, 0x2402};

/********************************************************************************
 * MEL_dec:
 *******************************************************************************/
// this class implementation is borrowed from OpenJPH
class MEL_dec {
 private:
  const uint8_t *buf;
  uint64_t tmp;
  int bits;
  int32_t length;
  bool unstuff;
  int MEL_k;

  int32_t num_runs;
  uint64_t runs;

 public:
  MEL_dec(const uint8_t *Dcup, int32_t Lcup, int32_t Scup)
      : buf(Dcup + Lcup - Scup),
        tmp(0),
        bits(0),
        length(Scup - 1),  // length is the length of MEL+VLC-1
        unstuff(false),
        MEL_k(0),
        num_runs(0),
        runs(0) {
    int num = 4 - static_cast<int>(reinterpret_cast<intptr_t>(buf) & 0x3);
    for (int32_t i = 0; i < num; ++i) {
      uint64_t d = (length > 0) ? *buf : 0xFF;  // if buffer is exhausted, set data to 0xFF
      if (length == 1) {
        d |= 0xF;  // if this is MEL+VLC+1, set LSBs to 0xF (see the spec)
      }
      buf += length-- > 0;  // increment if the end is not reached
      int d_bits = 8 - unstuff;
      tmp        = (tmp << d_bits) | d;
      bits += d_bits;
      unstuff = ((d & 0xFF) == 0xFF);
    }
    tmp <<= (64 - bits);
  }

  inline void read() {
    if (bits > 32) {  // there are enough bits in tmp, return without any reading
      return;
    }

    uint32_t val = 0xFFFFFFFF;  // feed in 0xFF if buffer is exhausted
    if (length > 4) {           // if there is data in the MEL segment
      val = *reinterpret_cast<uint32_t *>(const_cast<uint8_t *>(buf));  // read 32 bits from MEL data
      buf += 4;                                                         // advance pointer
      length -= 4;                                                      // reduce counter
    } else if (length > 0) {
      // 4 or less
      int i = 0;
      while (length > 1) {
        uint32_t v = *buf++;                // read one byte at a time
        uint32_t m = ~(0xFFU << i);         // mask of location
        val        = (val & m) | (v << i);  // put one byte in its correct location
        --length;
        i += 8;
      }
      // length equal to 1
      uint32_t v = *buf++;  // the one before the last is different
      v |= 0xF;             // MEL and VLC segments may be overlapped
      uint32_t m = ~(0xFFU << i);
      val        = (val & m) | (v << i);
      --length;
    } else {
      // error
    }

    // next we unstuff them before adding them to the buffer
    int bits_local =
        32 - unstuff;  // number of bits in val, subtract 1 if the previously read byte requires unstuffing

    // data is unstuffed and accumulated in t
    // bits_local has the number of bits in t
    uint32_t t        = val & 0xFF;
    bool unstuff_flag = ((val & 0xFF) == 0xFF);
    bits_local -= unstuff_flag;
    t = t << (8 - unstuff_flag);

    t |= (val >> 8) & 0xFF;
    unstuff_flag = (((val >> 8) & 0xFF) == 0xFF);
    bits_local -= unstuff_flag;
    t = t << (8 - unstuff_flag);

    t |= (val >> 16) & 0xFF;
    unstuff_flag = (((val >> 16) & 0xFF) == 0xFF);
    bits_local -= unstuff_flag;
    t = t << (8 - unstuff_flag);

    t |= (val >> 24) & 0xFF;
    unstuff = (((val >> 24) & 0xFF) == 0xFF);

    // move to tmp, and push the result all the way up, so we read from the MSB
    tmp |= (static_cast<uint64_t>(t)) << (64 - bits_local - bits);
    bits += bits_local;
  }

  inline void decode() {
    constexpr int32_t MEL_E[13] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5};
    if (bits < 6) {  // if there are less than 6 bits in tmp then read from the MEL bitstream 6 bits that is
                     // the largest decodable MEL codeword.
      read();
    }
    // repeat so long that there is enough decodable bits in tmp, and the runs store is not full
    // (num_runs < 8)
    while (bits >= 6 && num_runs < 8) {
      int32_t eval = MEL_E[MEL_k];
      int32_t run  = 0;
      // The next bit to decode (stored in MSB)
      if (tmp & (1ULL << 63)) {
        // "1" is found
        run = 1 << eval;
        run--;                                        // consecutive runs of 0 events - 1
        MEL_k = ((MEL_k + 1) < 12) ? MEL_k + 1 : 12;  // increment, max is 12
        tmp <<= 1;                                    // consume one bit from tmp
        bits--;
        run <<= 1;  // a stretch of zeros not terminating in one
      } else {
        // "0" is found
        run   = static_cast<int32_t>(tmp >> (63 - eval)) & ((1 << eval) - 1);
        MEL_k = ((MEL_k - 1) > 0) ? MEL_k - 1 : 0;  // decrement, min is 0
        tmp <<= eval + 1;                           // consume eval + 1 bits (max is 6)
        bits -= eval + 1;
        run = (run << 1) + 1;  // a stretch of zeros terminating with one
      }
      eval = num_runs * 7;                             // 7 bits per run
      runs &= ~(static_cast<uint64_t>(0x3F) << eval);  // 6 bits are sufficient
      runs |= (static_cast<uint64_t>(run)) << eval;    // store the value in runs
      num_runs++;                                      // increment count
    }
  }

  inline int32_t get_run() {
    if (num_runs == 0) {  // if no runs, decode more bit from MEL segment
      decode();
    }
    int32_t t = static_cast<int32_t>(runs & 0x7F);  // retrieve one run
    runs >>= 7;                                     // remove the retrieved run
    num_runs--;
    return t;  // return run
  }
};

/********************************************************************************
 * rev_buf:
 *******************************************************************************/
// this class implementation is borrowed from OpenJPH
class rev_buf {
 private:
  uint8_t *buf;
  uint64_t Creg;
  uint32_t bits;
  int32_t length;
  uint32_t unstuff;

 public:
  rev_buf(uint8_t *Dcup, int32_t Lcup, int32_t Scup)
      : buf(Dcup + Lcup - 2), Creg(0), bits(0), length(Scup - 2), unstuff(0) {
    uint32_t d = *buf--;  // read a byte (only use it's half byte)
    Creg       = d >> 4;
    bits       = 4 - ((Creg & 0x07) == 0x07);
    unstuff    = (d | 0x0F) > 0x8f;

    int32_t p = static_cast<int32_t>(reinterpret_cast<intptr_t>(buf) & 0x03);
    //    p &= 0x03;
    int32_t num  = 1 + p;
    int32_t tnum = (num < length) ? num : length;
    for (auto i = 0; i < tnum; ++i) {
      uint64_t d64;
      d64             = *buf--;
      uint32_t d_bits = 8 - ((unstuff && ((d64 & 0x7F) == 0x7F)) ? 1 : 0);
      Creg |= d64 << bits;
      bits += d_bits;
      unstuff = d64 > 0x8F;
    }
    length -= tnum;
    read();
  }

  inline void read() {
    // process 4 bytes at a time
    if (bits > 32) {
      // if there are already more than 32 bits, do nothing to prevent overflow of Creg
      return;
    }
    uint32_t val = 0;
    if (length > 3) {  // Common case; we have at least 4 bytes available
      val = *reinterpret_cast<uint32_t *>(buf - 3);
      buf -= 4;
      length -= 4;
    } else if (length > 0) {  // we have less than 4 bytes
      int i = 24;
      while (length > 0) {
        uint32_t v = *buf--;
        val |= (v << i);
        --length;
        i -= 8;
      }
    } else {
      // error
    }

    // accumulate in tmp, number of bits in tmp are stored in bits
    uint32_t tmp = val >> 24;  // start with the MSB byte
    uint32_t bits_local;

    // test unstuff (previous byte is >0x8F), and this byte is 0x7F
    bits_local        = 8 - ((unstuff && (((val >> 24) & 0x7F) == 0x7F)) ? 1 : 0);
    bool unstuff_flag = (val >> 24) > 0x8F;  // this is for the next byte

    tmp |= ((val >> 16) & 0xFF) << bits_local;  // process the next byte
    bits_local += 8 - ((unstuff_flag && (((val >> 16) & 0x7F) == 0x7F)) ? 1 : 0);
    unstuff_flag = ((val >> 16) & 0xFF) > 0x8F;

    tmp |= ((val >> 8) & 0xFF) << bits_local;
    bits_local += 8 - ((unstuff_flag && (((val >> 8) & 0x7F) == 0x7F)) ? 1 : 0);
    unstuff_flag = ((val >> 8) & 0xFF) > 0x8F;

    tmp |= (val & 0xFF) << bits_local;
    bits_local += 8 - ((unstuff_flag && ((val & 0x7F) == 0x7F)) ? 1 : 0);
    unstuff_flag = (val & 0xFF) > 0x8F;

    // now move the read and unstuffed bits into this->Creg
    Creg |= static_cast<uint64_t>(tmp) << bits;
    bits += bits_local;
    unstuff = unstuff_flag;  // this for the next read
  }

  FORCE_INLINE uint32_t fetch() {
    if (bits < 32) {
      read();
      if (bits < 32) {
        read();
      }
    }
    return static_cast<uint32_t>(Creg);
  }

  inline uint32_t advance(uint32_t num_bits) {
    if (num_bits > bits) {
      printf("ERROR: VLC require %d bits but there are %d bits left\n", num_bits, bits);
    }
    Creg >>= num_bits;
    bits -= num_bits;
    return static_cast<uint32_t>(Creg);
  }
};

/********************************************************************************
 * fwd_buf:
 *******************************************************************************/
template <int X>
class fwd_buf {
 private:
  const uint8_t *buf;
  uint64_t Creg;
  uint32_t bits;
  uint32_t unstuff;
  int32_t length;
  uint32_t pos;

 public:
  fwd_buf(const uint8_t *Dcup, int32_t Pcup)
      : buf(Dcup), Creg(0), bits(0), unstuff(0), length(Pcup), pos(0) {
    // for alignment
    auto p = reinterpret_cast<intptr_t>(buf);
    p &= 0x03;
    auto num = 4 - p;
    for (auto i = 0; i < num; ++i) {
      uint64_t d;
      if (length-- > 0) {
        d = *buf++;
        pos++;
      } else {
        d = (uint64_t)X;
      }
      Creg |= (d << bits);
      bits += 8 - unstuff;
      unstuff = ((d & 0xFF) == 0xFF);  // bit-unstuffing for next byte
    }
    read();
  }

  inline void read() {
    if (bits > 32) {
      printf("ERROR: in MagSgn reading\n");
    }

    uint32_t val = 0;
    if (length > 3) {
      val = *(uint32_t *)(buf);
      buf += 4;
      pos += 4;
      length -= 4;
    } else if (length > 0) {
      int i = 0;
      val   = (X != 0) ? 0xFFFFFFFFU : 0;
      while (length > 0) {
        uint32_t v = *buf++;
        pos++;
        uint32_t m = ~(0xFFU << i);
        val        = (val & m) | (v << i);  // put one byte in its correct location
        --length;
        i += 8;
      }
    } else {
      val = (X != 0) ? 0xFFFFFFFFU : 0;
    }

    // we accumulate in t and keep a count of the number of bits_local in bits_local
    uint32_t bits_local   = 8 - unstuff;
    uint32_t t            = val & 0xFF;
    uint32_t unstuff_flag = ((val & 0xFF) == 0xFF);  // Do we need unstuffing next?

    t |= ((val >> 8) & 0xFF) << bits_local;
    bits_local += 8 - unstuff_flag;
    unstuff_flag = (((val >> 8) & 0xFF) == 0xFF);

    t |= ((val >> 16) & 0xFF) << bits_local;
    bits_local += 8 - unstuff_flag;
    unstuff_flag = (((val >> 16) & 0xFF) == 0xFF);

    t |= ((val >> 24) & 0xFF) << bits_local;
    bits_local += 8 - unstuff_flag;
    unstuff = (((val >> 24) & 0xFF) == 0xFF);  // for next byte

    Creg |= ((uint64_t)t) << bits;  // move data to this->tmp
    bits += bits_local;
  }

  inline void advance(uint32_t n) {
    if (n > bits) {
      printf("ERROR: illegal attempt to advance %d bits but there are %d bits left in MagSgn advance\n", n,
             bits);
  #if defined(__clang__)
      // the following code might be problem with GCC, TODO: to be investigated
  #endif
    }
    Creg >>= n;  // consume n bits
    bits -= n;
  }

  inline uint32_t fetch() {
    if (bits < 32) {
      read();
      if (bits < 32)  // need to test
        read();
    }
    return (uint32_t)Creg;
  }
};

void ht_cleanup_decode(j2k_codeblock *block, const uint8_t &pLSB, const int32_t Lcup, const int32_t Pcup,
                       const int32_t Scup);
