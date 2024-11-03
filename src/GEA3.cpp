/*!
 * @file
 * @brief
 */

#include "GEA3.h"

extern "C" {
#include "tiny_time_source.h"
}

static const tiny_erd_client_configuration_t client_configuration = {
  .request_timeout = 250,
  .request_retries = 10
};

void GEA3::begin(Stream& uart, uint8_t clientAddress)
{
  tiny_timer_group_init(&timer_group, tiny_time_source_init());

  tiny_stream_uart_init(&stream_uart, &timer_group, uart);

  tiny_gea3_interface_init(
    &gea3_interface,
    &stream_uart.interface,
    clientAddress,
    send_buffer,
    sizeof(send_buffer),
    receive_buffer,
    sizeof(receive_buffer),
    send_queue_buffer,
    sizeof(send_queue_buffer));

  tiny_erd_client_init(
    &erd_client,
    &timer_group,
    &gea3_interface.interface,
    client_queue_buffer,
    sizeof(client_queue_buffer),
    &client_configuration);
}

void GEA3::loop()
{
  tiny_timer_group_run(&timer_group);
  tiny_gea3_interface_run(&gea3_interface);
}

void GEA3::send(const GEA3::Packet& packet)
{
  auto raw_packet = packet.getRawPacket();

  struct Context {
    const tiny_gea3_packet_t* rawPacket;
  };
  auto context = Context{ raw_packet };

  tiny_gea3_interface_send(
    &gea3_interface.interface,
    raw_packet->destination,
    raw_packet->payload_length,
    +[](void* _context, tiny_gea3_packet_t* packet) {
      auto context = static_cast<Context*>(_context);
      memcpy(packet->payload, context->rawPacket->payload, context->rawPacket->payload_length);
    },
    &context);
}
