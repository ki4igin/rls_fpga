#pragma once

#include <cstddef>
#include <cstdint>

namespace rls {

constexpr std::size_t N_SAMPS_IN_PACK   = 232;
constexpr std::size_t N_PACKS_IN_TX_BUF = 20;
constexpr std::size_t HDR_SIZE          = 6;
constexpr std::size_t U220_HDR_SIZE     = 2;
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
	TP_PU_IN        = 15,
	TP_SDR          = 99,
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
	uint32_t n_formulars;
};

struct noise_formular {
	uint32_t packet_number;
	uint32_t noise_fap[8];
	uint32_t noise_lou[8];
};

// Signed Q14 coefficients, each occupying a 32-bit DMA word.
struct compens_channel {
	int32_t re: 24;
	uint32_t  : 8;
	int32_t im: 24;
	uint32_t  : 8;
};

struct compens_formular {
	uint32_t packet_number;
	struct compens_channel channels[NUM_CHANNELS];
};

static_assert(sizeof(compens_channel) == 8, "Compensation channel must occupy two DMA words");
static_assert(sizeof(compens_formular) == 68, "Compensation formular must occupy 17 DMA words");
static_assert(offsetof(compens_formular, channels) == 4, "Coefficients must follow the packet number");

struct formular {
	uint32_t main_amplitude;
	uint32_t neighbor_amplitude;

	uint8_t range                 : 8;
	uint16_t main_diagram_number  : 3;
	uint16_t neighbor_diagram_side: 1;
	uint16_t frequency_channel    : 9;
	uint16_t padding              : 11;

	uint32_t rank_out;
};

struct range_packet {
	uint32_t n_ranges;
	uint32_t ranges[];
};

static_assert(sizeof(range_packet) == sizeof(uint32_t), "Range header must occupy one DMA word");

struct work_packet {
	struct header hdr;
	struct noise_formular nf;
	struct compens_formular cf;
	struct work_posthdr posthdr;
	struct formular formulars[];
};

#pragma pack(pop)

// The range packet follows the variable number of detection formulars.
inline const range_packet * get_range_packet(const work_packet * work) {
	return reinterpret_cast<const range_packet *>(work->formulars + work->posthdr.n_formulars);
}

uint32_t get_rx_words_per_buf(uint32_t tp, uint32_t u220_hdr_on = 0);
void print_hdr(const void * data);
void print_work(const void * data);
void print_noise(const void * data);
void print_ranges(const void * data);

} // namespace rls
