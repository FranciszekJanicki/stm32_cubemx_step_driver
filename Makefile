PROJECT_DIR := $(shell pwd)
BUILD_DIR := ${PROJECT_DIR}/build
APP_DIR := ${PROJECT_DIR}/app
DRIVERS_DIR := ${PROJECT_DIR}/Drivers
REQUIREMENTS_DIR := ${PROJECT_DIR}/requirements
STM32CUBEMX_DIR := ${PROJECT_DIR}/cmake/stm32cubemx
UTILITY_DIR := ${APP_DIR}/utility
DOCKER_DIR := ${PROJECT_DIR}/.devcontainer

.PHONY: docker-build
docker-build:
	docker build -t stm32 ${DOCKER_DIR}

.PHONY: docker-run
docker-run:
	docker run -it --privileged --device=/dev/ttyUSB0 -v $(PWD):/workspace -w /workspace stm32

.PHONY: build
build: 
	cd ${BUILD_DIR} && make

.PHONY: clean
clean: 
	rm -rf ${BUILD_DIR}

.PHONY: cmake
cmake:
	cd ${PROJECT_DIR} && make clean && mkdir build && cmake -S . -B build

.PHONY: flash
flash: 
	stlink -c port=swd -d ${BUILD_DIR}/app/main/app.elf -rst

.PHONY: serial
serial:
	minicom -D /dev/ttyACM0 -b 115200

.PHONY: clang-format
clang-format:
	for ext in h c cpp hpp; do \
		find $(SOURCE_DIR) -iname "*.$$ext" -print0 | xargs -0 -r clang-format -i; \
	done

.PHONY: add-utility
add-utility:
	git submodule add -f https://github.com/franciszekjanicki/stm32-utility.git ${UTILITY_DIR}

.PHONY: update-utility
update-utility:
	git submodule update --init --recursive

.PHONY: remove-utility
remove-utility:
	git submodule deinit -f ${UTILITY_DIR}
	git rm -rf ${UTILITY_DIR}
	rm -rf ${UTILITY_DIR}
	rm -rf .git/modules/app/utility

.PHONY: all
all:
	make build && make flash && make serial
