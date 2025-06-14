# === Arduino Project Configuration ===
BOARD_FQBN       = esp8266:esp8266:d1_mini
SKETCH_DIR       = .
ORG_FILE   	     = $(SKETCH_DIR)/soil-moisture-sensor.org
PORT             = /dev/ttyUSB0
BUILD_DIR        = $(SKETCH_DIR)/build
BIN_FILE         = $(BUILD_DIR)/soil-moisture-sensor.ino.bin
BUILD_PROPERTIES = --build-property build.partitions=default
ESPOTA           = $(shell find ~/.arduino15/packages/esp32/ -name espota.py | head -n 1)

.PHONY: all tangle compile upload clean monitor clean ota

all: compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) --build-path $(BUILD_DIR) $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

changelog:
	bash scripts/gen-changelog.sh >> CHANGELOG.md

clean:
	rm -rf $(BUILD_DIR)
