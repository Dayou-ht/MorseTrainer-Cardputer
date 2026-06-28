# ⋆ Morse Code Trainer / 摩斯电码学习器

**An interactive Morse code learning app for M5Stack Cardputer ADV**  
**一款为 M5Stack Cardputer ADV 打造的交互式摩斯电码学习器**

![Build](https://github.com/zhudayong88/MorseTrainer-Cardputer/actions/workflows/build.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## 📸 Screenshots / 截图

*(coming soon / 待添加)*

---

## ✨ Features / 功能

| # | Mode / 模式 | English | 中文说明 |
|---|-------------|---------|----------|
| 1 | **Reference** | Browse A-Z and 0-9 Morse codes | 浏览 A-Z、0-9 的摩斯码表 |
| 2 | **Learn** | See the letter + hear its Morse code | 显示字母 + 播放摩斯码声音 |
| 3 | **Listen Quiz** | Hear Morse code → type the letter | 听摩斯码 → 输入对应的字母 |
| 4 | **Type Code** | See a letter → type `.` and `-` | 看字母 → 用 `.` 和 `-` 打出摩斯码 |
| 5 | **Random Test** | 20-question comprehensive test | 20 题综合测验 + 评分 |
| 6 | **Speed** | Adjust playback speed | 调整播放速度（慢/标准/快） |

---

## 🎮 Controls / 操作说明

| Key / 按键 | Function / 功能 |
|-------------|-----------------|
| **1-6** | Select mode / 选择模式 |
| **N** | Next page / next item / 下一页 / 下一个 |
| **P** | Previous page / previous item / 上一页 / 上一个 |
| **R** or **P** | Replay Morse sound / 重播摩斯码 |
| **Enter** | Submit answer (with input) or exit (empty) / 提交或退出 |
| **Backspace** | Delete last character / 删除上一个字符 |
| **.** (period) | Dot in Type Code mode / 打点 |
| **-** (minus) | Dash in Type Code mode / 打划 |

### In Speed mode / 速度设置
| Key | Speed |
|-----|-------|
| **1** | Slow / 慢速 (250ms) |
| **2** | Normal / 标准 (120ms) |
| **3** | Fast / 快速 (60ms) |

---

## 📦 Installation / 安装方法

### Option 1: M5Burner (推荐 / Recommended)

1. Open **M5Burner** / 打开 M5Burner
2. Click **Custom → Share Burn** / 点击 自定义 → 分享下载
3. Enter share code / 输入分享码: **`nk1kjuKx2MK1xANY`**
4. Flash to device / 烧录

### Option 2: PlatformIO

```bash
# Clone / 克隆仓库
git clone https://github.com/zhudayong88/MorseTrainer-Cardputer.git
cd MorseTrainer-Cardputer

# Build / 编译
pio run

# Upload / 烧录（需要先进入下载模式）
pio run --target upload
```

### Option 3: Pre-built binary / 预编译固件

Download `MorseTrainer-Cardputer.bin` from the [Releases page](https://github.com/zhudayong88/MorseTrainer-Cardputer/releases) and flash:

从 [Releases 页面](https://github.com/zhudayong88/MorseTrainer-Cardputer/releases) 下载 `.bin` 文件后：

```bash
esptool.py --chip esp32s3 --port COM3 write_flash 0x0 firmware.bin
```

### Enter Download Mode / 进入下载模式

```
1. Slide power switch OFF / 开关拨到 OFF
2. HOLD G0 button / 按住 G0 按钮
3. Connect USB-C / 插 USB-C 线
4. Release G0 / 松开 G0
```

---

## 🛠 Building from Source / 从源码编译

### Prerequisites / 准备工作

- **PlatformIO** (VS Code extension or CLI)
- Or **Arduino IDE** with ESP32 board support

### PlatformIO

```bash
git clone https://github.com/zhudayong88/MorseTrainer-Cardputer.git
cd MorseTrainer-Cardputer
pio run            # Build / 编译
pio run --target upload  # Upload / 烧录
```

### Arduino IDE

1. Install **M5Cardputer** library via Library Manager / 通过库管理器安装 M5Cardputer
2. Open `src/main.cpp` / 打开 `src/main.cpp`
3. Board: `ESP32S3 Dev Module` / 选择开发板
4. Flash settings: Flash Mode `QIO`, Flash Size `8MB`, Partition `Default 4MB`
5. Upload / 上传

---

## 📁 Project Structure / 项目结构

```
MorseTrainer-Cardputer/
├── platformio.ini         # PlatformIO config / 编译配置
├── src/
│   └── main.cpp          # Main program (English UI) / 主程序（英文界面）
├── include/
│   └── morse.h           # Morse code table / 摩斯码数据
├── .github/workflows/    # CI build automation / 自动编译
├── README.md             # This file
├── LICENSE               # MIT License
└── M5BURNER_GUIDE.md     # M5Burner publish guide
```

---

## 📊 Technical Info / 技术参数

| Item / 项目 | Detail / 详情 |
|-------------|---------------|
| Display / 屏幕 | 240×135 TFT (ST7789), M5GFX |
| Audio / 音频 | Built-in speaker NS4168 / 内置扬声器 |
| Keyboard / 键盘 | Full QWERTY matrix / 全键盘矩阵 |
| Flash usage / 闪存占用 | ~15% (499KB / 3.2MB) |
| RAM usage / 内存占用 | ~7% (23KB / 320KB) |
| Platform / 平台 | ESP32-S3, Arduino framework |

---

## 📤 Publishing to M5Burner / 发布到 M5Burner

See [M5BURNER_GUIDE.md](M5BURNER_GUIDE.md) for instructions on publishing share codes.

发布分享码的方法请看 [M5BURNER_GUIDE.md](M5BURNER_GUIDE.md)。

---

## 🤝 Contributing / 贡献

PRs welcome! If you have ideas for improvement, feel free to open an issue or submit a pull request.

欢迎提交 PR！有改进想法请提 Issue 或直接 PR。

---

## 📄 License / 协议

MIT License — free to use, modify, share.  
MIT 协议 — 可自由使用、修改、分享。

---

*Built for the M5Stack Cardputer community · 为 Cardputer 社区打造*
