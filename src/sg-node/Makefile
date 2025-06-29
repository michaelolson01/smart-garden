# === Node Sensor skeleton MC build configuration ===
BOARD_FQBN       = esp8266:esp8266:d1_mini
SKETCH_DIR       = .
ORG_FILE         = $(SKETCH_DIR)/sg-node.org
PORT             = /dev/ttyUSB0
BUILD_DIR        = $(SKETCH_DIR)/build
BIN_FILE         = $(BUILD_DIR)/main.ino.bin
BUILD_PROPERTIES = --build-property build.partitions=default

.PHONY: all tangle compile upload clean

all: compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) \
        --build-path $(BUILD_DIR) \
        $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) \
        --build-path $(BUILD_DIR) \
        $(SKETCH_DIR)

clean:
	rm -rf $(BUILD_DIR)
