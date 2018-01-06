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

#include "player.h"

Player::Player(uint8_t resetPin, uint8_t clockPin, uint8_t dataPin, uint8_t busyPin)
{
    _resetPin = resetPin;
    _clockPin = clockPin;
    _dataPin  = dataPin;
    _busyPin  = busyPin;

    pinMode(_resetPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin , OUTPUT);
    pinMode(_busyPin , INPUT);
}

void Player::reset()
{
    digitalWrite(_clockPin, HIGH);
    digitalWrite(_resetPin, LOW);
    delay(5);
    digitalWrite(_resetPin, HIGH);
    delay(1200);
}

void Player::playAll(uint16_t id)
{  
    sendCommand(id);
    delay(1);
    while(digitalRead(_busyPin));
}

void Player::play(uint16_t id)
{
    sendCommand(id);
}

void Player::stop()
{
    sendCommand(STOP);
}

void Player::pause()
{
    sendCommand(PLAY_PAUSE);
}

void Player::volume(uint8_t vol)
{
    if (vol > 7) vol = 7;
    sendCommand(VOLUME_MIN | vol);
}

void Player::sendCommand(uint16_t command)
{
    digitalWrite(_clockPin, LOW);
    delay(2);
    for (uint8_t i = 0; i < 16; i++)
    {
        digitalWrite(_dataPin, (command & 0x8000 ? HIGH : LOW));
        digitalWrite(_clockPin, HIGH);
        delayMicroseconds(100);
        if (i < 15)
            digitalWrite(_clockPin, LOW);
        delayMicroseconds(100);
        command <<= 1;
    }
    delay(500);
}