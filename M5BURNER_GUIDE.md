# M5Burner 发布指南

## 分享码方式（推荐）

1. 打开 **M5Burner**
2. 点击左下角 **Custom → 自定义**
3. 点 **Publish → 发布**
4. 填写信息：
   - **Name:** Morse Code Trainer
   - **Description:** Learn Morse code with Cardputer keyboard + speaker
   - **Category:** Tool/工具
   - **Icon:** 可选
5. 上传 `firmware.bin` (位于 build/ 目录)
6. 发布后会生成一个 **Share Code (分享码)**
7. 把这个分享码分享出去，别人输入就能下载

## GitHub 方式

用户也可以通过 GitHub 直接使用：

1. 打开 M5Burner
2. 自定义 → 分享下载
3. 输入 GitHub Release 的 .bin 文件直链：
   ```
   https://github.com/你的用户名/MorseTrainer-Cardputer/releases/latest/download/MorseTrainer-Cardputer.bin
   ```

## 项目文件说明

```
D:\MorseTrainer\
├── platformio.ini       # 编译配置
├── src/main.cpp         # 主程序（英文界面）
├── include/morse.h      # 摩斯码数据
├── README.md            # 项目说明
├── LICENSE              # MIT 协议
└── .github/workflows/   # GitHub Actions 自动编译
```
