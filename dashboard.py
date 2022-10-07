#!/bin/python3

import os, sys
import re

#	A simple Python script to show all logged info in a fancy dashboard
# 	can be extended/improved to use a HTML/web UI to display fancy servo angles and stuff

if len(sys.argv) <= 1:
	print(
		f"Usage: {sys.argv[0]} <port>"
		"\n\t port: serial port where the arduino is connected. (Eg: COM1 on Windows/DOS; /dev/ttyUSB0 on Mac/Linux)"
	)
	exit()

try:
	import serial, parse
except ImportError:
	print("pyserial is missing, installing with pip")
	os.system(f"{sys.executable} -m pip install pyserial parse")

from serial import Serial
from parse import parse # headache-less regex

MAX_INFO_LINES = 5
BOLD = "\033[1m"
RESET = "\033[0m"

class Dashboard:

	def __init__(self):

		self.rate = 115200
		self.port = sys.argv[1]

		print(f"opening Serial connection to {self.port} at {self.rate}")

		#self.serial = Serial(self.port, self.rate, timeout=0)
		self.size = tuple(os.get_terminal_size())

		self.infos = []
		self.temps = {}
		self.gpsdata = {}

		print(end='\033[2J')

	def main(self):
		"""
			reads line from Serial and calls the relevant handler
		"""

		line = self.serial.readline().strip()
		
		group = parse("[{}] {}",line)[0]
		if ' ' in group: group = group.split(' ')[0]

		getattr(self, "data_" + group)( line )

	def testfile(self):
		## for testing purposes
		with open(sys.argv[1], 'r') as f:
			for line in f.readlines():

				line = line[:-1] #strip newline

				group = parse("[{}] {}",line)[0]
				if ' ' in group: group = group.split(' ')[0]

				getattr(self, "data_" + group)( line )				

	def data_INFO(self, line):
		"""
			handles all [INFO] lines:
			shows the last {MAX_INFO_LINES} info lines at the bottom of the screen
		"""
		
		self.infos.append(line)
		if len(self.infos) > MAX_INFO_LINES: self.infos.pop(0)

		print(end=f"\033[{self.size[1]//2 - MAX_INFO_LINES}E\033[0J") # move cursor to {size - MAX_INFO_LINES}-th line
		for i in self.infos:
			print(i.replace('[INFO]', f'{BOLD}[INFO]{RESET}'))

	def data_TEMP(self, line):
		"""
			handles all [TEMP (*)] lines:
		"""

		name = parse("[{} {}] {}" ,line)[1]
		self.temps[name] = line

		print(end="\033[H")
		for i in self.temps.values():
			print(
				('\033[31m\033[1m' if ("ALERT" in line) else "")
				+ i + RESET
			)

	def data_GPS(self, line):

		"""
			handles all [GPS] lines:
		"""

		_, attr, value = parse("[{}] {}: {}", line)
		self.gpsdata[attr] = value

		GPS_DISPLAY_OFFEST = 50

		print(end="\033[H")
		for k, v in self.gpsdata.items():
			if v != 'invalid':
				print(f"\n\033[{GPS_DISPLAY_OFFEST}C {k}: {v}", end='')


if __name__ == '__main__': Dashboard().main()