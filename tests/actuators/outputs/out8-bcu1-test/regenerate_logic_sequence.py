"""Render only the existing out8 Logic telegram array; never regenerate EEPROM."""
import argparse
import configparser
from pathlib import Path
import re
from types import SimpleNamespace


def generate(root):
    config = configparser.ConfigParser()
    config.read(root/'tests/Jung_2138_special_01.device')
    groups = {}
    for number, value in config['Group Addresses'].items():
        a, b, c = map(int, value.split(',')[0].split('/'))
        groups[int(number)] = (a << 11) | (b << 8) | c
    lines, count = [], 0

    def section(text):
        if lines: lines.append('')
        lines.append('// '+text)

    def emit(kind, length, step, data='', comment=''):
        nonlocal count
        if comment: lines.append('          // '+comment)
        count += 1
        prefix = ' ' if count == 1 else ','
        lines.append(f'/* {count:3} */ {prefix} {{{kind:<15}, {length:4},  0,  0, (StepFunction *) {step:<20}, {{{data}}}}}')

    def telegram(obj, value, step='_loop', comment='', sending=False):
        ga = groups[obj]
        source = 0x1101 if sending else 0x1114
        data = [0xbc, source >> 8, source & 255, ga >> 8, ga & 255, 0xe2, 0, 0x80, value]
        emit('TEL_TX' if sending else 'TEL_RX', 9, step, ', '.join(f'0x{x:02X}' for x in data), comment)

    namespace = dict(device=SimpleNamespace(com_object=list(range(20))), Section=section,
                     App_Loop=lambda step='_loop', ticks=0, comment='': emit('TIMER_TICK', ticks, step, comment=comment),
                     Receive_Telegram=telegram,
                     Send_Telegram=lambda obj, value, **kw: telegram(obj, value, sending=True, **kw))
    def include(name):
        exec(compile((root/'tests'/name).read_text(), name, 'exec'), namespace)
    namespace['Include'] = include
    exec(compile((root/'tests/special_logic.tc').read_text(), 'special_logic.tc', 'exec'), namespace)
    return 'static Telegram tel_out8_logic[] =\n{\n'+'\n'.join(lines)+'\n, {END}\n};'


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--write', action='store_true')
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    path = root/'src/special.cpp'
    old = path.read_text()
    new, count = re.subn(r'static Telegram tel_out8_logic\[\] =\n\{.*?\n\};', generate(root), old, count=1, flags=re.S)
    assert count == 1
    if args.write: path.write_text(new, newline='\n')
    elif new != old: raise SystemExit('Logic sequence is stale')
    print('LOGIC_SEQUENCE_MATCH')
