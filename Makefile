# === Arduino Project Configuration ===
BOARD_FQBN=esp32:esp32:mhetesp32devkit
SKETCH_DIR=firmware/main
ORG_FILE=$(SKETCH_DIR)/main.org
PORT=/dev/ttyUSB0

.PHONY: all tangle compile upload clean

all: compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

monitor:
	picocom --baud 115200 ${PORT}

clean:
	rm -rf $(SKETCH_DIR)/build
