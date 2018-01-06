/**
  ******************************************************************************
  * @file   Player.h
  * @author  A.E Tec
  * @version V1.0
  * @date    23/4/15
  * @brief   This file provides all the Uctronics MP3 module serial functions.
  ******************************************************************************
    @note
         This driver is for Uctronics MP3 module (http://www.ebay.com/itm/281387900991)
  ******************************************************************************
  * @section  HISTORY
  
    V1.0    Initial version.

  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, ELECHOUSE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 ELECHOUSE</center></h2>
  ******************************************************************************
  */

#ifndef _Player2_h_
#define _Player2_h_

#include "Arduino.h"
#include "SoftwareSerial.h"

#define MAX_VOLUME		(0x1E)
#define READ_CMD_SIZE	(10)
#define READ_MILLIS		(1000)
#define WAITING_SECONDS	(1000)

typedef enum {
	EQ_NORMAL = 0,
	EQ_ROCK,
	EQ_POP,
	EQ_JAZZ,
	EQ_CLASSICAL,
	EQ_COUNT
} eq_type_t;

typedef enum {
	REPEAT_ALL = 0,
	REPEAT_FOLDER,
	REPEAT_ONE,
	REPEAT_RAM,
	REPEAT_STOP,
	REPEAT_COUNT
} repeat_mode_t;

class Player2 : public SoftwareSerial {
	public:
		Player2(uint8_t receive_pin, uint8_t transmit_pin);
		
		void play();
		void pause();
		void stop();
		void next();
		void prev(); 
		void volumeUp();
		void volumeDown();
		
		void setVolume(uint8_t volume);
		void setEqualizer(eq_type_t eq_type);
		
		void reset();
		void setPower(bool is_on);
		
		void playTrack(uint16_t track_index);
		void playTrack(uint8_t folder_index, uint8_t track_index);
		
		void repeatTrack(uint16_t track_index);
		void repeatTrack(uint8_t folder_index, uint8_t track_index);
		
		void repeatTracks(repeat_mode_t repeat_mode);
		void repeatShuffledTracks();
		
		void repeatingTrack(bool is_repeated = true);
		void repeatFolder(uint8_t folder_index);
		
		int8_t waitForTrack(uint16_t timeout = WAITING_SECONDS);
		int8_t waitForTracks(uint16_t tracks_count, uint16_t timeout = WAITING_SECONDS);
		
		bool isPlaying();
		uint8_t getVolume();
		eq_type_t getEqualizer();
		uint16_t getTracksCount();
		
	private:
		void writeCommandStart(uint8_t len, uint8_t id);
		void writeCommandEnd();
		void writeCommand(uint8_t id);
		void writeCommand(uint8_t id, uint8_t a);
		void writeCommand(uint8_t id, uint8_t a, uint8_t b);
		uint16_t readCommand(uint32_t timeout = READ_MILLIS);
		uint16_t readToBuffer(char * buff, uint16_t len, uint32_t timeout);
};

#endif
