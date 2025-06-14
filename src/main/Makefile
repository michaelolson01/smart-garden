# === Arduino Project Configuration ===
BOARD_FQBN       = esp32:esp32:mhetesp32devkit
SKETCH_DIR       = .
ORG_FILE         = $(SKETCH_DIR)/main.org
PORT             = /dev/ttyUSB0
OTA_HOST         = smartGarden.local
OTA_PORT         = 3232
BUILD_DIR        = $(SKETCH_DIR)/build
BIN_FILE         = $(BUILD_DIR)/main.ino.bin
BUILD_PROPERTIES = --build-property build.partitions=default
ESPOTA           = $(shell find ~/.arduino15/packages/esp32/ -name espota.py | head -n 1)

.PHONY: all tangle compile upload clean monitor clean ota

all: compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) \
        --build-path $(BUILD_DIR) \
	 $(SKETCH_DIR)

compile_ota: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) \
        --build-path $(BUILD_DIR) \
        $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) \
        --build-path $(BUILD_DIR) \
	$(SKETCH_DIR)

ota: compile_ota
	python3 $(ESPOTA) -i $(OTA_HOST) -p $(OTA_PORT) \
        --progress --auth OTA_Password --file $(BIN_FILE)

clean:
	rm -rf $(BUILD_DIR) $(WEMOS_BUILD_DIR)
