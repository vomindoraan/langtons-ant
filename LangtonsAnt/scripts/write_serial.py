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
COMPORT_PATTERN = re.compile(r'COM\d+|/dev/ttyACM\d+')


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
    try:
        msg = sys.argv[1] if len(sys.argv) == 2 else input()
        msg_bytes = msg.encode('utf-8')
    except EOFError:
        msg = ""
        msg_bytes = b""
    except UnicodeEncodeError as e:
        logging.error("Invalid message: %s", e)
        sys.exit(-1)

    logging.info("Sending message: %r", msg)
    try:
        serial = connect()
        serial.write(msg_bytes + b'\0')  # Send as C string
        logging.info("OK")
    except SerialException as e:
        logging.error("ERR: %s", e)
        sys.exit(-2)
