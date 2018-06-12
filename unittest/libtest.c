#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <common.h>
#include <libmpegts.h>

#define PID_PMT   48
#define PID_VIDEO 49
#define PID_AUDIO 50
#define PID_PCR   PID_VIDEO

static int test_02(ts_writer_t *tswriter)
{
	uint32_t count = 0;
	int64_t lastpcr = 0;
	int verbose = 0;

	printf("Unit: %s\n", __func__);

	tswriter->serializerFH = fopen("/storage/dev/libmpegts-serializer-frames.bin", "rb");
	while (!feof(tswriter->serializerFH)) {

		ts_frame_t *frame;
		size_t len = libmpegts_frame_serializer_read(tswriter, &frame);
		printf("%08d: len %7d   pid %04x  pts %13" PRIi64 "  dts %13" PRIi64 "  iat: %13" PRIi64 "  fat: %13" PRIi64 "  ",
			count,
			len,
			frame->pid,
			frame->pts,
			frame->dts,
			frame->cpb_initial_arrival_time,
			frame->cpb_final_arrival_time);

		for (int i = 0; i < 16; i++)
			printf("%02x ", frame->data[i]);
		printf("\n");
		if (len <= 0)
			break;

		uint8_t *output = NULL;
		int tslenbytes = 0;
		int64_t *pcr_list;
		int ret = ts_write_frames(tswriter, frame, 1, &output, &tslenbytes, &pcr_list);
		if (ret < 0) {
			fprintf(stderr, "ts_write_frames failed.\n");
			return -1;
		}

		if (verbose && tslenbytes) {
			printf("\t%d ts packets\n", tslenbytes / 188);
			for (int i = 0; i < tslenbytes; i += 188) {
				printf("\t\tpcr %13" PRIi64 " (%6" PRIi64 ")  ", pcr_list[i / 188], pcr_list[i / 188] - lastpcr);
				for (int j = 0; j < 16; j++)
					printf("%02x ", output[i + j]);
				printf("\n");
				lastpcr = pcr_list[i / 188];
			}
		}

		free(frame->data);
		free(frame);
		count++;
	}

	return 0;
}

static int test_01(ts_writer_t *tswriter)
{
	printf("Unit: %s\n", __func__);

	int num_frames = 1;

	ts_frame_t *f = calloc(1, sizeof(*f));
	if (!f) {
		fprintf(stderr, "calloc failed.\n");
		return -1;
	}

	f->opaque                   = NULL; /* User context */
	f->size                     = 0; // nal_length;
	f->data                     = NULL; // nal_ptr;
	f->pid                      = PID_VIDEO;
	f->cpb_initial_arrival_time = 0; /* 27MHz clock */
	f->cpb_final_arrival_time   = 0; /* 27MHz clock */
	f->pts                      = 0; /* 90KHz clock */
	f->dts                      = 0; /* 90KHz clock */
	f->random_access            = 1;
	f->priority                 = 1;

	uint8_t *output = NULL;
	int len = 0;
	int64_t *pcr_list;
	int ret = ts_write_frames(tswriter, f, num_frames, &output, &len, &pcr_list);
	if (ret < 0) {
		fprintf(stderr, "ts_write_frames failed.\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	ts_writer_t *tswriter = ts_create_writer();
	if (!tswriter) {
		fprintf(stderr, "ts_create_writer failed.\n");
		return -1;
	}

	ts_stream_t *stream;
	ts_program_t program = {0};
	program.is_3dtv = 0;
	program.num_streams = 2;
	program.streams = calloc(1, sizeof(ts_stream_t) * program.num_streams);
	program.program_num = 1;
	program.pmt_pid = PID_PMT;
	program.pcr_pid = PID_PCR;

	stream = &program.streams[0];
	stream->stream_id = 1;
	stream->has_stream_identifier = 0;
	stream->pid = PID_VIDEO;
	stream->stream_format = LIBMPEGTS_VIDEO_AVC;
	stream->stream_id = LIBMPEGTS_STREAM_ID_MPEGVIDEO;

	stream = &program.streams[1];
	stream->stream_id = 2;
	stream->has_stream_identifier = 0;
	stream->pid = PID_AUDIO;
	stream->stream_format = LIBMPEGTS_AUDIO_ADTS;
	stream->stream_id = LIBMPEGTS_STREAM_ID_MPEGAUDIO;

	ts_main_t params = {0};
	params.num_programs = 1;
	params.programs = &program;
	params.ts_id = 5678;
	params.muxrate = 8000000; 
	params.cbr = 1;
	params.ts_type = TS_TYPE_GENERIC;
	params.network_pid = 0x10;
	params.legacy_constraints = 0;
	params.pcr_period = 100;
	params.pat_period = 100;
	params.network_id = 2345;
	params.sdt_period = 100;
	params.nit_period = 100;
	params.tdt_period = 100;
	params.tot_period = 100;

	if (ts_setup_transport_stream(tswriter, &params) < 0) {
		fprintf(stderr, "ts_setup_transport_stream failed.\n");
		return -1;
	}

	if (ts_setup_mpegvideo_stream(tswriter, PID_VIDEO, 41, AVC_MAIN, 0, 0, 0) < 0) {
		fprintf(stderr, "ts_setup_mpegvideo_stream failed.\n");
		return -1;
	}

	if (ts_setup_mpeg4_aac_stream(tswriter, PID_AUDIO, LIBMPEGTS_MPEG4_AAC_PROFILE_LEVEL_2, 2) < 0) {
		fprintf(stderr, "ts_setup_mpeg4_aac_stream failed.\n");
		return -1;
	}

	//int ret = test_01(tswriter);
	int ret = test_02(tswriter);
	if (ret < 0) {
		fprintf(stderr, "test failed.\n");
		return -1;
	}

	if (ts_delete_stream(tswriter, 49) != 0) {
		fprintf(stderr, "ts_delete_stream failed.\n");
		return -1;
	}

	if (ts_close_writer(tswriter) != 0) {
		fprintf(stderr, "ts_close_writer failed.\n");
		return -1;
	}

	free(program.streams);
#if 0
	unsigned char *p = NULL;
	printf("%d\n", *p);
#endif
	return 0;
}
