# === Arduino Project Configuration ===

.PHONY: all tangle compile upload clean monitor clean ota

all: main soil-moisture-sensor monitor

main:
	$(MAKE) -C src/main

main-ota:
	$(MAKE) -C src/main ota

main-upload:
	$(MAKE) -C src/main upload

soil-moisture-sensor:
	$(MAKE) -C src/soil-moisture-sensor

soil-moisture-sensor-upload:
	$(MAKE) -C src/soil-moisture-sensor upload

monitor:
	$(MAKE) -C src/monitor

changelog:
	bash scripts/gen-changelog.sh >> CHANGELOG.md

clean:
	$(MAKE) -C src/main clean
	$(MAKE) -C src/soil-moisture-sensor clean
	$(MAKE) -C src/monitor clean

changelog:
	bash scripts/gen-changelog.sh >> CHANGELOG.md
