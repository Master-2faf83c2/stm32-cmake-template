# STM32H745XIH6 HAL 库 CMake 工程模板

---

# 开发环境

| 工具 | 版本 / 说明 |
|---|---|
| STM32CubeMX | 6.15.0 |
| arm-none-eabi-gcc | 14.3.1 |
| CMake | 3.22 |
| Make | 3.81 |
| Python | 3.10 |
| VSCode | 编辑器 |
| Clangd | VSCode 插件 |
| CMake Language Support | VSCode 插件 |
| Cortex-Debug | VSCode 插件 |
| st-flash | 下载固件 | 

---

## 构建编译

```bash
# 正式下载
make

# 下载双核固件
make flash

# 单独下载
make flash-cm7
make flash-cm4

# 只复位
make reset
```
