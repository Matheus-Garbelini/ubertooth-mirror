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

#include "ubertooth.h"

#define BTLE_CHANNELS 40
#define ADVERTISING_CHANNELS 3
#define DATA_CHANNELS 37

static const u8 advertising_channels[] = {0, 12, 39};
static const u8 data_channels[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16 , 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38
};

static const u8 whitening[] = {
    1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,
    1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0,
    1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1
};

static const u8 whitening_index[] = {
    70, 62, 120, 111, 77, 46, 15, 101, 66, 39, 31, 26, 80,
    83, 125, 89, 10, 35, 8, 54, 122, 17, 33, 0, 58, 115, 6,
    94, 86, 49, 52, 20, 40, 27, 84, 90, 63, 112, 47, 102
};

static const u8 hop_interval_lut[] = {
    0, 1, 19, 25, 28, 15, 31, 16, 14, 33, 26, 27, 34, 20, 8,
    5, 7, 24, 35, 2, 13, 30, 32, 29, 17, 3, 10, 11, 4, 23, 21,
    6, 22, 9, 12, 18, 36,
};

static const u32 advertising_address = 0x8e89bed6;
u32 access_address;
u8 btle_channel;
u8 hop_increment;

u16 btle_next_hop();
int btle_find_access_address(u8 *idle_rxbuf);
u8 btle_channel_index(u8 channel);
u16 btle_channel_index_to_phys(u8 idx);
u32 btle_calc_crc(u32 crc_init, u8 *data, int len);
u32 btle_reverse_crc(u32 crc, u8 *data, int len);
