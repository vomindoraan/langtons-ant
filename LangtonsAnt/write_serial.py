#!/usr/bin/env python
import logging
import os
import re
import sys
from pathlib import Path

from serial import Serial, SerialException
from serial.tools.list_ports import comports


logging.basicConfig(
    filename=Path(__file__).with_suffix('.log'),
    format='[%(levelname)s] %(asctime)s - %(message)s',
    level=os.getenv('LOGLEVEL', 'INFO').upper()
)

SERIAL_BAUD_RATE = 115200
COMPORT_PATTERN = re.compile(r'/dev/ttyACM\d+|COM\d+')


def connect() -> Serial:
    """Open a serial connection on the first available matching port."""
    ports = [cp.device for cp in comports()]
    matching_ports = [p for p in ports if COMPORT_PATTERN.match(p)]
    logging.debug("All available ports: %s", ports)
    logging.debug("Matching ports: %s", matching_ports)
    if not matching_ports:
        raise SerialException("No serial device available")

    serial = Serial(baudrate=SERIAL_BAUD_RATE)
    serial.port = matching_ports[0]
    serial.open()
    logging.info("Connected to serial device on %s at %d baud",
                 serial.port, SERIAL_BAUD_RATE)
    return serial


if __name__ == '__main__':
    # if sys.argc < 2:
    #     logging.error("Missing message argument")
    #     sys.exit(-1)
    # msg = sys.argv[1].encode('utf-8')
    try:
        msg = input()
    except EOFError:
        msg = ""
    logging.info("Sending message: \"%s\"", msg)

    try:
        serial = connect()
    except SerialException as e:
        logging.error("Couldn't connect: %s", e)
        sys.exit(-2)

    serial.write(msg.encode('utf-8') + b'\0')
