#pragma once
#include <Arduino.h>

class TimerFF {
public:
	TimerFF(uint16_t period_ms, bool enable = true) {
		setPeriod(period_ms);
		en = enable;
	}

	void setPeriod(uint16_t period_ms) {
		prd = period_ms;
	}

	uint16_t getPeriod() {
		return prd;
	}

	void disable() {
		en = false;
	}

	void enable() {
		en = true;
		tmr = millis();
	}

	bool ready() {
		if (en && millis() - tmr >= prd) {
			tmr = millis();
			return true;
		}
		return false;
	}
private:
	uint32_t tmr = 0;
	uint16_t prd = 0;
	bool en = true;
};
