# === Arduino Project Configuration ===
BOARD_FQBN       = esp32:esp32:mhetesp32devkit
WEMOS_BOARD_FQBN = esp8266:esp8266:d1_mini
SKETCH_DIR       = firmware/main
WEMOS_SKETCH_DIR = firmware/wemos-main
ORG_FILE         = $(SKETCH_DIR)/main.org
WEMOS_ORG_FILE   = $(WEMOS_SKETCH_DIR)/wemos-main.org
PORT             = /dev/ttyUSB0
WEMOS_OTA_HOST   = ESP8266.local
OTA_HOST         = smartGarden.local
OTA_PORT         = 3232
BUILD_DIR        = $(SKETCH_DIR)/build
WEMOS_BUILD_DIR  = $(WEMOS_SKETCH_DIR)/build
BIN_FILE         = $(BUILD_DIR)/main.ino.bin
WEMOS_BIN_FILE   = $(BUILD_DIR)/wemos-main.ino.bin
BUILD_PROPERTIES = --build-property build.partitions=default
ESPOTA           = $(shell find ~/.arduino15/packages/esp32/ -name espota.py | head -n 1)

.PHONY: all tangle compile upload clean monitor clean ota

all: compile wemos_compile

tangle:
	emacs --batch $(ORG_FILE) -f org-babel-tangle

compile: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) $(SKETCH_DIR)

compile_ota: tangle
	arduino-cli compile --fqbn $(BOARD_FQBN) $(BUILD_PROPERTIES) \
        --build-path $(BUILD_DIR) \
        $(SKETCH_DIR)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(SKETCH_DIR)

# Fix this so it makes the monitor, not picocom.
ota: compile_ota
	python3 $(ESPOTA) -i $(OTA_HOST) -p $(OTA_PORT) \
        --progress --auth OTA_Password --file $(BIN_FILE)

wemos_tangle:
	emacs --batch $(WEMOS_ORG_FILE) -f org-babel-tangle

wemos_compile: wemos_tangle
	arduino-cli compile --fqbn $(WEMOS_BOARD_FQBN) $(BUILD_PROPERTIES) $(WEMOS_SKETCH_DIR)

wemos_compile_ota: wemos_tangle
	arduino-cli compile --fqbn $(WEMOS_BOARD_FQBN) $(BUILD_PROPERTIES) \
        --build-path $(WEMOS_BUILD_DIR) \
        $(WEMOS_SKETCH_DIR)

wemos_upload: wemos_compile
	arduino-cli upload -p $(PORT) --fqbn $(WEMOS_BOARD_FQBN) $(WEMOS_SKETCH_DIR)

# Fix this so it makes the monitor, not picocom.
wemos_ota: wemos_compile_ota
	python3 $(ESPOTA) -i $(WEMOS_OTA_HOST) -p $(OTA_PORT) \
        --progress --auth OTA_Password --file $(WEMOS_BIN_FILE)

monitor:
	picocom --baud 115200 ${PORT}

changelog:
	bash scripts/gen-changelog.sh >> CHANGELOG.md

clean:
	rm -rf $(BUILD_DIR) $(WEMOS_BUILD_DIR)
