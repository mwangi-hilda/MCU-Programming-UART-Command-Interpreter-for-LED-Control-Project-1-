/*
 * UARTCommProject1.cpp
 *
 * Created: 19/06/2025 11:30:59
 * Author : Hilda
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>  
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#define LED_DDR DDRA
#define LED_PORT PORTA
#define LED_TERMINAL PA0

// --- UART Buffer ---
#define RX_BUFFER_SIZE 32
volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t new_message_flag = 0;

// --- Function Prototypes ---
void uart0_init(uint32_t baud_rate);
void uart0_transmit(char data);
void uart0_send_string(const char *str);

// --- Interrupt Service Routine for USART0 Receive Complete ---
ISR(USART0_RX_vect) {
	char received_char = UDR0; // Read the received character

	if (received_char == '\r' || received_char == '\n') { // End of message (Carriage Return or Newline)
		if (rx_index > 0) { // Only process if there's actual data
			rx_buffer[rx_index] = '\0'; // Null-terminate the string
			new_message_flag = 1;       // Set flag to process in main loop
			rx_index = 0;               // Reset index for next message
		}
		} else if (rx_index < (RX_BUFFER_SIZE - 1)) { // Store character if buffer not full
		rx_buffer[rx_index++] = received_char;
	}
}

int main(void) {
	// --- Configure LED Pin as Output ---
	LED_DDR |= (1 << LED_TERMINAL); // Set PD0 as output
	LED_PORT &= ~(1 << LED_TERMINAL); // Ensure LED is off initially

	// --- Initialize UART0 ---
	uart0_init(9600); // Initialize UART0 at 9600 baud rate

	// Enable global interrupts
	sei(); // Set Global Interrupt Enable bit

	uart0_send_string("MCU Ready! Send commands.\r\n");

	while (1) {
		// --- Main loop to process received commands ---
		if (new_message_flag) {
			new_message_flag = 0; // Clear the flag

			// Compare received message
			if (strcmp((const char*)rx_buffer, "Light on") == 0) {
				LED_PORT |= (1 << LED_TERMINAL); // Turn on LED1
				uart0_send_string("Light 1 on\r\n"); // Send confirmation back
			} else if (strcmp((const char*)rx_buffer, "Light off") == 0) {
				LED_PORT &= ~(1 << LED_TERMINAL); // Turn off LED1
				uart0_send_string("Light off\r\n"); // Send confirmation back
			} else {
				uart0_send_string("Unknown command: ");
				uart0_send_string((const char*)rx_buffer);
				uart0_send_string("\r\n");
			}
			// Clear buffer after processing
			memset((void*)rx_buffer, 0, RX_BUFFER_SIZE);
		}
	}
}

// --- Function Definitions ---

void uart0_init(uint32_t baud_rate) {
	// Calculate UBRR value for desired baud rate
	// Using U2X0 = 1 (Double Speed Mode) for better accuracy at 9600 with 16MHz
	uint16_t ubrr_value = (uint16_t)(F_CPU / (8 * baud_rate)) - 1;

	// Set baud rate (UBRR0H and UBRR0L)
	UBRR0H = (unsigned char)(ubrr_value >> 8);
	UBRR0L = (unsigned char)ubrr_value;

	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Enable RX complete interrupt
	UCSR0B |= (1 << RXCIE0);

	// Set frame format: 8 data bits, 1 stop bit, No Parity (Asynchronous)
	// UMSEL01:0 = 00 (Asynchronous)
	// UPM01:0 = 00 (No Parity)
	// USBS0 = 0 (1 Stop Bit)
	// UCSZ01:0 = 11 (8 Data Bits) - UCSZ02 is in UCSR0B
	UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) | // Asynchronous mode
	(0 << UPM01)  | (0 << UPM00)  | // No Parity
	(0 << USBS0)  |                 // 1 Stop Bit
	(1 << UCSZ01) | (1 << UCSZ00);  // 8 Data Bits

	UCSR0B |= (0 << UCSZ02); // Ensure UCSZ02 is 0 for 8-bit data (as UCSZ01:0 are 11)

	// Set U2X0 bit for double speed
	UCSR0A = (1 << U2X0); // Double speed for better baud rate accuracy
}

void uart0_transmit(char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)))
	;
	// Put data into buffer, sends the data
	UDR0 = data;
}

void uart0_send_string(const char *str) {
	while (*str) {
		uart0_transmit(*str++);
	}
}