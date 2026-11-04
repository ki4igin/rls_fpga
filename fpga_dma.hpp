#pragma once

#include "dma_channel.hpp"

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <string>

class fpga_dma {
public:
	static constexpr int NUM_TX_CHANNELS = 8;
	static constexpr int NUM_RX_CHANNELS = 1;

	struct rx_buffer_info {
		int buf_id              = -1;
		const uint32_t * buffer = nullptr;
		std::chrono::steady_clock::time_point timestamp{};
	};

	int init(int u220_hdr_on = 0);
	bool can_send() const;
	int send(const void * data[NUM_TX_CHANNELS]);
	int send();
	int receive();
	void cleanup();


	void * get_tx_buffer(int ch) { return tx_channels[ch].get_buffer(0); }
	void * get_current_tx_buffer(int ch) { return tx_channels[ch].get_buffer(static_cast<int>(submitted % TX_BUFFER_COUNT)); }
	void get_all_tx_buffers(int ch, void ** buffer_array) { tx_channels[ch].get_all_buffers(buffer_array); }

	size_t get_submitted() const { return submitted; }
	size_t get_completed() const { return completed; }

	bool try_get_rx_buffer(rx_buffer_info & info);
	size_t get_rx_ready_count() const;
	rx_buffer_info get_rx_buffer();

private:
	struct config {
		std::string rx_devnode;
		std::string tx_devnodes[NUM_TX_CHANNELS];

		int tx_buf_size;
		int rx_buf_size;

		int rx_buf_count;
	};

	struct rx_queue {
		std::array<rx_buffer_info, RX_BUFFER_COUNT> buffers;
		size_t write_seq = 0;
		size_t read_seq  = 0;
		mutable std::mutex mutex;
	};

	rx_queue rx;

	dma_channel rx_channel;
	dma_channel tx_channels[NUM_TX_CHANNELS];

	int tx_buf_size    = 0;
	int rx_buf_size    = 0;
	int rx_buf_count   = 0;

	size_t submitted   = 0;
	size_t completed   = 0;

	int last_rx_buf_id = -1;
};
