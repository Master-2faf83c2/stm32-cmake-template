BUILD_DIR = build
APP_BUILD_DIR = ${BUILD_DIR}/application

all: app
	
app:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Release -DTARGET_NAME=application
	cmake --build $(APP_BUILD_DIR) -j8

debug:
	@echo "Configuring project..."
	cmake -G "Unix Makefiles" -B $(APP_BUILD_DIR) -S . -DCMAKE_BUILD_TYPE=Debug -DTARGET_NAME=application
	cmake --build $(APP_BUILD_DIR) -j8