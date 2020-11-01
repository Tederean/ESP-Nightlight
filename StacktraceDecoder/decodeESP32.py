#!/usr/bin/env python3

import subprocess
import os

subprocess.call([
    'java', '-jar', 'StacktraceDecoder.jar',
    os.path.abspath('/home/tederean/.platformio/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-addr2line'),
    os.path.abspath('../.pio/build/Bodenbeleuchtung_Test/firmware.elf'),
    os.path.abspath('StacktraceDecoder.txt')
])
