#pragma once

#include <cstddef>
#include <cstdint>

namespace rls {

constexpr std::size_t N_SAMPS_IN_PACK   = 232;
constexpr std::size_t N_PACKS_IN_TX_BUF = 20;
constexpr std::size_t HDR_SIZE          = 6;
constexpr std::size_t N_SAMPS_IN_TX_BUF = N_SAMPS_IN_PACK * N_PACKS_IN_TX_BUF;
constexpr std::size_t TX_BUF_SIZE       = sizeof(uint32_t) * N_SAMPS_IN_TX_BUF;
constexpr std::size_t NUM_CHANNELS      = 8;

enum tp {
	TP_WORK         = 0,
	TP_BYPASS       = 1,
	TP_CUT          = 2,
	TP_FAPCH        = 3,
	TP_LOU          = 4,
	TP_SF           = 5,
	TP_DDR          = 6,
	TP_FFT          = 7,
	TP_MAX          = 8,
	TP_FIND         = 9,
	TP_RANK         = 10,
	TP_APU          = 11,
	TP_FAPCH_COEFFS = 12,
	TP_WEIGHT_OUT   = 13,
	TP_MTI          = 14,
};

#pragma pack(push, 1)

struct header {
	uint32_t del_high;
	uint32_t del_low;
	uint32_t packet_number;
	uint64_t timestamp;

	uint32_t channel: 3;
	uint32_t range  : 13;
	uint32_t tp     : 16;
};

struct work_posthdr {
	uint32_t packet_number;
	uint32_t n_work_packets;
};

struct work_packet {
	uint32_t main_amplitude;
	uint32_t neighbor_amplitude;

	uint8_t range                 : 8;
	uint16_t main_diagram_number  : 3;
	uint16_t neighbor_diagram_side: 1;
	uint16_t frequency_channel    : 9;
	uint16_t padding              : 11;

	uint32_t rank_out;
};

#pragma pack(pop)

uint32_t get_rx_words_per_buf(uint32_t tp);
void print_hdr(const void * data);
void print_work(const void * data);

} // namespace rls
