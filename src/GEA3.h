/*!
 * @file
 * @brief
 */

#ifndef GEA3_h
#define GEA3_h

#include <Arduino.h>
#include <cstdint>
#include <memory>

extern "C" {
#include "tiny_erd_client.h"
#include "tiny_gea3_interface.h"
#include "tiny_timer.h"
}

class GEA3 {
 public:
  class Packet {
   public:
    friend class GEA3;

    Packet(uint8_t source, uint8_t destination, std::initializer_list<uint8_t> payload)
    {
      this->rawPacket = std::unique_ptr<char[]>(new char[sizeof(RawPacket) + payload.size()]);
      auto rawPacket = reinterpret_cast<RawPacket*>(this->rawPacket.get());
      rawPacket->destination = destination;
      rawPacket->payloadLength = payload.size();
      rawPacket->source = destination;
      memcpy(rawPacket->payload, payload.begin(), payload.size());
    }

    template <typename T>
    Packet(uint8_t source, uint8_t destination, const T& payload)
    {
      this->rawPacket = std::unique_ptr<char[]>(new char[sizeof(RawPacket) + sizeof(T)]);
      auto rawPacket = reinterpret_cast<RawPacket*>(this->rawPacket.get());
      rawPacket->destination = destination;
      rawPacket->payloadLength = sizeof(T);
      rawPacket->source = destination;
      memcpy(rawPacket->payload, &payload, sizeof(T));
    }

   private:
    struct RawPacket {
      uint8_t destination;
      uint8_t payloadLength;
      uint8_t source;
      uint8_t payload[0];
    };

    std::unique_ptr<char[]> rawPacket;

    const tiny_gea3_packet_t* getRawPacket() const
    {
      return reinterpret_cast<const tiny_gea3_packet_t*>(this->rawPacket.get());
    }
  };

  static constexpr unsigned long baud = 230400;

  void begin(Stream& uart, uint8_t address = 0xE4);
  void loop();
  void send(const GEA3::Packet& packet);

  uint8_t readERD(uint8_t address, uint16_t erd)
  {
    tiny_erd_client_request_id_t request_id;
    tiny_erd_client_read(&erd_client.interface, &request_id, address, erd);
    return request_id;
  }

  template <typename T>
  uint8_t writeERD(uint8_t address, uint16_t erd, T value)
  {
    tiny_erd_client_request_id_t request_id;
    tiny_erd_client_write(&erd_client.interface, &request_id, address, erd, &value, sizeof(value));
    return request_id;
  }

 private:
  static void send(i_tiny_uart_t* _self, uint8_t byte);
  static i_tiny_event_t* on_send_complete(i_tiny_uart_t* _self);
  static i_tiny_event_t* on_receive(i_tiny_uart_t* _self);

  const i_tiny_uart_api_t uart_adapter_api = { GEA3::send, GEA3::on_send_complete, GEA3::on_receive };

  tiny_timer_group_t timer_group;

  tiny_gea3_interface_t gea3_interface;
  uint8_t send_buffer[255];
  uint8_t receive_buffer[255];
  uint8_t send_queue_buffer[1000];

  tiny_erd_client_t erd_client;
  uint8_t client_queue_buffer[1024];

  struct {
    i_tiny_uart_t interface;

    Stream* uart;
    tiny_event_t send_complete_event;
    tiny_event_t receive_event;
    tiny_timer_group_t* timer_group;
    tiny_timer_t timer;
    bool sent;
  } uart_adapter;

  void poll();
};

#endif
