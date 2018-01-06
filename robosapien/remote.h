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
| Library to read IR remote NEC commands from VS1003 IR sensor in to arduino                     |
\*****************************************************************************/

#ifndef _REMOTE_H_
#define _REMOTE_H_


typedef enum remote_key_e
{
    KEY_MAKRENIM = 1,
    KEY_AIN_AIN_O,
    KEY_AIN_AIN_END,
    KEY_SHAT_TASH,
    KEY_MESOGALOT,
    KEY_LO_RELEVANTI,
    KEY_SHELA_METZUINET,
    KEY_IMPORT_AGAMIM,
    KEY_ALE_VEATZLACH,
    KEY_RESEND_NESSI,
    KEY_MA_KORE,
    KEY_THROW_IT,
    
    KEY_VOLUME_LOW,
    KEY_VOLUME_HIGH,
    
    KEY_MOVE_UP,
    KEY_MOVE_DOWN,
    KEY_MOVE_LEFT,
    KEY_MOVE_RIGHT,
    
    KEY_F1,
    KEY_F2,
    
    KEY_HCREPS_BINARY,
    KEY_RONI_FACHA,
    KEY_NIPAGESH_BATZAD,
    KEY_SHUTDOWN,
    KEY_SHLOMI,
    KEY_AD_MATAY,
    KEY_KAMA_ATA,
    KEY_HALAMAT_TIZ,
    KEY_SHEL_HMAFHCIDOT,
    KEY_FOLLOW_INSTRUCTION,
    KEY_AGAMITZI,
    KEY_ALE_AGAMIM,
    KEY_SACHBAK_NESTEA,
    KEY_DAHOKI,
    KEY_SONG_1,
    KEY_SONG_2,
    KEY_SONG_3,
    KEY_SONG_4,
    KEY_STOP,
    KEY_REPEAT = 254,
    KEY_NONE = 255,
} remote_key_t;

uint8_t toIndex40(uint32_t key)
{
    if (REPEAT == key) return KEY_REPEAT;
    if ((key >> 16) != 0x2FD) return KEY_NONE;
    key &= 0xFFFF;
    if (0x20DF == key) return KEY_STOP;    // off
    if (0xA857 == key) return KEY_STOP;    // mute
    if (0xD22D == key) return 1;    // 1
    if (0xD827 == key) return 2;    // 2
    if (0x7A85 == key) return 3;    // 3
    if (0x10EF == key) return 4;    // 4
    if (0x42BD == key) return 5;    // 5
    if (0xC23D == key) return 6;    // 6
    if (0xF00F == key) return 7;    // 7
    if (0x7887 == key) return 8;    // 8
    if (0xB847 == key) return 9;    // 9
    if (0x38C7 == key) return 10;    // 0
    if (0x18E7 == key) return 11;    // -/--
    if (0xB04F == key) return 12;    // <>
    if (0x52AD == key) return 13;    // nicam/a2
    if (0x48B7 == key) return 14;    // system
    if (0xE817 == key) return 15;    // up = pro +
    if (0xD02F == key) return 16;    // down = pro -
    if (0x50AF == key) return 17;    // left = vol -
    if (0xA05F == key) return 18;    // right = vol +
    if (0x8877 == key) return KEY_STOP;    // middle
    if (0x629D == key) return 19;    // display
    if (0xA25D == key) return 20;    // input
    if (0x6897 == key) return 21;    // esc
    if (0x30CF == key) return 22;    // auto
    if (0xF20D == key) return 23;    // page up
    if (0x40BF == key) return 24;    // page down
    if (0x926D == key) return 25;    // text
    if (0x807F == key) return 26;    // mix
    if (0xFA05 == key) return 27;    // hold
    if (0xBA45 == key) return 28;    // picture
    if (0xAA55 == key) return 29;    // index
    if (0x58A7 == key) return 30;    // size
    if (0x32CD == key) return 31;    // reveal
    if (0x906F == key) return 32;    // subpage
    if (0x6A95 == key) return 33;    // red
    if (0xEA15 == key) return 34;    // green
    if (0xB24D == key) return 35;    // yellow
    if (0x22DD == key) return 36;    // cyan
    return KEY_NONE;
}


#endif
