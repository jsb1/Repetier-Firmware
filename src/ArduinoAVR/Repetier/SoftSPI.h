/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SOFTSPI_H_
#define SOFTSPI_H_

#include "fastio_template.h"

enum SoftSPIFlags {
	NONE = 0, CPHA_MODE = 1, CPOL_MODE = 2, MISO_PULLUP = 4
};

/**
 * adapted from https://github.com/niteris/ArduinoSoftSpi
 * @class SoftSPI
 * @brief Fast software SPI.
 */
template<uint8_t SckPin, uint8_t MisoPin = DioDummyPin::pin, uint8_t MosiPin = DioDummyPin::pin,
		SoftSPIFlags Mode = NONE>
class SoftSPI {
public:
	SoftSPI() {
		begin();
	}
	//----------------------------------------------------------------------------
	/** Initialize SoftSPI pins. */
	void begin() {
		fastPinConfig<MisoPin>(false, Mode & MISO_PULLUP);
		fastPinConfig<MosiPin>(true, !MODE_CPHA());
		fastPinConfig<SckPin>(true, MODE_CPOL());
	}
	//----------------------------------------------------------------------------
	/** Soft SPI receive byte.
	 * @return Data byte received.
	 */
	uint8_t receive() {
		uint8_t data = 0;
		for (uint8_t m = 0x80; m != 0; m >>= 1) {
			if (receiveBit())
				data |= m;
		}
		return data;
	}
	//----------------------------------------------------------------------------
	/** Soft SPI send byte.
	 * @param[in] data Data byte to send.
	 */
	void send(uint8_t data) {
		for (uint8_t m = 0x80; m != 0; m >>= 1) {
			sendBit((data & m) != 0);
		}
	}
	//----------------------------------------------------------------------------
	/** Soft SPI transfer byte.
	 * @param[in] txData Data byte to send.
	 * @return Data byte received.
	 */
	uint8_t transfer(uint8_t txData) {
		uint8_t rxData = 0;
		for (uint8_t m = 0x80; m != 0; m >>= 1) {
			if (transferBit((txData & m) != 0))
				rxData |= m;
		}
		return rxData;
	}

private:
	//----------------------------------------------------------------------------
	template<uint8_t pin> void fastPinMode(bool mode) {
		DioPin<pin>::setDirection(mode);
	}
	template<uint8_t pin> bool fastDigitalRead() {
		return DioPin<pin>::getValue();
	}
	template<uint8_t pin> void fastDigitalWrite(bool level) {
		DioPin<pin>::setValue(level);
	}
	template<uint8_t pin> void fastPinConfig(bool mode, bool level) {
		fastPinMode<pin>(mode);
		fastDigitalWrite<pin>(level);
	}

	inline __attribute__((always_inline))
	bool MODE_CPOL() {
		return (Mode & CPOL_MODE) != 0;
	}
	inline __attribute__((always_inline))
	bool MODE_CPHA() {
		return (Mode & CPHA_MODE) != 0;
	}

	inline __attribute__((always_inline))
	bool receiveBit() {
		if (MODE_CPHA()) {
			fastDigitalWrite<SckPin>(!MODE_CPOL());
		}
		fastDigitalWrite<SckPin>(MODE_CPHA() ? MODE_CPOL() : !MODE_CPOL());
		bool ret = fastDigitalRead<MisoPin>();
		if (!MODE_CPHA()) {
			fastDigitalWrite<SckPin>(MODE_CPOL());
		}
		return ret;
	}
	//----------------------------------------------------------------------------
	inline __attribute__((always_inline))
	void sendBit(bool bit) {
		if (MODE_CPHA()) {
			fastDigitalWrite<SckPin>(!MODE_CPOL());
		}
		fastDigitalWrite<MosiPin>(bit);
		fastDigitalWrite<SckPin>(MODE_CPHA() ? MODE_CPOL() : !MODE_CPOL());
		if (!MODE_CPHA()) {
			fastDigitalWrite<SckPin>(MODE_CPOL());
		}
	}
	//----------------------------------------------------------------------------
	inline __attribute__((always_inline))
	bool transferBit(bool bit) {
		if (MODE_CPHA()) {
			fastDigitalWrite<SckPin>(!MODE_CPOL());
		}
		fastDigitalWrite<MosiPin>(bit);
		fastDigitalWrite<SckPin>(MODE_CPHA() ? MODE_CPOL() : !MODE_CPOL());
		bool ret = fastDigitalRead<MisoPin>();
		if (!MODE_CPHA()) {
			fastDigitalWrite<SckPin>(MODE_CPOL());
		}
		return ret;
	}
	//----------------------------------------------------------------------------
};

#undef nop
#endif /* SOFTSPI_H_ */
