import serial
import wiringpi 
from time import sleep


class RFID_Reader(object):
    
	def __init__(self, serial_interface_path, baudrate, proximity_pin_BCM_number):

		self.serial_interface = serial.Serial(serial_interface_path, baudrate)
		self.proximity_pin_BCM_number = proximity_pin_BCM_number

		wiringpi.wiringPiSetupGpio()
		wiringpi.pinMode(proximity_pin_BCM_number, wiringpi.GPIO.INPUT)
		wiringpi.pullUpDnControl(proximity_pin_BCM_number, wiringpi.PUD_DOWN)


	def readByte(self):
		return self.serial_interface.read()

	def readProximityState(self):
		return wiringpi.digitalRead(self.proximity_pin_BCM_number)

	def flushRFIDBuffer(self):
		self.serial_interface.reset_input_buffer()

	def listenForRFID(self):

		RFID_code = ""

		while True:

			byte = self.readByte()
			# Parse RF message to extract 10 byte RFID
			if byte == '\r':

				# First 2 bytes are always garbage, so discard them.
				RFID_code = RFID_code[2:len(RFID_code) - 1] 
                
				# RFID tag scans sometimes return different numbers of bytes. The extra bytes
				# are always at the start of the message. To deal with this, if the RFID is not
				# 10 bytes after the above step, cleave the message again to take the last 10 bytes of it.
				if len(RFID_code) > 10:
				
					RFID_code = RFID_code[len(RFID_code) - 10:len(RFID_code)]
					
				# Flush the buffer because multiple unwanted tag reads can occur and they will accumulate in the buffer.
				self.serial_interface.reset_input_buffer()
				sleep(1)
				return RFID_code 
			else:

				RFID_code += byte

