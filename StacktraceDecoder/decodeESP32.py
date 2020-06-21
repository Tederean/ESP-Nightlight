#!/usr/bin/env python3

import subprocess
import os

subprocess.call([
    'java', '-jar', 'StacktraceDecoder.jar',
    os.path.abspath('~/.platformio/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-addr2line'),
    os.path.abspath('../.pioenvs/nodemcu-32s/firmware.elf'),
    os.path.abspath('StacktraceDecoder.txt')
])
