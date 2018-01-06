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

#ifndef _ROBOT_H_
#define _ROBOT_H_

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

//{ Consts And Defines

    #define IR_TICK_MS                (833) // 1/1200 second clock (~.833ms)
    #define IR_START_MS              (IR_TICK_MS * 8)
    #define IR_HIGH_MS               (IR_TICK_MS * 4)
    #define IR_LOW_MS                (IR_TICK_MS * 1)
    
    typedef enum command_e
    {
         CMD_TURN_RIGHT               = 0x80,
         CMD_RIGHT_ARM_UP           = 0x81,
         CMD_RIGHT_ARM_OUT         = 0x82,
         CMD_TILT_BODY_RIGHT       = 0x83,
         CMD_RIGHT_ARM_DOWN      = 0x84,
         CMD_RIGHT_ARM_IN            = 0x85,
         CMD_WALK_FORWARD         = 0x86,
         CMD_WALK_BACKWARD       = 0x87,
         CMD_TURN_LEFT                 = 0x88,
         CMD_LEFT_ARM_UP             = 0x89,
         CMD_LEFT_ARM_OUT           = 0x8A,
         CMD_TILT_BODY_LEFT         = 0x8B,
         CMD_LEFT_ARM_DOWN        = 0x8C,
         CMD_LEFT_ARM_IN              = 0x8D,
         CMD_STOP                          = 0x8E,
         CMD_RIGHT_TURN_STEP       = 0xA0,
         CMD_RIGHT_HAND_THUMP    = 0xA1,
         CMD_RIGHT_HAND_THROW   = 0xA2,
         CMD_SLEEP                         = 0xA3,
         CMD_RIGHT_HAND_PICK_UP  = 0xA4,
         CMD_LEAN_BACKWARD         = 0xA5,
         CMD_FORWARD_STEP           = 0xA6,
         CMD_BACKWARD_STEP         = 0xA7,
         CMD_LEFT_TURN_STEP         = 0xA8,
         CMD_LEFT_HAND_THUMP      = 0xA9,
         CMD_LEFT_HAND_THROW     = 0xAA,
         CMD_LISTEN                        = 0xAB,
         CMD_LEFT_HAND_PICK_UP    = 0xAC,
         CMD_LEAN_FORWARD           = 0xAD,
         CMD_RESET                         = 0xAE,
         CMD_EXECUTE                     = 0xB0,
         CMD_WAKEUP                      = 0xB1,
         CMD_RIGHT                         = 0xB2,
         CMD_LEFT                           = 0xB3,
         CMD_SONIC                         = 0xB4,
         CMD_RIGHT_HAND_STRIKE3  = 0xC0,
         CMD_RIGHT_HAND_SWEEP    = 0xC1,
         CMD_BURP                          = 0xC2,
         CMD_RIGHT_HAND_STRIKE2  = 0xC3,
         CMD_HIGH5                        = 0xC4,
         CMD_RIGHT_HAND_STRIKE1  = 0xC5,
         CMD_BULLDOZER                 = 0xC6,
         CMD_OOPS                          = 0xC7,
         CMD_LEFT_HAND_STRIKE3     = 0xC8,
         CMD_LEFT_HAND_SWEEP       = 0xC9,
         CMD_WHISTLE                     = 0xCA,
         CMD_LEFT_HAND_STRIKE2     = 0xCB,
         CMD_TALKBACK                    = 0xCC,
         CMD_LEFT_HAND_STRIKE1     = 0xCD,
         CMD_ROAR                          = 0xCE,
         CMD_ALLDEMO                     = 0xD0,
         CMD_POWER_OFF                 = 0xD1,
         CMD_DEMO1                        = 0xD2,
         CMD_DEMO2                        = 0xD3,
         CMD_DANCE                        = 0xD4,
         CMD_FEET_SHUFFLE              = 0xF6,
         CMD_NOTHING                     = 0xFB,
         CMD_RAISE_ARM_THROW      = 0xFC,
         CMD_KARATE_CHOP              = 0xD6,
	} command_t;
    
//}

class Robot
{
    public:
        Robot(uint8_t irPin);
        void sendCommand(command_t command);
    private:
        uint8_t * _irPort;
        uint8_t  _irMask;
};

#endif
