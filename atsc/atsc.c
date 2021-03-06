/*****************************************************************************
 * atsc.c : ATSC specific functions
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

#include "../common.h"
#include "atsc.h"

/* Second Loop of PMT */
void parse_ac3_frame( ts_atsc_ac3_info *atsc_ac3_ctx, uint8_t *frame )
{
#if 0
for (int i = 0; i < 16; i++)
printf("%02x ", frame[i]);
printf("\n");
#endif

    /* from the syncframe() frame. See A52 Table 5.1 */
    atsc_ac3_ctx->sample_rate_code = frame[4] >> 6;
    atsc_ac3_ctx->bit_rate_code = frame[4] & 0x3f;

    /* From the bsi() frame. See A52 Table 5.2 */
    atsc_ac3_ctx->bsid = frame[5] >> 3;
    atsc_ac3_ctx->bsmod = frame[5] & 0x7;
    atsc_ac3_ctx->surround_mode = (frame[6] & 1) << 1 | (frame[7] >> 7);
    atsc_ac3_ctx->num_channels = frame[6] >> 5;
    atsc_ac3_ctx->full_svc = 1;

    atsc_ac3_ctx->langcod = 0xff; /* Deprecated in spec, must contain FF */
    atsc_ac3_ctx->langcod2 = 0xff; /* Deprecated in spec, must contain FF */
    atsc_ac3_ctx->mainid = 0;
    atsc_ac3_ctx->priority = 0;
    atsc_ac3_ctx->asvcflags = 0;
}

/* From the A52 spec.
 * "In System A, the AC-3 audio descriptor is titled “AC-3_audio_stream_descriptor” while
 * in System B the AC-3 audio descriptor is titled “AC- 3_descriptor”. It should be noted
 * that the syntax of these descriptors differs significantly between the two systems."
 * ...
 * ATSC has done so to complete the standardization process for System A.
 * ATSC is System A.
 *
 * TODO: This descriptor is missing a number of mandatory fields.
 */
void write_atsc_ac3_descriptor( bs_t *s, ts_atsc_ac3_info *atsc_ac3_ctx )
{
    bs_write( s, 8, ATSC_AC3_DESCRIPTOR_TAG ); // descriptor_tag
    if (atsc_ac3_ctx->num_channels == 0)
        bs_write( s, 8, 8 ); // descriptor_length
    else
        bs_write( s, 8, 7 ); // descriptor_length
    bs_write( s, 3, atsc_ac3_ctx->sample_rate_code ); // sample_rate_code
    bs_write( s, 5, atsc_ac3_ctx->bsid ); // bsid
    bs_write( s, 6, atsc_ac3_ctx->bit_rate_code ); // bit_rate_code
    bs_write( s, 2, atsc_ac3_ctx->surround_mode ); // surround_mode
    bs_write( s, 3, atsc_ac3_ctx->bsmod ); // bsmod
    bs_write( s, 4, atsc_ac3_ctx->num_channels ); // num_channels
    bs_write( s, 1, atsc_ac3_ctx->full_svc);
    bs_write( s, 8, atsc_ac3_ctx->langcod);
    if (atsc_ac3_ctx->num_channels == 0)
        bs_write( s, 8, atsc_ac3_ctx->langcod2);
    if (atsc_ac3_ctx->bsmod < 2) {
        bs_write( s, 8, 0x37); /* mainid = 1, priority = 1, reserved. */
    }

    bs_write( s, 8, 0x01); /* txtlen = 0, encoded = ISO8859-1. */
    bs_write( s, 8, 0x3f); /* language_flag / flag_2 = 0, reserved. */
}

/* Also in EIT */
// FIXME fill in the blanks
void write_caption_service_descriptor( bs_t *s )
{
    bs_write( s, 8, ATSC_CAPTION_SERVICE_DESCRIPTOR_TAG ); // descriptor_tag
    bs_write( s, 8, 0 ); // descriptor_length
    bs_write( s, 3, 0x7 ); // reserved

    bs_write( s, 5, 0 ); // number_of_services

    for( int i = 0; i < 0; i++ )
    {
        bs_write( s, 8, 0 ); // language[0]
        bs_write( s, 8, 0 ); // language[1]
        bs_write( s, 8, 0 ); // language[2]
        bs_write1( s, 0 );   // digital_cc
        bs_write1( s, 1 );   // reserved

        if( 0 )
            bs_write( s, 6, 0x3f ); // reserved
        else
            bs_write( s, 6, 0 ); // caption_service_number

        bs_write1( s, 0 ); // easy_reader
        bs_write1( s, 0 ); // wide_aspect_ratio
        bs_write( s, 13, 0x3fff ); // reserved
    }
}
