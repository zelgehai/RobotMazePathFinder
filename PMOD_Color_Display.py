# @file PMOD_Color_Display.py
# 
# @brief Python test script used to verify the color detected by the PMOD COLOR module.
# 
# Python script that can be used to test the PMOD COLOR module
# by reading the detected color hexadecimal values from the serial terminal
# and displaying the color to a Pygame window.
#
# @note Python 3 and the Pygame library must be installed in order to run the test script.
#
# @author Aaron Nanas

import pygame
import serial
import sys

def validate_serial_port():
	if len(sys.argv) < 2:
		print("Provide a valid COM port number to connect to (format: COM#)")
		sys.exit()

def connect_to_serial_port():
	# Try to open the serial COM port with a baud rate of 115200.
 	# Otherwise, print an error message and exit the program
	try:
		ser = serial.Serial(sys.argv[1], 115200)
	except serial.serialutil.SerialException:
		print("ERROR! Could not find COM port %s" % sys.argv[1])
		sys.exit()
	
	return ser

def pygame_init():
	# Create a 500 x 500 Pygame window
	color_screen = pygame.display.set_mode((500, 500))

	# Create a timer event that will be triggered every 200 ms
	pygame.time.set_timer(pygame.USEREVENT, 10)

	return color_screen

if __name__ == "__main__":
	validate_serial_port()

	ser = connect_to_serial_port()

	color_screen = pygame_init()

	# Initialize an array to store hexadecimal and integer color values
	color_hex_buffer = list()
	color_integer_buffer = list()

	while True:
		# Wait until the timer event has been triggered
		timer_event = pygame.event.wait()

		# Exit the loop if the Pygame window has been closed by the user
		if timer_event.type == pygame.QUIT:
			break

		elif timer_event.type == pygame.USEREVENT:
			# If the timer event has been triggered, then read the received data
			color_value = ser.readline().decode("utf-8")

			# Display the color hexadecimal values received in the terminal
			print("Color Received:", color_value)
			
			# Expected format: "r=%04x g=%04x b=%04x\r\n"
			color_hex_buffer = [color_value[2:6], color_value[9:13], color_value[16:20]]
			
			# Pygame expects the color values to be within the range of 0 - 255
			# The lower eight bits must be truncated for each color channel
			color_integer_buffer = [int(value, 16) >> 8 for value in color_hex_buffer]

			# Update the screen by displaying the color that has been detected by the PMOD COLOR module
			color_screen.fill(pygame.Color(color_integer_buffer[0], color_integer_buffer[1], color_integer_buffer[2], 255))

			pygame.display.flip()

			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					pygame.quit()
					print("Pygame window closed")
					sys.exit()

	pygame.quit()
