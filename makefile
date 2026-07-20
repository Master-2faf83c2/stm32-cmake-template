BUILD_DIR = build
APP_BUILD_DIR = ${BUILD_DIR}/app
CM7_ELF = $(APP_BUILD_DIR)/CM7/Project_CM7.elf
CM4_ELF = $(APP_BUILD_DIR)/CM4/Project_CM4.elf
CM7_BIN = $(APP_BUILD_DIR)/CM7/Project_CM7.bin
CM4_BIN = $(APP_BUILD_DIR)/CM4/Project_CM4.bin

.PHONY: all app debug bins flash flash-cm7 flash-cm4 reset

all: app
	
app:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Release -DTARGET_NAME=app
	cmake --build $(APP_BUILD_DIR) -j8

debug:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Debug -DTARGET_NAME=app
	cmake --build $(APP_BUILD_DIR) -j8

bins: app
	arm-none-eabi-objcopy -O binary $(CM7_ELF) $(CM7_BIN)
	arm-none-eabi-objcopy -O binary $(CM4_ELF) $(CM4_BIN)

flash: bins
	@echo "Flashing CM7..."
	st-flash write $(CM7_BIN) 0x08000000
	@echo "Flashing CM4..."
	st-flash write $(CM4_BIN) 0x08100000
	st-flash reset

flash-cm7: bins
	st-flash --reset write $(CM7_BIN) 0x08000000

flash-cm4: bins
	st-flash --reset write $(CM4_BIN) 0x08100000

reset:
	st-flash reset
