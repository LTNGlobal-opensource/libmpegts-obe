/*****************************************************************************
 * codecs.h : Codec specific headers
 *****************************************************************************
 * Copyright (C) 2010 Kieran Kunhya
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#ifndef LIBMPEGTS_CODECS_H
#define LIBMPEGTS_CODECS_H

/* Video */
typedef struct
{
    int level;
    int profile;
    int bitrate;      /* max bitrate (kbit/sec) */
    int vbv;          /* max vbv buffer (kbit) */
} mpeg2_level_t;

const mpeg2_level_t mpeg2_levels[] =
{
    { LIBMPEGTS_MPEG2_LEVEL_LOW,      LIBMPEGTS_MPEG2_PROFILE_MAIN,   4000000,  475136 },
    { LIBMPEGTS_MPEG2_LEVEL_MAIN,     LIBMPEGTS_MPEG2_PROFILE_SIMPLE, 15000000, 1835008 },
    { LIBMPEGTS_MPEG2_LEVEL_MAIN,     LIBMPEGTS_MPEG2_PROFILE_MAIN,   15000000, 1835008 },
    { LIBMPEGTS_MPEG2_LEVEL_MAIN,     LIBMPEGTS_MPEG2_PROFILE_422,    50000000, 9437184 },
    { LIBMPEGTS_MPEG2_LEVEL_HIGH_1440,LIBMPEGTS_MPEG2_PROFILE_MAIN,   60000000, 7340732 },
    { LIBMPEGTS_MPEG2_LEVEL_HIGH,     LIBMPEGTS_MPEG2_PROFILE_MAIN,   80000000, 9781248 },
    { LIBMPEGTS_MPEG2_LEVEL_HIGHP,    LIBMPEGTS_MPEG2_PROFILE_MAIN,   80000000, 9781248 },
    { 0 }
};

/* See ISO14496-10:2014 -- Table A-1 Level Limits */
typedef struct
{
    int level_idc;
    int bitrate;     /* Max video bitrate (kbit/sec) */
    int cpb;         /* Max CPB Size (kbit/sec) */
} avc_level_t;

const avc_level_t avc_levels[] =
{
    { 10,     64,     64 }, /* level 1.0 */
    {  9,    128,    350 }, /* level 1b */
    { 11,    192,    500 },
    { 12,    384,   1000 },
    { 13,    768,   2000 },
    { 20,   2000,   2000 },
    { 21,   4000,   4000 },
    { 22,   4000,   4000 },
    { 30,  10000,  10000 },
    { 31,  14000,  14000 },
    { 32,  20000,  20000 },
    { 40,  20000,  25000 },
    { 41,  50000,  62500 },
    { 42,  50000,  62500 },
    { 50, 135000, 135000 },
    { 51, 240000, 240000 },
    { 52, 240000, 240000 }, /* level 5.2 */
    { 0 }
};

const uint8_t avc_profiles[] =
{
    [AVC_BASELINE] = 66,
    [AVC_MAIN]     = 77,
    [AVC_HIGH]     = 100,
    [AVC_HIGH_10]  = 110,
    [AVC_HIGH_422] = 122,
    [AVC_HIGH_444_PRED]   = 244,
    [AVC_HIGH_10_INTRA]   = 110,
    [AVC_HIGH_422_INTRA]  = 122,
    [AVC_HIGH_444_INTRA]  = 244,
    [AVC_CAVLC_444_INTRA] = 44,
};

/* See ISO14496-10:2014 -- Table A-2 */
const int avc_nal_factor[] =
{
    [AVC_BASELINE] = 1200,
    [AVC_MAIN]     = 1200,
    [AVC_HIGH]     = 1500,
    [AVC_HIGH_10]  = 3600,
    [AVC_HIGH_422] = 4800,
    [AVC_HIGH_444_PRED]   = 4800,
    [AVC_HIGH_10_INTRA]   = 3600,
    [AVC_HIGH_422_INTRA]  = 4800,
    [AVC_HIGH_444_INTRA]  = 4800,
    [AVC_CAVLC_444_INTRA] = 4800,
};

/* Audio */
typedef struct
{
    int max_channels;
    int rxn;         /* Leak rate from Transport Buffer */
    int bsn;         /* Size of Main buffer */
} aac_buffer_t;

const aac_buffer_t aac_buffers[] =
{
    { 2,  2000000,  3584*8 },
    { 8,  5529600,  8976*8 },
    { 12, 8294400,  12804*8 },
    { 48, 33177600, 51216*8 },
    { 0 },
};

/* See ISO23008:2014 -- Table A-6 General tier and level limits (cbp)
 *                      Table A-7 Tier and level limits for the video profiles. (bitrate)
 */
typedef struct
{
    uint32_t level_idc;
    uint32_t max_bitrate_main;
    uint32_t max_bitrate_high;
    uint32_t max_cpbsize_main;
    uint32_t max_cpbsize_high;
} hevc_level_t;

const hevc_level_t hevc_levels[] =
{
    { 10,    128, UINT32_MAX,    350, UINT32_MAX, },
    { 20,   1500, UINT32_MAX,   1500, UINT32_MAX, },
    { 21,   3000, UINT32_MAX,   3000, UINT32_MAX, },
    { 30,   6000, UINT32_MAX,   6000, UINT32_MAX, },
    { 31,  10000, UINT32_MAX,  10000, UINT32_MAX, },
    { 40,  12000,      30000,  12000,      30000, },
    { 41,  20000,      50000,  20000,      50000, },
    { 50,  25000,     100000,  25000,     100000, },
    { 51,  40000,     160000,  40000,     160000, },
    { 52,  60000,     240000,  60000,     240000, },
    { 60,  60000,     240000,  60000,     240000, },
    { 61, 120000,     480000, 120000,     480000, },
    { 62, 240000,     800000, 240000,     800000, },
};

/* See ISO23008:2014 -- Table A-8 */
/* The spec provides LOTS of factors. They're not all implemented here.
 * I'll focus on 4:2:2 8bit main and high.
 */
const int hevc_nal_factor[] =
{
    [HEVC_PROFILE_MAIN] = 1100, /* Profile column 1 = 'main' */
    [HEVC_PROFILE_HIGH] = 2200, /* Profile column 1 = 'High throughput 4:4:4' */
};

/* AC3 buffer sizes */
#define AC3_BS_ATSC         2592*8
#define AC3_BS_DVB          5696*8

/* SMPTE 302M */
#define SMPTE_302M_AUDIO_BS 65024*8
#define SMPTE_302M_AUDIO_SR 48000

/* Misc Audio */
#define MISC_AUDIO_BS       3584*8
#define MISC_AUDIO_RXN      2000000

/* DVB Subtitles */
#define DVB_SUB_RXN         192000
#define DVB_SUB_MB_SIZE     24000
#define DVB_SUB_DDS_TB_SIZE TB_SIZE*2*8
#define DVB_SUB_DDS_RXN     400000
#define DVB_SUB_DDS_MB_SIZE 100000

/* Teletext */
#define TELETEXT_T_BS       480*8 /* Seems to have an odd Transport Buffer size */
#define TELETEXT_RXN        6750000
#define TELETEXT_BTTX       1504*8

/* SCTE VBI */
#define SCTE_VBI_RXN        324539
#define SCTE_VBI_MB_SIZE    2256*8

#endif
