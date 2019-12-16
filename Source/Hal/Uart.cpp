/*
 * Uart.cpp
 *
 *  Created on: 26 Jun 2019
 *      Author: yuri
 */

#include "Uart.h"
#include "driver/uart.h"
#include "Dwt.h"

namespace Hal
{

Uart::Uart(Gpio *gpio, UartPort uartPort, uint32_t baudRate, Gpio::GpioIndex txPin, Gpio::GpioIndex rxPin) : _gpio(gpio), _uartPort(uartPort), _baudRate(baudRate), _txPin(txPin), _rxPin(rxPin)
{
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.use_ref_tick = false};
	uart_param_config(static_cast<uart_port_t>(_uartPort), &uart_config);
	uart_set_pin(static_cast<uart_port_t>(_uartPort), static_cast<int>(_txPin), static_cast<int>(_rxPin), 0, 0);
	uart_driver_install(static_cast<uart_port_t>(_uartPort), UartBufferSize, 0, 0, NULL, 0);
	_driverEnabled = true;
}

Uart::~Uart()
{
}

void Uart::Configure(uint32_t baudRate, DataBit dataBit,
					 Parity parity, StopBit stopBit, HardwareFlowControl flowControl)
{
	_baudRate = baudRate;
	_dataBit = dataBit;
	_parity = parity;
	_stopBit = stopBit;
	_flowControl = flowControl;

	Disable();
	Dwt::DelayMilliseconds(50);
	Enable();
}

void Uart::Enable()
{
	if (_driverEnabled)
		return;

	_gpio->SetAlternate(_txPin, Gpio::AltFunc::Uart);
	_gpio->SetAlternate(_rxPin, Gpio::AltFunc::Uart);

	uart_config_t uart_config = {
		.baud_rate = static_cast<int>(_baudRate),
		.data_bits = static_cast<uart_word_length_t>(_dataBit),
		.parity = static_cast<uart_parity_t>(_parity),
		.stop_bits = static_cast<uart_stop_bits_t>(_stopBit),
		.flow_ctrl = static_cast<uart_hw_flowcontrol_t>(_flowControl),
		.rx_flow_ctrl_thresh = 0,
		.use_ref_tick = false};

	uart_param_config(static_cast<uart_port_t>(_uartPort), &uart_config);
	uart_set_pin(static_cast<uart_port_t>(_uartPort), static_cast<int>(_txPin), static_cast<int>(_rxPin), 0, 0);
	uart_driver_install(static_cast<uart_port_t>(_uartPort), UartBufferSize, 0, 0, NULL, 0);

	_driverEnabled = true;
}

void Uart::Disable()
{
	if (_driverEnabled == false)
		return;

	uart_driver_delete(static_cast<uart_port_t>(_uartPort));
	_gpio->SetAlternate(_txPin, Gpio::AltFunc::Gpio);
	_gpio->SetAlternate(_rxPin, Gpio::AltFunc::Gpio);

	_driverEnabled = false;
}

void Uart::Write(const char *data, uint16_t len)
{
	uart_write_bytes(static_cast<uart_port_t>(_uartPort), data, len);
}

int Uart::Read(uint8_t *data, uint16_t len)
{
	return uart_read_bytes(static_cast<uart_port_t>(_uartPort), data, len, UartTimeOut);
}

} // namespace Hal
