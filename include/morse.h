#ifndef MORSE_H
#define MORSE_H

#include <Arduino.h>
#include <map>
#include <vector>
#include <cstring>

// 摩斯电码表：字符 → 摩斯码（. = DOT, - = DASH）
struct MorseEntry {
    char ch;
    const char* code;  // 用 '.' 和 '-' 表示
};

// 标准摩斯电码表 A-Z, 0-9
static const MorseEntry MORSE_TABLE[] = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
    {'E', "."},   {'F', "..-."}, {'G', "--."},  {'H', "...."},
    {'I', ".."},  {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."},
    {'M', "--"},  {'N', "-."},   {'O', "---"},  {'P', ".--."},
    {'Q', "--.-"},{'R', ".-."},  {'S', "..."},  {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"},  {'X', "-..-"},
    {'Y', "-.--"},{'Z', "--.."},

    {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
    {'8', "---.."}, {'9', "----."},

    {'.', ".-.-.-"},{',', "--..--"},{'?', "..--.."},{'/', "-..-."},
    {'(', "-.--."}, {')', "-.--.-"},{'&', ".-..."},{':', "---..."},
    {';', "-.-.-."},{'=', "-...-"}, {'+', ".-.-."}, {'-', "-....-"},
    {'_', "..--.-"},{'"', ".-..-."},{'@', ".--.-."},{'!', "-.-.--"},
};

static const int MORSE_TABLE_SIZE = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

// 摩斯码符号时长常量（毫秒）
// 单位时长 = 1 dot (dit)
// dash = 3 dot时长
// 字符内间隔 = 1 dot时长
// 字符间间隔 = 3 dot时长
// 单词间间隔 = 7 dot时长

// 练习速度：慢速（适合入门）
const int DOT_TIME_SLOW = 250;    // 250ms 每个点
const int DASH_TIME_SLOW = 750;   // 750ms 每个划

// 标准速度
const int DOT_TIME_NORMAL = 120;  // 120ms
const int DASH_TIME_NORMAL = 360;

// 快速
const int DOT_TIME_FAST = 60;
const int DASH_TIME_FAST = 180;

// 根据字符查找摩斯码
const char* charToMorse(char c) {
    c = toupper(c);
    for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (MORSE_TABLE[i].ch == c) {
            return MORSE_TABLE[i].code;
        }
    }
    return nullptr;
}

// 根据摩斯码查找字符
char morseToChar(const char* code) {
    for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
        if (strcmp(MORSE_TABLE[i].code, code) == 0) {
            return MORSE_TABLE[i].ch;
        }
    }
    return '?';
}

// 获取可学习字符列表（A-Z, 0-9）
static const char LEARN_CHARS[] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    '0','1','2','3','4','5','6','7','8','9'
};
static const int LEARN_CHARS_COUNT = 36;

#endif // MORSE_H
