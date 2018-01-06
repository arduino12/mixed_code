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
| Library to control the robosapian robot by simulating ir commands from an Arduino board.     |
\*****************************************************************************/

#include "robot.h"

#define PLAY_PAUSE                    (0xFFFE)
#define PLAY_PAUSE STOP            (0xFFFF)
#define PLAY_PAUSE VOLUME_MIN (0xFFF0)

Robot::Robot(uint8_t irPin) {
	_irPort = (uint8_t *)portOutputRegister(digitalPinToPort(irPin));
	_irMask = digitalPinToBitMask(irPin);
	pinMode(irPin, OUTPUT);
    *_irPort |= _irMask;
}

void Robot::sendCommand(command_t command) {
    *_irPort &= ~_irMask;
    delayMicroseconds(IR_START_MS);
    for (uint8_t i = 0x80; i > 0; i >>= 0x01) {
        *_irPort |= _irMask;
        delayMicroseconds((command & i) ? IR_HIGH_MS : IR_LOW_MS);
        *_irPort &= ~_irMask;
        delayMicroseconds(IR_TICK_MS);
    }
    *_irPort |= _irMask;
    delayMicroseconds(10000);
}
