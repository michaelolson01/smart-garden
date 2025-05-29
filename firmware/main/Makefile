BOARD=esp32:esp32:esp32
PORT=/dev/ttyUSB0
FQBN=$(BOARD)
SKETCH=main.ino

all:
	arduino-cli compile --fqbn $(FQBN) .

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) .

monitor:
	arduino-cli monitor -p $(PORT) -c baudrate=115200

clean:
	rm -rf build/
