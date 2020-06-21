#!/usr/bin/env python3

import subprocess
import os

subprocess.call([
    'java', '-jar', 'StacktraceDecoder.jar',
    os.path.abspath('~/.platformio/packages/toolchain-xtensa/bin/xtensa-lx106-elf-addr2line'),
    os.path.abspath('../.pioenvs/nodemcuv2/firmware.elf'),
    os.path.abspath('StacktraceDecoder.txt')
])
