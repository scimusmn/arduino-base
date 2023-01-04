/* @INCLUDE */
#include <avr/io.h>
#include <avr/interrupt.h>

/* @HEADER */
#ifndef SMM_NO_SWITCH
class PcInterruptPort;
class Switch;

class PcInterruptManager {
	public:
	#if defined(SMM_ARCH_MEGA)
	#ifndef SMM_PCINT_NO_PORTB
	static PcInterruptPort portB;
	#endif
	#ifndef SMM_PCINT_NO_PORTJ
	static PcInterruptPort portJ;
	#endif
	#ifndef SMM_PCINT_NO_PORTK
	static PcInterruptPort portK;
	#endif
	#elif defined(SMM_ARCH_UNO)
	#ifndef SMM_PCINT_NO_PORTB
	static PcInterruptPort portB;
	#endif
	#ifndef SMM_PCINT_NO_PORTC
	static PcInterruptPort portC;
	#endif
	#ifndef SMM_PCINT_NO_PORTD
	static PcInterruptPort portD;
	#endif
	#endif

	static void AddSwitch(int pin, Switch *b);
};


/** @brief base switch class
 *
 * This is a pure virtual class, so you *must* implement a child class before using it. Typical usage
 * looks like this:
 *
 * ```
 * class CounterButton : public smm::Switch {
 *     public:
 *     int count;
 *     CounterButton() : smm::Switch(10), count(0) {}
 *     void onLow() {
 *         // press
 *         count += 1;
 *     }
 *     void onHigh() {
 *         // release, ignore
 *     }
 * } counter;
 * ```
 *
 * Thanks to the auto-registration of the base class's constructor, the `counter` object now is bound
 * to a PCINT interrupt and will immediately respond to changes on Arduino pin 10, with no need
 * to poll.
 *
 * Note that it is good practice to keep your `onLow` and `onHigh` functions as short and simple as possible,
 * as they may interrupt *any* part of your code. If you have performace- or timing-critical sections of code,
 * you can disable interrupts with the `cli()` function and re-enable them with the `sei()` function,
 * but your switches will not be able to respond during these sections of code. (Of course, a polled switch
 * wouldn't be able to respond during said sections either...)
 *
 * As with the SerialController class, the auto-registration system for smm::Switch uses static member variables.
 * This means that simply including this code will result in increased memory usage.
 * If you are not using it and wish to conserve RAM, you can disable it by defining `SMM_NO_SWITCH`
 * before including `smm.h`.
 */
class Switch {
	protected:
	volatile Switch *m_next;
	volatile uint8_t m_mask;
	volatile unsigned long m_debounceTime;
	volatile unsigned long m_lastTime;
	volatile bool m_inverted;
	volatile bool m_isLow;
	friend class PcInterruptPort;

	void addSwitch(Switch *b) {
		if (m_next != nullptr) {
			m_next->addSwitch(b);
		}
		else {
			m_next = b;
		}
	}

	void onChange(int state) {
		if ((millis() - m_lastTime) < m_debounceTime) {
			// still debouncing, ignore
			return;
		}

		bool isLow = state == 0;
		if (m_isLow == isLow) {
			// no change in state, ignore
			return;
		}

		m_isLow = isLow;
		m_lastTime = millis();
		if (m_isLow)
			onLow();
		else
			onHigh();
	}

	public:
	/** @brief (constructor)
	 *
	 * @param pin  The Arduino pin to monitor
	 * @param debounceTime  The time in milliseconds to debounce after a state change
	 */
	Switch(int pin, bool pullUp=true, unsigned long debounceTime=5) {
		m_next = nullptr;
		m_mask = digitalPinToBitMask(pin);
		m_debounceTime = debounceTime;
		m_lastTime = 0;
		m_isLow = false;

		if (pullUp) {
			pinMode(pin, INPUT_PULLUP);
		}
		else {
			pinMode(pin, INPUT);
		}
		PcInterruptManager::AddSwitch(pin, this);
	}

	/** @brief pure virtual function, called when going LOW */
	virtual void onLow() = 0;
	/** @brief pure virtual function, called when going HIGH */
	virtual void onHigh() = 0;
};

class PcInterruptPort {
	protected:
	volatile Switch *m_head;
	volatile uint8_t m_pinMask;
	volatile uint8_t m_previousState;
	volatile uint8_t *m_pcmsk;

	public:
	PcInterruptPort(int index, uint8_t *pcmsk) {
		m_pcmsk = pcmsk;
		cli();
		PCICR |= (1<<index);
		sei();
		m_head = nullptr;
		m_pinMask = 0;
		m_previousState = 0xff;
	}

	void addSwitch(Switch *b) {
		if (m_head == nullptr) {
			m_head = b;
		}
		else {
			m_head->addSwitch(b);
		}
		cli();
		(*m_pcmsk) |= b->m_mask;
		sei();
		m_pinMask |= b->m_mask;
	}

	void onChange(uint8_t state) {
		uint8_t changed = state ^ m_previousState;
		m_previousState = state;

		Switch *b = m_head;
		while (b != nullptr) {
				if (changed & b->m_mask) {
				b->onChange(state & b->m_mask);
			}
			b = b->m_next;
		}
	}
};
/* ifndef SMM_NO_SWITCH */
#endif

/* @IMPLEMENTATION */
#ifndef SMM_NO_SWITCH
/* smm::Switch implementation */
void smm::PcInterruptManager::AddSwitch(int pin, smm::Switch *b) {
	uint8_t port = digitalPinToPort(pin);
	uint16_t oport = portOutputRegister(port);

	#if defined(SMM_ARCH_MEGA)
	if (oport == &PORTB) {
		#ifndef SMM_PCINT_NO_PORTB
		portB.addSwitch(b);
		#endif
	}
	else if (oport == &PORTJ) {
		#ifndef SMM_PCINT_NO_PORTJ
		portJ.addSwitch(b);
		#endif
	}
	else if (oport == &PORTK) {
		#ifndef SMM_PCINT_NO_PORTK
		portK.addSwitch(b);
		#endif
	}
	#elif defined(SMM_ARCH_UNO)
	if (oport == &PORTB) {
		#ifndef SMM_PCINT_NO_PORTB
		portB.addSwitch(b);
		#endif
	}
	else if (oport == &PORTC) {
		#ifndef SMM_PCINT_NO_PORTC
		portC.addSwitch(b);
		#endif
	}
	else if (oport == &PORTD) {
		#ifndef SMM_PCINT_NO_PORTD
		portD.addSwitch(b);
		#endif
	}
	#endif
	#ifndef SMM_ARCH_UNKNOWN
	else {
		Serial.print("\n\n\n\n\n\n\n\n");
		Serial.println("!!!!!!!! WARNING !!!!!!!!");
		Serial.print("No PCINT port for pin ");
		Serial.print(pin);
		Serial.println("; it WILL NOT work as a switch!");
	}
	#else
	Serial.println("\n\n\n\n\n\n\n\n\n\n\n\nThis chipset is currently not supported by smm::Switch.");
	#endif
}

#if defined(SMM_ARCH_MEGA)
#ifndef SMM_PCINT_NO_PORTB
smm::PcInterruptPort smm::PcInterruptManager::portB(PCIE0, &PCMSK0);
ISR(PCINT0_vect) {
	uint8_t pins = PINB;
	smm::PcInterruptManager::portB.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTJ
smm::PcInterruptPort smm::PcInterruptManager::portJ(PCIE1, &PCMSK1);
ISR(PCINT1_vect) {
	uint8_t pins = PINJ;
	smm::PcInterruptManager::portJ.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTK
smm::PcInterruptPort smm::PcInterruptManager::portK(PCIE2, &PCMSK2);
ISR(PCINT2_vect) {
	uint8_t pins = PINK;
	smm::PcInterruptManager::portK.onChange(pins);
}
#endif

#elif defined(SMM_ARCH_UNO)
#ifndef SMM_PCINT_NO_PORTB
smm::PcInterruptPort smm::PcInterruptManager::portB(PCIE0, &PCMSK0);
ISR(PCINT0_vect) {
	uint8_t pins = PINB;
	smm::PcInterruptManager::portB.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTC
smm::PcInterruptPort smm::PcInterruptManager::portC(PCIE1, &PCMSK1);
ISR(PCINT1_vect) {
	uint8_t pins = PINC;
	smm::PcInterruptManager::portC.onChange(pins);
}
#endif
#ifndef SMM_PCINT_NO_PORTD
smm::PcInterruptPort smm::PcInterruptManager::portD(PCIE2, &PCMSK2);
ISR(PCINT2_vect) {
	uint8_t pins = PIND;
	smm::PcInterruptManager::portD.onChange(pins);
}
#endif

#endif
/* ifndef SMM_NO_SWITCH */
#endif

/* @END */
