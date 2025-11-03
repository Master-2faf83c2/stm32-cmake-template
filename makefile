BUILD_DIR = build
APP_BUILD_DIR = ${BUILD_DIR}/app

all: app
	
app:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Release -DTARGET_NAME=app
	cmake --build $(APP_BUILD_DIR) -j8

debug:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Debug -DTARGET_NAME=app
	cmake --build $(APP_BUILD_DIR) -j8

flash:
	@echo "flash bin file..."
	st-flash write $(APP_BUILD_DIR)/app.bin 0x08000000
