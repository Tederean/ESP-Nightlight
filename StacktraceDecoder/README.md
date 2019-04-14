# CLI Stracktrace decoder for ESP8266 & ESP32

based on the work from https://github.com/me-no-dev/EspExceptionDecoder, 
littleyoda rewrote the decoder completely for command-line usage.

## Usage

Download the jar: https://github.com/littleyoda/EspStrackTraceDecoder/releases/latest

java -jar EspStackTraceDecoder.jar \<Path to xtensa-lx106-elf-addr2line> \<Elf-File> \<Dump of Exception>
