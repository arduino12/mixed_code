#ifndef SERIAL_MENU_H
#define SERIAL_MENU_H

#include <common.h>

#ifndef SERIAL_MENU_CMD_BUF_SIZE
	/* max amount of bytes for a command */
	#define SERIAL_MENU_CMD_BUF_SIZE			(254)
#endif
#ifndef SERIAL_MENU_CMD_TIMEOUT_MS
	/* max amount of milliseconds for command buffer to grow */
	#define SERIAL_MENU_CMD_TIMEOUT_MS			(150)
#endif


struct {
	char buffer[SERIAL_MENU_CMD_BUF_SIZE];
	uint8_t buffer_index = 0;
	uint32_t timeout = 0;
} command;


/* reads a number from the command buffer */
uint16_t _serial_menu_read_16(uint16_t number) {
	uint16_t i = number;
	sscanf(command.buffer + 2, "%d", &i);
	return i;
}

/* must be called repeatedly from loop, this is our busy loop */
bool serial_menu_update() {
	uint32_t cur_millis = millis();
	uint16_t i;

	/* is it the time for a screen saver? */
	if (cur_millis > command.timeout)
		if (command.buffer_index)
			command.buffer_index = 0;

	/* got a command? */
	if (Serial.available()) {
		if (!command.buffer_index)
			command.timeout = cur_millis + SERIAL_MENU_CMD_TIMEOUT_MS;

		command.buffer_index += Serial.readBytes(command.buffer + command.buffer_index,
			sizeof(command.buffer) - command.buffer_index);

		i = command.buffer[command.buffer_index - 1];
		if (command.buffer_index >= 2 && i == '\n' || i == '\r') {
			i = command.buffer[command.buffer_index - 2];
			i = ((i == '\n' || i == '\r') ? 3 : 2);
			while (i--)
				command.buffer[command.buffer_index - i] = '\0';
			command.buffer_index = 0;
			return true;
		}
	}

	return false;
}

#endif /* SERIAL_MENU_H */
