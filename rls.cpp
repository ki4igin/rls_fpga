#include "rls.hpp"

#include <cinttypes>
#include <cstdio>

namespace rls {

uint32_t get_rx_words_per_buf(uint32_t test_point, uint32_t u220_hdr_on) {
	switch (test_point) {
	case TP_WORK: return (sizeof(noise_formular) + sizeof(work_posthdr)) / sizeof(uint32_t) + HDR_SIZE;
	case TP_BYPASS: return (N_SAMPS_IN_PACK + HDR_SIZE + u220_hdr_on * U220_HDR_SIZE) * N_PACKS_IN_TX_BUF;
	case TP_CUT:
	case TP_FAPCH:
	case TP_LOU: return (164 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
	case TP_MTI:
	case TP_SF: return (141 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
	case TP_MAX:
	case TP_RANK:
	case TP_APU: return 141 + HDR_SIZE;
	case TP_DDR:
	case TP_FFT:
	case TP_WEIGHT_OUT: return 512 + HDR_SIZE;
	case TP_FIND: return 141 * 5 + HDR_SIZE;
	case TP_FAPCH_COEFFS: return (8 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
	default: return 0;
	}
}


void print_work(const void * data) {
	const auto * work = static_cast<const work_posthdr *>(data);
	printf("Packet Number\t\t\t\t0x%X\n", work->packet_number);
	printf("Number of detections\t\t\t%u\n", work->n_formulars);

	const auto * packets = reinterpret_cast<const formular *>(work + 1);

	for (uint32_t i = 0; i < work->n_formulars; ++i) {
		const auto & packet = packets[i];
		printf("\n");
		printf("Work packet\t\t\t\t%u\n", i + 1);
		printf("Range\t\t\t\t\t%u\n", static_cast<unsigned int>(packet.range));
		printf("Main amplitude at sample %u\t\t%u\n", static_cast<unsigned int>(packet.main_diagram_number), packet.main_amplitude);
		printf("Neighbour amplitude at sample %u\t%u\n",
		       static_cast<unsigned int>(packet.main_diagram_number - 1 + 2 * packet.neighbor_diagram_side),
		       packet.neighbor_amplitude);
		printf("Frequency channel\t\t\t%u\n", static_cast<unsigned int>(packet.frequency_channel));
		printf("Ranker output\t\t\t\t%u\n", packet.rank_out);
	}
	printf("\n");
}


void print_hdr(const void * data) {
	const auto * work_pac = static_cast<const struct work_packet *>(data);
	const auto * hdr      = &work_pac->hdr;

	printf("Delimiter\t\t\t\t0x%X_%X\n", hdr->del_high, hdr->del_low);
	printf("Packet Number\t\t\t\t0x%X\n", hdr->packet_number);
	printf("Timestamp\t\t\t\t%" PRIu64 "\n", hdr->timestamp);
	printf("Channel\t\t\t\t\t%u\n", static_cast<unsigned int>(hdr->channel));
	printf("Range gate\t\t\t\t%u\n", static_cast<unsigned int>(hdr->range));
	printf("Test point\t\t\t\t%u\n", static_cast<unsigned int>(hdr->tp));

	if (hdr->tp == TP_WORK) {
		print_noise(&work_pac->nf);
		print_work(&work_pac->posthdr);
	}
}

void print_noise(const void * data) {
	const auto * noise = static_cast<const noise_formular *>(data);

	printf("\nNoise formular\n");
	printf("Packet Number\t\t\t\t0x%X\n", noise->packet_number);
	for (uint32_t i = 0; i < 8; ++i) {
		printf("Noise FAP [%u]\t\t\t\t%u\n", i, noise->noise_fap[i]);
	}
	for (uint32_t i = 0; i < 8; ++i) {
		printf("Noise LOU [%u]\t\t\t\t%u\n", i, noise->noise_lou[i]);
	}

	printf("\n");
}

} // namespace rls
