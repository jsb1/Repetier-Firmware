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

#ifndef SOFTPOTMANAGER_H_
#define SOFTPOTMANAGER_H_

#include <util/delay.h>
#include "fastio_template.h"

#define I2C_DELAY_USEC 9

template<typename SCL_PIN, typename ... SDA_PINS> class SoftPotManager;

template<typename SCL_PIN> class SoftPotManager<SCL_PIN> {
public:
	void init(bool) const {
	}
	bool start(uint8_t,uint8_t, bool) const {
		return false;
	}
	void stop(uint8_t, bool) const {
	}
	uint8_t read(uint8_t, bool) const {return 0;}
	bool write(uint8_t , uint8_t) const {
		return false;
	}
	void setSDA(bool) const {}
};

/**
 * adapted from original firmware
 * Warning: this is no real I2C! Normally lines were not driven actively high.
 */
template<typename SCL_PIN, typename SDA_PIN, typename ... OTHER_SDA_PINS>
class SoftPotManager<SCL_PIN, SDA_PIN, OTHER_SDA_PINS ...> : public SoftPotManager<
		SCL_PIN, OTHER_SDA_PINS ...> {
private:
	SoftPotManager<SCL_PIN, OTHER_SDA_PINS ...> &rest() {
		return *this;
	}

public:
   const static size_t numChannels = 1 + sizeof...(OTHER_SDA_PINS);

// init pins and set bus high
	void init(bool isFirst=true) {
		rest().init(false);
		SDA_PIN::setDirection(true);
		SDA_PIN::setValue(true);
		if (isFirst) {
			SCL_PIN::setDirection(true);
			SCL_PIN::setValue(true);
			delay();
		}
	}

	bool writeByte(uint8_t channel, uint8_t address, uint8_t v) {
		start(channel, address | I2C_WRITE);
		bool ret = write(channel,v);
		stop(channel);
		return ret;
	}
	uint8_t readByte(uint8_t channel, uint8_t address) {
		start(channel, address | I2C_READ);
		uint8_t ret = read(channel, true);
		stop(channel);
		return ret;
	}

//------------------------------------------------------------------------------
// send new address and read/write without stop
	uint8_t restart(uint8_t channel, uint8_t addressRW) {
		if (channel == 0) {
			SCL_PIN::setValue(true);
			return start(addressRW);
		}
		return rest().restart(channel - 1, addressRW);
	}

//------------------------------------------------------------------------------
// issue a start condition for i2c address with read/write bit
	uint8_t start(uint8_t channel, uint8_t addressRW, bool isFirst=true) {
		if(isFirst) {
			setSDA(false);
		}
		if (channel == 0) {
			return start(addressRW);
		}
		return rest().start(channel - 1, addressRW, false);
	}

//------------------------------------------------------------------------------
// issue a stop condition
	void stop(uint8_t channel, bool isFirst=true) {
		if (channel == 0) {
			stop();
		} else {
			rest().stop(channel - 1, false);
		}
		if(isFirst) {
			setSDA(true);
			delay();
		}
	}

	//------------------------------------------------------------------------------
	// write byte and return true for Ack or false for Nak
	bool write(uint8_t channel, uint8_t value) {
		if (channel == 0) {
		  return write(value);
		}
		return rest().write(channel - 1, value);
	}

	//------------------------------------------------------------------------------
	// read a byte and send Ack if last is false else Nak to terminate read
	uint8_t read(uint8_t channel, bool last) {
		if (channel == 0) {
		  return read(last);
		}
		return rest().read(channel - 1, last);
	}

	void setSDA(bool value) {
		SDA_PIN::setValue(value);
    rest().setSDA(value);
	}
private:
	void delay() {
		_delay_us(I2C_DELAY_USEC);
	}
	uint8_t start(uint8_t addressRW) {
		SCL_PIN::setValue(false);
		return write(addressRW);
	}
  void stop() {
    delay();
    SCL_PIN::setValue(true);
    delay();
  }
	bool write(uint8_t value) {
		for (uint8_t m = 0X80; m != 0; m >>= 1) {
			SDA_PIN::setValue((m & value) != 0);
			delay();
			SCL_PIN::setValue(true);
			delay();
			SCL_PIN::setValue(false);
		}
		// get Ack or Nak
		SDA_PIN::setValue(true);
		SDA_PIN::setDirection(false);
		SCL_PIN::setValue(true);
		delay();
		value = SDA_PIN::getValue();
		SCL_PIN::setValue(false);
		SDA_PIN::setDirection(true);
		return value == 0;
	}
	uint8_t read(bool last) {
		uint8_t b = 0;
		// make sure pullup enabled
		SDA_PIN::setValue(true);
		SDA_PIN::setDirection(false);
		// read byte
		for (uint8_t m = 0x80; m != 0; m >>= 1) {
			SCL_PIN::setValue(true);
			// incoming bit is most stable before falling clock edge
			delay();
			b |= SDA_PIN::getValue() ? m : 0;
			SCL_PIN::setValue(false);
			delay();
		}
		// send Ack or Nak
		SDA_PIN::setDirection(true);
		SDA_PIN::setValue(last);
		SCL_PIN::setValue(true);
		delay();
		SCL_PIN::setValue(false);
		SDA_PIN::setValue(true);
		return b;
	}
};

#endif /* SOFTPOTMANAGER_H_ */
