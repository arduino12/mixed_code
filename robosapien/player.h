/*********************************** LICENCE **********************************\
| Copyright (c) 2013, A.E. TEC                                                                                         |
| All rights reserved.                                                                                                      |
|                                                                                                                                 |
| Redistribution and use in source and binary forms, with or without                                     |
| modification, are permitted provided that the following conditions are met:                         |
|                                                                                                                                 |
| * Redistributions of source code must retain the above copyright notice,                             |
|   this list of conditions and the following disclaimer.                                                        |
| * Redistributions in binary form must reproduce the above copyright notice,                        |
|   this list of conditions and the following disclaimer in the documentation                           |
|   and/or other materials provided with the distribution.                                                     |
|                                                                                                                                 |
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"    |
| AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE         |
| IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE     |
| ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE       |
| LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR                       |
| CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF              |
| SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS            |
| INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN         |
| CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)            |
| ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         |
| POSSIBILITY OF SUCH DAMAGE.                                                                                    |
\*****************************************************************************/
/************************************ USES ************************************\
| Library to control a WTV020-SD-16P module to play voices from an Arduino board.                |
\*****************************************************************************/

#ifndef _PLAYER_H_
#define _PLAYER_H_

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

//{ Consts And Defines

#define PLAY_PAUSE  (0xFFFE)
#define STOP 	       (0xFFFF)
#define VOLUME_MIN (0xFFF0)

typedef enum audio_e
{
    MAKRENIM = 1,
    AIN_AIN_O_A,
    AIN_AIN_O_B,
    AIN_AIN_END_A,
    AIN_AIN_END_B,
    SHAT_TASH,
    MESOGALOT,
    LO_RELEVANTI,
    SHELA_METZUINET,
    IMPORT_AGAMIM,
    ALE_VEATZLACH,
    RESEND_NESSI,
    MA_KORE,
    THROW_IT,
    HCREPS_BINARY,
    RONI_FACHA_A,
    RONI_FACHA_B,
    NIPAGESH_BATZAD,
    SHUTDOWN,
    SHLOMI,
    AD_MATAY,
    KAMA_ATA,
    HALAMAT_TIZ,
    SHEL_HMAFHCIDOT,
    FOLLOW_INSTRUCTION,
    MASTER_BUILTER,
    AGAMITZI_A,
    AGAMITZI_B,
    ALE_AGAMIM_A,
    ALE_AGAMIM_B,
    SACHBAK_NESTEA,
    DAHOKI,
    SONG_1,
    SONG_2,
    SONG_3,
    SONG_4,
    
    /*
    AGAMITZI_A = 1,
    AGAMITZI_B,
    AIN_AIN_O_A,
    AIN_AIN_O_B,
    AIN_AIN_END_A,
    AIN_AIN_END_B,
    ALE_AGAMIM_A,
    ALE_AGAMIM_B,
    ALE_VEATZLACH,
    FOLLOW_INSTRUCTION,
    HCREPS_BINARY,
    IMPORT_AGAMIM,
    LO_RELEVANTI,
    MA_KORE,
    MAKRENIM,
    MESOGALOT,
    NIPAGESH_BATZAD,
    RAUTER,
    RESEND_NESSI,
    RONI_FACHA_A,
    RONI_FACHA_B,
    SACHBAK_NESTEA,
    SHAT_TASH,
    SHELA_METZUINET,
    SHINOI_BALUZ,
    SHUTDOWN,
    SONG_1,
    SONG_2,
    SONG_3,
    SONG_4,
    THROW_IT,*/
} audio_t;

//}

class Player
{
    public:
        Player(uint8_t resetPin, uint8_t clockPin, uint8_t dataPin, uint8_t busyPin);
        void reset();
        void playAll(uint16_t id);
        void play(uint16_t id);
        void stop();
        void pause();
        void volume(uint8_t vol);
    private:
        void sendCommand(uint16_t command);
        uint8_t _resetPin, _clockPin, _dataPin, _busyPin;
};

#endif
