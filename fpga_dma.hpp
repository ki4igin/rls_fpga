#pragma once

#include "dma_channel.hpp"

#include <cstddef>
#include <string>

class fpga_dma {
public:
  static constexpr int NUM_TX_CHANNELS = 8;
  static constexpr int NUM_RX_CHANNELS = 1;

  int init();
  bool can_send() const;
  int send(const void *data[NUM_TX_CHANNELS]);
  int send();
  int receive();
  void cleanup();

  void *get_rx_buffer() {
    if (last_rx_buf_id < 0) {
      return nullptr;
    }

    return rx_channel.get_buffer(last_rx_buf_id);
  }

  void *get_tx_buffer(int ch) { return tx_channels[ch].get_buffer(0); }

  void get_all_tx_buffers(int ch, void **buffer_array) {
    tx_channels[ch].get_all_buffers(buffer_array);
  }

  size_t get_submitted() const { return submitted; }
  size_t get_completed() const { return completed; }

private:
  struct config {
    std::string rx_devnode;
    std::string tx_devnodes[NUM_TX_CHANNELS];

    int tx_buf_size;
    int rx_buf_size;

    int rx_buf_count;
  };
  dma_channel rx_channel;
  dma_channel tx_channels[NUM_TX_CHANNELS];

  int tx_buf_size = 0;
  int rx_buf_size = 0;
  int rx_buf_count = 0;

  size_t submitted = 0;
  size_t completed = 0;

  int last_rx_buf_id = -1;
};