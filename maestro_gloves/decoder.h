#ifndef DECODER_H
#define DECODER_H

/*

	return n strings until \n

*/

void excecut_command(char *args[], uint8_t argc) {
	uint32_t pos = 0;
	
	if (argc <= 0)
		return;
	
	switch (args[0][0]) {
		case 'L':
		case 'R':
			if (argc <= 1)
				return;
			pos = atoi(args[1]);
			if (pos < MIN_POS || pos > MAX_POS)
				return;
			if (args[0][0] == 'L')
				move_left(pos);
			else
				move_right(pos);
		break;
		case 'K':
			if (argc <= 1)
				return;
			mask = atoi(args[1]);
			if (mask < 0 || mask > 255)
				return;
			(pos);
		break;
	}
}

char *commands[] = {
	"L %d\n",
	"R %d\n",
	"K %d\n",
	"C\n",
	"P %d\n",
}

bool decode_integer(char c, uint32_t * number) {
	if (c >= '0' && c <= '9') {
		*number *= 10;
		*number += c - '0';
		return false;
	}
	return true;
}

void decode_command(char * command_buffer, uint8_t command_buffer_size) {
	switch (command_buffer[0]) {
		case 'C':
			arguments_count = 0;
		break;
		case 'L':
			arguments_count = 1;
			// sscanf_P(command_buffer, PSTR("%d"), &num);
		break;
		case 'R':
			arguments_count = 1;
		break;
		case 'K':
			arguments_count = 1;
		break;
	}
}

void decode_command(char c) {
	static char command_buffer[MAX_COMAND_SIZE];
	static uint8_t command_buffer_size = 0;
	if (c == END_COMMAND) {
		
		command_buffer_size = 0;
	}
	else if (command_buffer_size >= (MAX_COMAND_SIZE - 1)) {
		command_buffer_size = 0;
	}
	else {
		command_buffer_size[command_buffer_size] = c;
		command_buffer_size++;
	}
}

void decode_command_id(char c) {
	if		(c == 'C') {
		arguments_count = 0;
	}
	else if	(c == 'L') {
		arguments_count = 1;
	}
	else if	(c == 'R') {
		arguments_count = 1;
	}
	else if	(c == 'K') {
		arguments_count = 1;
	}
	// decoder state...
}

void decoder_feed(char c) {
	static uint8_t command_state = 0;
	static uint8_t command_tag = 0;
	static char command_buffer[MAX_COMAND_SIZE];
	static uint8_t command_buffer_size = 0;
	switch (c) {
		case ' ':
			command_arguments++;
		break;
		case '\n':
			command_state = 0;
		break;
	}
	
	if		(c == 'C') {
		arguments_count = 0;
	}
	else if	(c == 'L') {
		arguments_count = 1;
	}
	else if	(c == 'R') {
		arguments_count = 1;
	}
	else if	(c == 'K') {
		arguments_count = 1;
	}
	else if	(c == ' ') {
		if (arguments_count) arguments_count--;
		//else ...
	}
	else if	(c == '\n') {
		
	}
	else if	(c >= '0' && c <= '9') {
		
	}
	else {
		DEBUG_PRINT("decoder error: ");
		DEBUG_PRINTLN(c);
	}
}


#endif /* DECODER_H */
