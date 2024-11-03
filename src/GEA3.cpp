/*!
 * @file
 * @brief
 */

#include "GEA3.h"
#include "tiny_utils.h"

extern "C" {
#include "tiny_time_source.h"
}

static const tiny_erd_client_configuration_t client_configuration = {
  .request_timeout = 250,
  .request_retries = 10
};

#include <Arduino.h>

extern "C" {
#include "hal/i_tiny_uart.h"
#include "tiny_event.h"
#include "tiny_timer.h"
}

void GEA3::poll()
{
  int rxBytes = uart_adapter.uart->available();

  while(rxBytes--) {
    int byte = uart_adapter.uart->read();
    tiny_uart_on_receive_args_t args = { (uint8_t)byte };
    tiny_event_publish(&uart_adapter.receive_event, &args);
  }

  if(uart_adapter.sent) {
    uart_adapter.sent = false;
    tiny_event_publish(&uart_adapter.send_complete_event, NULL);
  }
}

void GEA3::send(i_tiny_uart_t* _self, uint8_t byte)
{
  auto self = container_of(GEA3, uart_adapter.interface, _self);
  self->uart_adapter.sent = true;
  self->uart_adapter.uart->write(byte);
}

i_tiny_event_t* GEA3::on_send_complete(i_tiny_uart_t* _self)
{
  auto self = container_of(GEA3, uart_adapter.interface, _self);
  return &self->uart_adapter.send_complete_event.interface;
}

i_tiny_event_t* GEA3::on_receive(i_tiny_uart_t* _self)
{
  auto self = container_of(GEA3, uart_adapter.interface, _self);
  return &self->uart_adapter.receive_event.interface;
}

void GEA3::begin(Stream& uart, uint8_t clientAddress)
{
  tiny_timer_group_init(&timer_group, tiny_time_source_init());

  uart_adapter.interface.api = &uart_adapter_api;
  uart_adapter.uart = &uart;
  tiny_event_init(&uart_adapter.send_complete_event);
  tiny_event_init(&uart_adapter.receive_event);
  tiny_timer_start_periodic(&timer_group, &uart_adapter.timer, 0, this, +[](void* self) { static_cast<GEA3*>(self)->poll(); });

  tiny_gea3_interface_init(
    &gea3_interface,
    &uart_adapter.interface,
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
