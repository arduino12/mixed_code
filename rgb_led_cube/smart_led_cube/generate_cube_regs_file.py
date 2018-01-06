import os.path
import argparse


DEFAULT_COLUMNS_TO_MASKS = [
    0,  4,  8, 12,
    0,  5,  9, 15,
    0,  6, 10, 13,
    0,  7, 11, 14,
    1,  4,  9, 13,
    1,  5,  8, 14,
    1,  6, 11, 12,
    1,  7, 10, 15,
    2,  4, 10, 14,
    2,  5, 11, 13,
    2,  6,  8, 15,
    2,  7,  9, 12,
    3,  4, 11, 15,
    3,  5, 10, 12,
    3,  6,  9, 14,
    3,  7,  8, 13,
]


def led_connection_to_mask(led_format, columns_to_masks):
    led_format = led_format.lower()
    L, H = [v for k, v in {'a': '10', 'c': '01'}.items() if k in led_format][0]
    led_format = led_format.replace('c', 'a')
    rgb = [led_format.index(i) for i in 'rgb']
    ret = [None] * 192
    a = led_format.index('a')
    d = int(led_format[4] + '1')
    for z in range(4):
        for x in range(4):
            xz = x + z * 4
            col = columns_to_masks[xz * 4: xz * 4 + 4]
            for y in range(4):
                xyz = y * 16 + xz
                o = y * d
                for i, c in zip(range(0, 129, 64), rgb):
                    t = ['z'] * 16
                    t[col[(a + o) % 4]] = L
                    t[col[(c + o) % 4]] = H
                    ret[i + xyz] = ''.join(t)
    return ret


def masks_to_zipped_regs(index_masks):
    return [(i.index('1') << 4) | i.index('0') for i in index_masks]


def to_c_array(name, values, in_flash=False, values_per_line=16):
    j = 0
    ret = []
    l = len(values)
    ret.append('static const uint8_t %s[%s]%s = {' % (name, l, ' PROGMEM' if in_flash else ''))
    while j < l:
        ret.append('\t%s,' % (','.join('0x{:02X}'.format(i) for i in values[j: j + values_per_line]),))
        j += values_per_line
    ret.append('};')
    return ret


if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='charlieplexed rgb led cube array generator')
    parser.add_argument(
        '--led_format',
        metavar='<5 characters led format string>',
        dest='led_format',
        type=str,
        default='rgba+',
        help="led pins order: 'r', 'g', 'b' common pin: 'a' -anode 'c' -cathode rotation: '+' clockwise, '-' anti clockwise examples: 'rgba+' 'barg-'")
    parser.add_argument(
        '--columns_to_masks',
        type=int,
        nargs=64,
        default=DEFAULT_COLUMNS_TO_MASKS,
        metavar='',
        help='64 columns pins (16 groups of 4), maps column\'s pin (0 - 63) to mask index (0 - 15)')
    parser.add_argument(
        '--array_memory',
        type=str,
        default='ram',
        choices=('ram', 'flash'),
        metavar='<ram or flash>',
        help='save the generated array in ram or flash')
    parser.add_argument(
        '--output_file',
        metavar='<generated header file path>',
        type=str,
        default=os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'cube_zipped_regs.h')),
        help='generated array for each port register, or single zipped array')
    
    args = parser.parse_args()
    in_flash = args.array_memory == 'flash'
    output_content = ['''/*
auto generated charlieplexed rgb led cube array
led_format: %s
columns_to_masks: %s
array_memory: %s
*/''' % (args.led_format, ' '.join(str(i) for i in args.columns_to_masks), args.array_memory)]
    output_content.append('')
    output_content.extend(to_c_array(
        'cube_zipped_regs',
        masks_to_zipped_regs(led_connection_to_mask(args.led_format, args.columns_to_masks)),
        in_flash))
    output_content.append('')

    with open(args.output_file, 'w') as f:
        f.write('\n'.join(output_content))
