# === Arduino Project Configuration ===
# BOARD_FQBN=esp32:esp32:esp32 
BOARD_FQBN=esp32:esp32:mhetesp32devkit
SKETCH_DIR=firmware/main
ORG_FILE=$(SKETCH_DIR)/main.org
INO_FILE=$(SKETCH_DIR)/main.ino
PORT=/dev/ttyUSB0

.PHONY: all tangle compile upload clean

all: compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

clean:
	rm -rf $(SKETCH_DIR)/build
