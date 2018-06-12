#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <libmpegts.h>

#define PID_PMT   48
#define PID_VIDEO 49
#define PID_AUDIO 50
#define PID_PCR   PID_VIDEO

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
