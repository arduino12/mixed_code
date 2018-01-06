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
| Make robosapian be a segel by moving and speaking like them                                          |
\*****************************************************************************/
/*********************************** CIRCUIT **********************************\
| Robosapian:                                                                                                               |
|    * IR_IN to pin 3 via resistor and diode                                                                        |
|    * Disconnect original mic and speaker                                                                         |
|    * VDD (6v) to VCC (5v) via diode                                                                                |
|    * GND to GND                                                                                                          |
|                                                                                                                                 |
| IR Sensor:                                                                                                                  |
|    * OUT to pin 2                                                                                                         |
|    * VCC to VCC                                                                                                           |
|    * GND to GND                                                                                                          |
|                                                                                                                                 |
| Voice Module:                                                                                                             |
|    * RST_PIN to pin 5                                                                                                   |
|    * CLK_PIN to pin 4                                                                                                   |
|    * DAT_PIN to pin 6                                                                                                   |
|    * BUS_PIN to pin 7                                                                                                   |
|    * VCC to VCC                                                                                                           |
|    * GND to GND                                                                                                          |
|                                                                                                                                 |
| Amplifier:                                                                                                                   |
|    * R_IN to AUDIO_OUT (from Voice Module)                                                                  |
|    * L_IN to AUDIO_OUT (from Voice Module)                                                                  |
|    * VCC to VDD (6v)                                                                                                    |
|    * GND to GND                                                                                                          |
\*****************************************************************************/

//{ Includes

    #include <MyRemote.h>
    #include "remote.h"
    #include "player.h"
    #include "robot.h"
    
//}

//{ Consts And Defines

    //#define DEBUG
    //#define DEBUG_ROBOT
    //#define DEBUG_PLAYER
    //#define DEBUG_REMOTE
    
    #if defined DEBUG_ROBOT || defined DEBUG_PLAYER || defined DEBUG_REMOTE || defined DEBUG 
        #define SERIAL_BAUDRATE   (115200)
        #define SERIAL_TIMEOUT     (200)
    #endif
    
    #define IR_SENSOR_PIN      (2)
    #define IR_ROBOT_PIN        (3)
    #define RST_PIN                 (5)
    #define CLK_PIN                 (4)
    #define DAT_PIN                (6)
    #define BUS_PIN                (7)    

//}

//{ Veraibles

    Player player(RST_PIN, CLK_PIN, DAT_PIN, BUS_PIN);
    Robot robot(IR_ROBOT_PIN);
    uint8_t remoteKey = KEY_NONE;
    
//}

//{ Segel

void makrenim() 
{
    //if (playAll(MAKRENIM)) goto l_exit;
    player.play(MAKRENIM);
    if(sendCommandsLoop(CMD_RIGHT_ARM_OUT, CMD_LEFT_ARM_OUT, 10, 10)) goto l_exit;
    if(sendCommandsLoop(CMD_RIGHT_ARM_UP, CMD_LEFT_ARM_UP, 40, 1)) goto l_exit;

    for (uint8_t count =0; count < 4; count++)
    {
        robot.sendCommand(CMD_RIGHT_ARM_IN);
        robot.sendCommand(CMD_LEFT_ARM_OUT);
        if (waitMs(500)) goto l_exit;
        robot.sendCommand(CMD_RIGHT_ARM_OUT);
        robot.sendCommand(CMD_LEFT_ARM_IN);
        if (waitMs(500)) goto l_exit;
    }

l_exit:
    sendCommandsLoop(CMD_RIGHT_ARM_DOWN, CMD_LEFT_ARM_DOWN, 4, 1);
    sendCommandsLoop(CMD_RIGHT_ARM_IN, CMD_LEFT_ARM_IN, 10, 1);
}

void ain_ain_o()
{
    player.play(AIN_AIN_O_A);
    if (sendCommand(CMD_FORWARD_STEP, 2700)) goto l_exit;
    player.play(AIN_AIN_O_B);
    if (sendCommand(CMD_HIGH5, 2700)) goto l_exit;
    
l_exit:
    return;
}

void ain_ain_end()
{
    player.play(AIN_AIN_END_A);
    if (sendCommand(CMD_OOPS, 2700)) goto l_exit;
    if (playAll(AIN_AIN_END_B)) goto l_exit;

l_exit:
    return;
}

void shat_tash()
{
    if (sendCommand(CMD_LEAN_BACKWARD, 2000)) goto l_exit;
    player.play(SHAT_TASH);
    if (sendCommand(CMD_LEAN_FORWARD, 2000)) goto l_exit;
    
l_exit:
    return;
}

void mesogalot()
{
    if (sendCommand(CMD_LEFT_TURN_STEP, 4000)) goto l_exit;
    if (playAll(MESOGALOT)) goto l_exit;

l_exit:
    return;
}

void lo_relevanti()
{
    player.play(LO_RELEVANTI);
    if (sendCommand(CMD_LEFT_HAND_THUMP, 4000)) goto l_exit;

l_exit:
    return;
}

//}

//{ Nessi

void shela_metzuinet()
{
    player.play(SHELA_METZUINET);
    if (sendCommand(CMD_RIGHT_HAND_STRIKE3, 4000)) goto l_exit;
    
l_exit:
    return;
}

void import_agamim()
{
    player.play(IMPORT_AGAMIM);
    if(sendCommandsLoop(CMD_RIGHT_ARM_OUT, CMD_LEFT_ARM_OUT, 10, 5)) goto l_exit;
    if (sendCommand(CMD_LEAN_BACKWARD, 2000)) goto l_exit;
    
l_exit:
    sendCommand(CMD_LEAN_FORWARD, 2000);
    sendCommandsLoop(CMD_RIGHT_ARM_IN, CMD_LEFT_ARM_IN, 5, 20);
}

void ale_veatzlach()
{
    player.play(ALE_VEATZLACH);
    if (sendCommand(CMD_RIGHT_TURN_STEP, 4000)) goto l_exit;
    
l_exit:
    return;
}

void resend_nessi()
{
    player.play(RESEND_NESSI);
    if (sendCommand(CMD_RIGHT_HAND_THUMP, 4000)) goto l_exit;
    
l_exit:
    return;
}
//}

//{ Martzim

void ma_kore()
{
    player.play(MA_KORE);
    if (sendCommand(CMD_LEFT_HAND_THROW, 4000)) goto l_exit;
    
l_exit:
    return;
}

void throw_it()
{
    player.play(THROW_IT);
    waitMs(1000);
    if (sendCommand(CMD_RIGHT_HAND_THROW, 4000)) goto l_exit;
    
l_exit:
    return;
}

void hcreps_binary()
{
    player.play(HCREPS_BINARY);
    if (sendCommand(CMD_KARATE_CHOP, 4000)) goto l_exit;
    
l_exit:
    return;
}

//}

//{ Hanichim

void roni_facha()
{
    player.play(RONI_FACHA_A);
    if (sendCommandLoop(CMD_TILT_BODY_RIGHT, 20, 10)) goto l_exit;
    if (sendCommand(CMD_TILT_BODY_LEFT, 1500)) goto l_exit;
    if (playAll(RONI_FACHA_B)) goto l_exit;
    
l_exit:
    return;
}

void nipagesh_batzad()
{
    player.play(NIPAGESH_BATZAD);
    if (sendCommand(CMD_RIGHT_HAND_STRIKE2, 3000)) goto l_exit;
    
l_exit:
    return;
}

void shutdown()
{
    if(sendCommandsLoop(CMD_RIGHT_ARM_UP, CMD_LEFT_ARM_UP, 40, 1)) goto l_exit;
    player.play(SHUTDOWN);
    if (sendCommand(CMD_RIGHT_HAND_THUMP, 4000)) goto l_exit;
    
l_exit:
    sendCommandsLoop(CMD_RIGHT_ARM_DOWN, CMD_LEFT_ARM_DOWN, 4, 1);
    return;
}

void shlomi()
{
    player.play(SHLOMI);
    if (sendCommand(CMD_RIGHT_HAND_THROW, 4000)) goto l_exit;
    
l_exit:
    return;
}

void ad_matay()
{
    player.play(AD_MATAY);
    if (sendCommand(CMD_TILT_BODY_RIGHT, 500)) goto l_exit;
    if (sendCommand(CMD_TILT_BODY_LEFT, 500)) goto l_exit;
    if (sendCommand(CMD_TILT_BODY_RIGHT, 500)) goto l_exit;
    if (sendCommand(CMD_TILT_BODY_LEFT, 500)) goto l_exit;
l_exit:
    return;
}

void kama_ata()
{
    if (sendCommand(CMD_LEAN_BACKWARD, 800)) goto l_exit;
    player.play(KAMA_ATA);
    if(sendCommandsLoop(CMD_RIGHT_ARM_UP, CMD_LEFT_ARM_UP, 40, 1)) goto l_exit;
    
l_exit:
    sendCommand(CMD_LEAN_FORWARD, 800);
    sendCommandsLoop(CMD_RIGHT_ARM_DOWN, CMD_LEFT_ARM_DOWN, 4, 1);
    return;
}

void halamat_tiz()
{
    player.play(HALAMAT_TIZ);
    if (sendCommand(CMD_RIGHT_ARM_OUT, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_IN, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_OUT, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_IN, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_OUT, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_IN, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_OUT, 500)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_IN, 500)) goto l_exit;
l_exit:
    return;
}

void shel_hmafhcidot()
{
    player.play(SHEL_HMAFHCIDOT);
    if (sendCommand(CMD_BULLDOZER, 5000)) goto l_exit;
    
l_exit:
    return;
}
    
//}

//{ Others

void follow_instruction()
{
    player.play(FOLLOW_INSTRUCTION);
    if (sendCommand(CMD_BACKWARD_STEP, 4000)) goto l_exit;
    
l_exit:
    return;
}

void agamitzi()
{
    player.play(AGAMITZI_A);
    if (waitMs(800)) goto l_exit;
    if (sendCommand(CMD_LEFT_TURN_STEP, 4000)) goto l_exit;
    player.play(AGAMITZI_B);
    if (sendCommand(CMD_BURP, 4000)) goto l_exit;
    
l_exit:
    return;
}

void ale_agamim()
{
    player.play(ALE_AGAMIM_A);
    if (sendCommand(CMD_ROAR, 4000)) goto l_exit;
    player.play(ALE_AGAMIM_B);
    if (sendCommand(CMD_FEET_SHUFFLE, 4000)) goto l_exit;
    
l_exit:
    return;
}

void sachbak_nestea()
{
    player.play(SACHBAK_NESTEA);
    if (sendCommand(CMD_RIGHT_ARM_UP, 700)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_UP, 700)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_DOWN, 200)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_DOWN, 200)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_UP, 700)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_UP, 700)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_DOWN, 200)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_DOWN, 200)) goto l_exit;
    if (sendCommand(CMD_RIGHT_ARM_UP, 700)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_UP, 700)) goto l_exit;
l_exit:
    if (sendCommand(CMD_RIGHT_ARM_DOWN, 700)) goto l_exit;
    if (sendCommand(CMD_LEFT_ARM_DOWN, 700)) goto l_exit;
    return;
}

void dahoki()
{
    player.play(DAHOKI);
    if (sendCommand(CMD_HIGH5, 3000)) goto l_exit;
    if (sendCommand(CMD_RAISE_ARM_THROW, 3000)) goto l_exit;
    if (sendCommand(CMD_HIGH5, 3000)) goto l_exit;
    if (sendCommand(CMD_RAISE_ARM_THROW, 3000)) goto l_exit;
l_exit:
    return;
}

//}

//{ Songs

void song_1()
{
    player.play(SONG_1);
    if (waitMs(10000)) goto l_exit;
    robot.sendCommand(CMD_ALLDEMO);

l_exit:
    return;
}

void song_2()
{
    player.play(SONG_2);
    if (waitMs(10000)) goto l_exit;
    robot.sendCommand(CMD_DEMO1);

l_exit:
    return;
}

void song_3()
{
    player.play(SONG_3);
    if (waitMs(10000)) goto l_exit;
    robot.sendCommand(CMD_DEMO2);

l_exit:
    return;
}

void song_4()
{
    player.play(SONG_4);
    if (waitMs(10000)) goto l_exit;
    robot.sendCommand(CMD_DANCE);

l_exit:
    return;
}

//}

//{ Main and Loop

void setup()
{
    delay(1000);
    player.reset();
    delay(1000);
    player.volume(5);
    irBegin(IR_SENSOR_PIN);
    
#if defined DEBUG_ROBOT || defined DEBUG_PLAYER || defined DEBUG_REMOTE || defined DEBUG 
    Serial.begin(SERIAL_BAUDRATE);
    Serial.setTimeout(SERIAL_TIMEOUT); // Using Serial.readBytesUntil()
    Serial.println("Start!");
#endif

#ifdef DEBUG_ROBOT
    char buff[20];
    uint8_t data;
    uint8_t readBytes;
    
    for(;;)
    {
        if (Serial.available())
        {
            Serial.print(">");
            readBytes = Serial.readBytes(buff, 20);
            buff[readBytes] = '\0';
            data = strtoul(buff, '\0', 16);
            Serial.print(" 0x");
            Serial.println(data, HEX);
            robot.sendCommand((command_t)data);
        }
    }
#endif

#ifdef DEBUG_PLAYER
    char buff[20];
    uint16_t data;
    uint8_t readBytes;
    
    for(;;)
    {
        if (Serial.available())
        {
            Serial.print(">");
            readBytes = Serial.readBytes(buff, 20);
            buff[readBytes] = '\0';
            data = strtoul(buff, '\0', 10);
            Serial.print(" ");
            Serial.println(data, DEC);
            player.play(data);
        }
    }
#endif

#ifdef DEBUG_REMOTE
    uint32_t remoteData;
    uint8_t key;
    
    for(;;)
    {
        if (irAvailable())
        {
            remoteData = irRead();
            key = toIndex41(remoteData);
            Serial.print("remote: 0x");
            Serial.println(remoteData, HEX);
            Serial.print("key: ");
            Serial.println(key);
        }
    }
#endif
}

void loop()
{
    uint8_t volume = 5;
    for(;;)
    {
        updateRemoteKey();
        switch (remoteKey)
        {
            case 1: makrenim(); break;
            case 2: ain_ain_o(); break;
            case 3: ain_ain_end(); break;
            
            case 4: shat_tash(); break;
            case 5: mesogalot(); break;
            case 6: lo_relevanti(); break;
            
            case 7: shela_metzuinet(); break;
            case 8: import_agamim(); break;
            case 9: ale_veatzlach(); break;
            
            case 10: resend_nessi(); break;
            case 11: ma_kore(); break;
            case 12: throw_it(); break;
            
            case 13: 
                volume--;
                volume &= 7;
                player.volume(volume);
                break;
            case 14:
                volume++;
                volume &= 7;
                player.volume(volume);
                break;
            
            case 15: robot.sendCommand(CMD_WALK_FORWARD); break;
            case 16: robot.sendCommand(CMD_WALK_BACKWARD); break;
            case 17: robot.sendCommand(CMD_TURN_LEFT); break;
            case 18: robot.sendCommand(CMD_TURN_RIGHT); break;
            
            case 19: dahoki(); break;
            case 20: dahoki(); break;
            
            case 21: hcreps_binary(); break;
            case 22: roni_facha(); break;
            case 23: nipagesh_batzad(); break;
            case 24: shutdown(); break;
            
            case 25: shlomi(); break;
            case 26: ad_matay(); break;
            case 27: kama_ata(); break;
            case 28: halamat_tiz(); break;
            
            case 29: follow_instruction(); break;
            case 30: agamitzi(); break;
            case 31: ale_agamim(); break;
            case 32: sachbak_nestea(); break;
            
            case 33: song_1(); break;
            case 34: song_2(); break;
            case 35: song_3(); break;
            case 36: song_4(); break;
            
            case KEY_STOP:
                robot.sendCommand(CMD_STOP);
                player.stop();            
            break;
        }
        //delay(1);
    }
}

//}

//{ Functions

bool playAll(uint16_t id)
{
    player.play(id);
	while(digitalRead(BUS_PIN))
        if (updateRemoteKey())
            return true;
    return false;
}

bool sendCommand(command_t command, uint32_t ms)
{
    robot.sendCommand(command);
    if (waitMs(ms))
        return true;
    return false;
}

bool sendCommandsLoop(command_t command1, command_t command2, uint8_t count, uint32_t ms)
{
    while (count--)
    {
        robot.sendCommand(command1);
        robot.sendCommand(command2);
        if (waitMs(ms))
            return true;
    }
    return false;
}

bool sendCommandLoop(command_t command, uint8_t count, uint32_t ms)
{
    while (count--)
    {
        robot.sendCommand(command);
        if (waitMs(ms))
            return true;
    }
    return false;
}

bool waitMs(uint32_t ms)
{
    uint32_t key;
    while (ms--)
    {
        delay(1);
        if (updateRemoteKey())
            return true;
    }
    return false;
}

bool updateRemoteKey()
{
    if (irAvailable())
    {
        uint8_t tmp = toIndex40(irRead());
        if (KEY_NONE != tmp && KEY_REPEAT != tmp)
        {
            remoteKey = tmp;
            return true;
        }
    }
    remoteKey = KEY_NONE;
    return false;
}

//}

