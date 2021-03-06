/*
 * Copyright 2012 Dominic Spill
 *
 * This file is part of Project Ubertooth.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "bluetooth_le.h"

extern u8 le_channel_idx;
extern u8 le_hop_amount;

u16 btle_next_hop()
{
	le_channel_idx = (le_channel_idx + le_hop_amount) % 37;
	return btle_channel_index_to_phys(le_channel_idx);
}

u32 received_data = 0;

int btle_find_access_address(u8 *idle_rxbuf)
{
	/* Looks for an AA in the stream */
	u16 count;
	u8 curr_buf;
	int i = 0;

	if (received_data == 0) {
		for (; i<8; i++) {
			received_data <<= 8;
			received_data |= idle_rxbuf[i];
		}
	}
	curr_buf = idle_rxbuf[i];

	// Search until we're 32 symbols from the end of the buffer
	for(count = 0; count < ((8 * DMA_SIZE) - 32); count++)
	{
		if (received_data == access_address)
			return count;

		if (count%8 == 0)
			curr_buf = idle_rxbuf[++i];

		received_data <<= 1;
		curr_buf <<= 1;
	}
	return -1;
}

u8 btle_channel_index(u8 channel) {
	u8 idx;
	channel /= 2;
	if (channel == 0)
		idx = 37;
	else if (channel < 12)
		idx = channel - 1;
	else if (channel == 12)
		idx = 38;
	else if (channel < 39)
		idx = channel - 2;
	else
		idx = 39;
	return idx;
}

u16 btle_channel_index_to_phys(u8 idx) {
	u16 phys;
	if (idx < 11)
		phys = 2404 + 2 * idx;
	else if (idx < 37)
		phys = 2428 + 2 * (idx - 11);
	else if (idx == 37)
		phys = 2402;
	else if (idx == 38)
		phys = 2426;
	else
		phys = 2480;
	return phys;
}

// calculate CRC
//	note 1: crc_init's bits should be in reverse order
//	note 2: output bytes are in reverse order compared to wire
//
//		example output:
//			0x6ff46e
//
//		bytes in packet will be:
//		  { 0x6e, 0xf4, 0x6f }
//
u32 btle_calc_crc(u32 crc_init, u8 *data, int len) {
	u32 state = crc_init;
	u32 lfsr_mask = 0x5a6000; // 010110100110000000000000
	int i, j;

	for (i = 0; i < len; ++i) {
		u8 cur = data[i];
		for (j = 0; j < 8; ++j) {
			int next_bit = (state ^ cur) & 1;
			cur >>= 1;
			state >>= 1;
			if (next_bit) {
				state |= 1 << 23;
				state ^= lfsr_mask;
			}
		}
	}

	return state;
}

// runs the CRC in reverse to generate a CRCInit
//
//	crc should be big endian
//	the return will be big endian
//
u32 btle_reverse_crc(u32 crc, u8 *data, int len) {
	u32 state = crc;
	u32 lfsr_mask = 0xb4c000; // 101101001100000000000000
	u32 ret;
	int i, j;

	for (i = len - 1; i >= 0; --i) {
		u8 cur = data[i];
		for (j = 0; j < 8; ++j) {
			int top_bit = state >> 23;
			state = (state << 1) & 0xffffff;
			state |= top_bit ^ ((cur >> (7 - j)) & 1);
			if (top_bit)
				state ^= lfsr_mask;
		}
	}

	ret = 0;
	for (i = 0; i < 24; ++i)
		ret |= ((state >> i) & 1) << (23 - i);

	return ret;
}
