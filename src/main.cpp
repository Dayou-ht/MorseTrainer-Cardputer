/*********************************************************************
 * Morse Code Trainer for M5Stack Cardputer ADV
 *
 * Features:
 *   1. Reference Table - Browse Morse codes A-Z, 0-9
 *   2. Learn Mode     - See letter + hear Morse code
 *   3. Listen Quiz    - Hear Morse code, type the letter
 *   4. Type The Code  - See a letter, type dots and dashes
 *   5. Random Test    - 20-question comprehensive test
 *   6. Speed Setting  - Slow / Normal / Fast
 *
 * Hardware: M5Stack Cardputer ADV
 * Repository: https://github.com/yourusername/MorseTrainer-Cardputer
 *
 * MIT License
 *********************************************************************/

#include <Arduino.h>
#include <M5Cardputer.h>
#include "morse.h"

// ==================== Display Constants ====================
const int SCREEN_W = 240;
const int SCREEN_H = 135;
const int CHAR_W = 6;   // textSize 1 char width
const int CHAR_H = 8;   // textSize 1 char height

// Color theme - modern dark UI
const uint16_t CL_BG       = TFT_BLACK;
const uint16_t CL_PRIMARY  = TFT_CYAN;     // titles, accents
const uint16_t CL_SECONDARY = TFT_WHITE;   // body text
const uint16_t CL_HINT     = TFT_DARKGREY; // hints, labels
const uint16_t CL_ACCENT   = TFT_GREEN;    // correct, morse codes
const uint16_t CL_ERROR    = TFT_RED;      // wrong answers
const uint16_t CL_SCORE    = TFT_YELLOW;   // score display
const uint16_t CL_DIM      = 0x4208;       // dim border
const uint16_t CL_PANEL    = 0x0841;       // panel background

// ==================== State ====================
int currentSpeed = 1;  // 0=Slow, 1=Normal, 2=Fast
int quizScore = 0, quizTotal = 0;

// ==================== Speed ====================
int getDotTime() {
    const int times[] = { DOT_TIME_SLOW, DOT_TIME_NORMAL, DOT_TIME_FAST };
    return times[currentSpeed];
}

const char* speedName() {
    const char* names[] = { "Slow", "Normal", "Fast" };
    return names[currentSpeed];
}

int getDashTime() { return getDotTime() * 3; }

// ==================== Audio ====================
void playBeep(int freq, int ms) {
    M5Cardputer.Speaker.tone(freq, ms);
    delay(ms);
    M5Cardputer.Speaker.end();
}

void playMorseCode(const char* code, int dotTime) {
    for (int i = 0; code[i]; i++) {
        if (code[i] == '.') playBeep(700, dotTime);
        else if (code[i] == '-') playBeep(700, dotTime * 3);
        if (code[i + 1]) delay(dotTime);
    }
}

void playChar(char c, int dotTime) {
    const char* code = charToMorse(c);
    if (code) playMorseCode(code, dotTime);
}

// ==================== Keyboard ====================
char getKeyPress() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        auto keys = M5Cardputer.Keyboard.keyList();
        if (keys.size() > 0) {
            return M5Cardputer.Keyboard.getKeyValue(keys[0]).value_first;
        }
    }
    return 0;
}

char waitForKey() {
    char key;
    do { key = getKeyPress(); delay(30); } while (key == 0);
    while (getKeyPress()) delay(30);
    return key;
}

// ==================== UI Helpers ====================
static int txtW(const char* s, int sz) { return strlen(s) * CHAR_W * sz; }
static int centerX(const char* s, int sz) {
    int x = (SCREEN_W - txtW(s, sz)) / 2;
    return x < 0 ? 0 : x;
}

void drawPanel(int x, int y, int w, int h) {
    M5Cardputer.Display.fillRoundRect(x, y, w, h, 3, CL_PANEL);
    M5Cardputer.Display.drawRoundRect(x, y, w, h, 3, CL_DIM);
}

void drawTitle(const char* title) {
    M5Cardputer.Display.fillScreen(CL_BG);
    // Title bar with bottom border
    M5Cardputer.Display.fillRoundRect(0, 0, SCREEN_W, 26, 0, 0x001F); // very dark blue
    M5Cardputer.Display.drawFastHLine(0, 26, SCREEN_W, CL_PRIMARY);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(CL_PRIMARY);
    M5Cardputer.Display.setCursor(centerX(title, 2), 6);
    M5Cardputer.Display.printf("%s", title);
}

void drawHint(const char* text, int y) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(centerX(text, 1), y);
    M5Cardputer.Display.printf("%s", text);
}

void drawBigChar(char c, int y) {
    M5Cardputer.Display.setTextSize(6);
    M5Cardputer.Display.setTextColor(CL_PRIMARY);
    char buf[2] = {c, 0};
    M5Cardputer.Display.setCursor(centerX(buf, 6), y);
    M5Cardputer.Display.printf("%c", c);
}

void drawMorse(const char* code, int y) {
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(CL_ACCENT);
    M5Cardputer.Display.setCursor(centerX(code, 3), y);
    M5Cardputer.Display.printf("%s", code);
}

void drawMessage(const char* msg, int y, uint16_t color) {
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(color);
    M5Cardputer.Display.setCursor(centerX(msg, 2), y);
    M5Cardputer.Display.printf("%s", msg);
}

void drawProgressBar(int x, int y, int w, int h, float ratio, uint16_t color) {
    M5Cardputer.Display.drawRoundRect(x, y, w, h, 2, CL_DIM);
    if (ratio > 0) {
        int fw = (int)((float)w * ratio);
        M5Cardputer.Display.fillRoundRect(x + 1, y + 1, fw - 1, h - 2, 2, color);
    }
}

void drawMenuItem(int num, const char* title, const char* desc, int y, bool active) {
    M5Cardputer.Display.setTextSize(1);

    // Number box
    M5Cardputer.Display.fillRoundRect(3, y, 18, 14, 3, active ? CL_PRIMARY : CL_DIM);
    M5Cardputer.Display.setTextColor(CL_BG);
    M5Cardputer.Display.setCursor(7, y + 3);
    M5Cardputer.Display.printf("%d", num);

    // Title
    M5Cardputer.Display.setTextColor(active ? CL_PRIMARY : CL_SECONDARY);
    M5Cardputer.Display.setCursor(26, y + 1);
    M5Cardputer.Display.printf("%s", title);

    // Description
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(26, y + 9);
    M5Cardputer.Display.printf("%s", desc);
}

// ==================== Main Menu ====================
void showMenu() {
    M5Cardputer.Display.fillScreen(CL_BG);

    // Title area
    M5Cardputer.Display.fillRoundRect(0, 0, SCREEN_W, 28, 0, 0x001F);
    M5Cardputer.Display.drawFastHLine(0, 28, SCREEN_W, CL_PRIMARY);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.setCursor(centerX("⋆ MORSE TRAINER ⋆", 2), 7);
    M5Cardputer.Display.printf("⋆ MORSE TRAINER ⋆");

    struct MenuItem { const char* title; const char* desc; };
    const MenuItem items[] = {
        {"Reference",  "Browse Morse codes"},
        {"Learn",      "See code + hear it"},
        {"Listen Quiz","Type what you hear"},
        {"Type Code",  "Type . and - keys"},
        {"Random Test","20 questions"},
    };

    int y = 32;
    for (int i = 0; i < 5; i++) {
        drawMenuItem(i + 1, items[i].title, items[i].desc, y, false);
        y += 17;
    }

    // Speed row
    M5Cardputer.Display.drawFastHLine(0, y + 1, SCREEN_W, CL_DIM);
    y += 5;
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(5, y);
    M5Cardputer.Display.printf("Speed");
    M5Cardputer.Display.setTextColor(CL_SCORE);
    M5Cardputer.Display.printf(": %s", speedName());
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(SCREEN_W - 10 - txtW("[6]", 1), y);
    M5Cardputer.Display.printf("[6]");

    y += 12;
    drawHint("Press number key 1-6", SCREEN_H - 10);
}

// ==================== Mode 1: Reference ====================
void modeReference() {
    int page = 0;
    const int perPage = 8;

    while (true) {
        drawTitle("REFERENCE TABLE");

        int start = page * perPage;
        M5Cardputer.Display.setTextSize(1);
        int y = 33;
        for (int i = start; i < start + perPage && i < LEARN_CHARS_COUNT; i++) {
            char c = LEARN_CHARS[i];
            const char* code = charToMorse(c);

            // Char box
            M5Cardputer.Display.fillRoundRect(5, y, 16, 11, 2, 0x0841);
            M5Cardputer.Display.setTextColor(CL_PRIMARY);
            M5Cardputer.Display.setCursor(9, y + 2);
            M5Cardputer.Display.printf("%c", c);

            // Morse code
            M5Cardputer.Display.setTextColor(CL_ACCENT);
            M5Cardputer.Display.setCursor(28, y + 2);
            M5Cardputer.Display.printf("%s", code);

            y += 13;
        }

        int total = (LEARN_CHARS_COUNT + perPage - 1) / perPage;
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(5, SCREEN_H - 10);
        M5Cardputer.Display.printf("Pg %d/%d  N(next) P(prev) Enter=exit", page + 1, total);

        char key = waitForKey();
        if (key == KEY_ENTER || key == '`' || key == '~') return;
        if ((key == 'N' || key == 'n') && page < total - 1) page++;
        if ((key == 'P' || key == 'p') && page > 0) page--;
    }
}

// ==================== Mode 2: Learn ====================
void modeLearn() {
    int idx = 0;

    while (true) {
        char c = LEARN_CHARS[idx];
        const char* code = charToMorse(c);

        drawTitle("LEARN MODE");

        // Big letter
        drawBigChar(c, 32);

        // Morse code below
        if (code) drawMorse(code, 82);

        // Sound indicator
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(centerX("♫ playing...", 1), 106);
        M5Cardputer.Display.printf("♫ playing...");

        // Play sound
        if (code) playMorseCode(code, getDotTime());

        // Controls
        M5Cardputer.Display.fillRect(0, SCREEN_H - 10, SCREEN_W, 10, CL_BG);
        M5Cardputer.Display.setCursor(centerX("N=next  P=prev  Enter=exit", 1), SCREEN_H - 10);
        M5Cardputer.Display.printf("N=next  P=prev  Enter=exit");

        char key = waitForKey();
        if (key == KEY_ENTER || key == '`' || key == '~') return;
        if (key == 'N' || key == 'n') idx = (idx + 1) % LEARN_CHARS_COUNT;
        if (key == 'P' || key == 'p') idx = (idx - 1 + LEARN_CHARS_COUNT) % LEARN_CHARS_COUNT;
    }
}

// ==================== Mode 3: Listen Quiz ====================
void modeListenQuiz() {
    quizScore = quizTotal = 0;
    int streak = 0;

    while (true) {
        int idx = random(0, 26);  // letters only
        char correct = LEARN_CHARS[idx];
        const char* code = charToMorse(correct);

        drawTitle("LISTEN QUIZ");

        // Score
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_SCORE);
        M5Cardputer.Display.setCursor(5, SCREEN_H - 10);
        if (quizTotal > 0) {
            M5Cardputer.Display.printf("Score: %d/%d (%d%%)", quizScore, quizTotal,
                (quizScore * 100) / quizTotal);
        } else {
            M5Cardputer.Display.printf("Type letter then Enter");
        }

        drawHint("Listen and type the letter", 33);
        M5Cardputer.Display.setCursor(centerX("P=replay  Enter=submit", 1), 45);
        M5Cardputer.Display.printf("P=replay  Enter=submit");

        // Play Morse code
        if (code) playMorseCode(code, getDotTime());

        // Input
        char buf[4] = {0};
        int len = 0;
        while (true) {
            char key = getKeyPress();
            if (key) {
                if (key == KEY_ENTER || key == '`' || key == '~') {
                    if (len > 0) break;
                    else return;  // exit
                }
                if (key == KEY_BACKSPACE && len > 0) buf[--len] = 0;
                else if (key >= 'a' && key <= 'z' && len < 3) buf[len++] = key - 32;  // toupper
                else if (key >= '0' && key <= '9' && len < 3) buf[len++] = key;
                else if ((key == 'P' || key == 'p') && code) playMorseCode(code, getDotTime());

                // Show input
                M5Cardputer.Display.fillRect(60, 55, 120, 28, CL_BG);
                M5Cardputer.Display.setTextSize(4);
                M5Cardputer.Display.setTextColor(CL_ACCENT);
                M5Cardputer.Display.setCursor(centerX(buf, 4), 58);
                M5Cardputer.Display.printf("%s", buf);
            }
            delay(30);
        }

        quizTotal++;
        if (buf[0] == correct) {
            quizScore++; streak++;
            playBeep(1200, 80); playBeep(1500, 80);
            drawMessage("CORRECT!", 90, CL_ACCENT);
            delay(700);
        } else {
            streak = 0;
            playBeep(200, 300);
            char msg[32];
            snprintf(msg, sizeof(msg), "Wrong! Answer: %c  [%s]", correct, code ? code : "");
            M5Cardputer.Display.setTextSize(1);
            M5Cardputer.Display.setTextColor(CL_ERROR);
            M5Cardputer.Display.setCursor(centerX(msg, 1), 90);
            M5Cardputer.Display.printf("%s", msg);
            delay(1500);
        }
        if (streak >= 10) {
            drawMessage("10 in a row! Excellent!", 105, CL_ACCENT);
            delay(1000); streak = 0;
        }
    }
}

// ==================== Mode 4: Type Code ====================
void modeTypeCode() {
    quizScore = quizTotal = 0;

    while (true) {
        int idx = random(0, 26);
        char c = LEARN_CHARS[idx];
        const char* correct = charToMorse(c);

        drawTitle("TYPE THE CODE");

        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_SCORE);
        M5Cardputer.Display.setCursor(5, SCREEN_H - 10);
        if (quizTotal > 0) {
            M5Cardputer.Display.printf("Score: %d/%d (%d%%)  Enter=exit", quizScore, quizTotal,
                (quizScore * 100) / quizTotal);
        } else {
            M5Cardputer.Display.printf("Enter=exit");
        }

        drawBigChar(c, 28);

        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(centerX("[.]=dot  [-]=dash  R=replay", 1), 76);
        M5Cardputer.Display.printf("[.]dot  [-]=dash  R=replay");
        M5Cardputer.Display.setCursor(centerX("Enter=submit", 1), 85);
        M5Cardputer.Display.printf("Enter=submit");

        char input[16] = {0};
        int len = 0;
        while (true) {
            char key = getKeyPress();
            if (key) {
                if (key == KEY_ENTER || key == '`' || key == '~') {
                    if (len == 0) return;
                    break;
                }
                if (key == KEY_BACKSPACE && len > 0) input[--len] = 0;
                else if (key == '.' && len < 14) input[len++] = '.';
                else if (key == '-' && len < 14) input[len++] = '-';
                else if ((key == 'R' || key == 'r') && correct) playMorseCode(correct, getDotTime());

                M5Cardputer.Display.fillRect(5, 92, SCREEN_W - 10, 28, CL_BG);
                M5Cardputer.Display.setTextSize(2);
                M5Cardputer.Display.setTextColor(CL_ACCENT);
                M5Cardputer.Display.setCursor(centerX(input, 2), 95);
                M5Cardputer.Display.printf("%s", input);
            }
            delay(30);
        }

        quizTotal++;
        if (strcmp(input, correct) == 0) {
            quizScore++;
            drawMessage("CORRECT!", 118, CL_ACCENT);
            playBeep(1200, 100); delay(500);
        } else {
            drawMessage("WRONG", 118, CL_ERROR);
            playBeep(200, 300); delay(300);
            char msg[20]; snprintf(msg, sizeof(msg), "Correct: %s", correct);
            M5Cardputer.Display.setTextSize(1);
            M5Cardputer.Display.setTextColor(CL_ACCENT);
            M5Cardputer.Display.setCursor(centerX(msg, 1), 125);
            M5Cardputer.Display.printf("%s", msg);
            delay(1200);
        }
    }
}

// ==================== Mode 5: Random Test ====================
void modeTest() {
    quizScore = quizTotal = 0;
    const int MAX = 20;
    int order[MAX];
    for (int i = 0; i < MAX; i++) order[i] = random(0, LEARN_CHARS_COUNT);

    drawTitle("RANDOM TEST");
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(centerX("20 questions - hear Morse", 1), 45);
    M5Cardputer.Display.printf("20 questions - hear Morse");
    M5Cardputer.Display.setCursor(centerX("and type the letter", 1), 55);
    M5Cardputer.Display.printf("and type the letter");
    M5Cardputer.Display.setCursor(centerX("Press Enter to start", 1), 70);
    M5Cardputer.Display.printf("Press Enter to start");
    waitForKey();
    while (getKeyPress()) delay(30);

    for (int q = 0; q < MAX; q++) {
        char c = LEARN_CHARS[order[q]];
        const char* code = charToMorse(c);

        drawTitle("RANDOM TEST");

        // Progress
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(5, 30);
        M5Cardputer.Display.printf("Q %d/%d", q + 1, MAX);

        M5Cardputer.Display.setTextColor(CL_SCORE);
        M5Cardputer.Display.setCursor(SCREEN_W - 10 - txtW("  Score: 0/0", 1), SCREEN_H - 10);
        if (quizTotal > 0) {
            M5Cardputer.Display.printf("Score: %d/%d", quizScore, quizTotal);
        }

        drawProgressBar(5, 40, SCREEN_W - 10, 5, (float)q / MAX, CL_PRIMARY);

        M5Cardputer.Display.setCursor(centerX("Listen + type letter  P=replay", 1), 52);
        M5Cardputer.Display.printf("Listen + type letter  P=replay");
        M5Cardputer.Display.setCursor(centerX("Enter=submit", 1), 62);
        M5Cardputer.Display.printf("Enter=submit");

        if (code) playMorseCode(code, getDotTime());

        // Input
        char buf[4] = {0};
        int len = 0;
        while (true) {
            char key = getKeyPress();
            if (key) {
                if (key == KEY_ENTER || key == '`' || key == '~') {
                    if (len > 0) break;
                    else return;
                }
                if (key == KEY_BACKSPACE && len > 0) buf[--len] = 0;
                else if (key >= 'a' && key <= 'z' && len < 3) buf[len++] = key - 32;
                else if (key >= '0' && key <= '9' && len < 3) buf[len++] = key;
                else if ((key == 'P' || key == 'p') && code) playMorseCode(code, getDotTime());

                M5Cardputer.Display.fillRect(60, 70, 120, 30, CL_BG);
                M5Cardputer.Display.setTextSize(4);
                M5Cardputer.Display.setTextColor(CL_ACCENT);
                M5Cardputer.Display.setCursor(centerX(buf, 4), 72);
                M5Cardputer.Display.printf("%s", buf);
            }
            delay(30);
        }

        quizTotal++;
        if (buf[0] == c) {
            quizScore++;
            drawMessage("✓ Correct!", 100, CL_ACCENT);
            playBeep(1200, 80); delay(500);
        } else {
            drawMessage("✗ Wrong!", 100, CL_ERROR);
            playBeep(200, 300); delay(300);
            char msg[24]; snprintf(msg, sizeof(msg), "Answer: %c  [%s]", c, code ? code : "");
            M5Cardputer.Display.setTextSize(1);
            M5Cardputer.Display.setTextColor(CL_ACCENT);
            M5Cardputer.Display.setCursor(centerX(msg, 1), 115);
            M5Cardputer.Display.printf("%s", msg);
            delay(1000);
        }
    }

    // Results
    drawTitle("TEST COMPLETE!");
    if (quizTotal > 0) {
        int pct = (quizScore * 100) / quizTotal;
        M5Cardputer.Display.setTextSize(2);
        M5Cardputer.Display.setTextColor(CL_SCORE);
        char buf[20]; snprintf(buf, sizeof(buf), "%d / %d", quizScore, quizTotal);
        M5Cardputer.Display.setCursor(centerX(buf, 2), 45);
        M5Cardputer.Display.printf("%s", buf);

        snprintf(buf, sizeof(buf), "%d%%", pct);
        M5Cardputer.Display.setCursor(centerX(buf, 2), 65);
        M5Cardputer.Display.printf("%s", buf);

        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(TFT_WHITE);
        M5Cardputer.Display.setCursor(centerX(
            pct >= 90 ? "Amazing! Morse Master!" :
            pct >= 70 ? "Great job! Keep it up!" :
            pct >= 50 ? "Not bad, keep practicing!" :
            "Keep practicing, you'll get it!", 1), 90);
        M5Cardputer.Display.printf("%s",
            pct >= 90 ? "Amazing! Morse Master!" :
            pct >= 70 ? "Great job! Keep it up!" :
            pct >= 50 ? "Not bad, keep practicing!" :
            "Keep practicing, you'll get it!");
    }
    drawHint("Press Enter to return", SCREEN_H - 10);
    waitForKey();
    while (getKeyPress()) delay(30);
}

// ==================== Mode 6: Speed ====================
void modeSpeed() {
    while (true) {
        drawTitle("SPEED SETTING");

        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(5, 35);
        M5Cardputer.Display.printf("Current: ");
        M5Cardputer.Display.setTextColor(CL_SCORE);
        M5Cardputer.Display.printf("%s (%dms/dot)", speedName(), getDotTime());

        // Speed options as cards
        const int speeds[] = {0, 1, 2};
        const char* labels[] = {"Slow", "Normal", "Fast"};
        const int times[] = {DOT_TIME_SLOW, DOT_TIME_NORMAL, DOT_TIME_FAST};

        int x = 10;
        for (int i = 0; i < 3; i++) {
            bool active = (i == currentSpeed);
            int w = 65;
            M5Cardputer.Display.fillRoundRect(x, 52, w, 40, 4, active ? CL_PRIMARY : CL_PANEL);
            M5Cardputer.Display.drawRoundRect(x, 52, w, 40, 4, active ? CL_PRIMARY : CL_DIM);

            M5Cardputer.Display.setTextColor(active ? CL_BG : CL_SECONDARY);
            M5Cardputer.Display.setTextSize(1);
            M5Cardputer.Display.setCursor(x + (w - txtW(labels[i], 1)) / 2, 58);
            M5Cardputer.Display.printf("%s", labels[i]);
            M5Cardputer.Display.setTextSize(1);
            char t[8]; snprintf(t, sizeof(t), "%dms", times[i]);
            M5Cardputer.Display.setTextColor(active ? 0x630C : CL_HINT);
            M5Cardputer.Display.setCursor(x + (w - txtW(t, 1)) / 2, 70);
            M5Cardputer.Display.printf("%s", t);

            x += w + 8;
        }

        drawHint("Press 1=Slow  2=Normal  3=Fast  Enter=exit", SCREEN_H - 10);

        // Demo at bottom
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setTextColor(CL_HINT);
        M5Cardputer.Display.setCursor(5, 102);
        M5Cardputer.Display.printf("Demo: S O S (press key to change)");

        char key = waitForKey();
        if (key == KEY_ENTER || key == '`' || key == '~') return;
        if (key == '1') currentSpeed = 0;
        if (key == '2') currentSpeed = 1;
        if (key == '3') currentSpeed = 2;

        // Play SOS demo
        playMorseCode("...", getDotTime());
        delay(getDotTime() * 3);
        playMorseCode("---", getDotTime());
        delay(getDotTime() * 3);
        playMorseCode("...", getDotTime());
    }
}

// ==================== Setup & Loop ====================
void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextWrap(false);
    randomSeed(millis());

    // Splash
    M5Cardputer.Display.fillScreen(CL_BG);

    // Animated title
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(CL_PRIMARY);
    M5Cardputer.Display.setCursor(centerX("⋆ MORSE CODE", 2), 25);
    M5Cardputer.Display.printf("⋆ MORSE CODE");
    M5Cardputer.Display.setCursor(centerX("TRAINER ⋆", 2), 45);
    M5Cardputer.Display.printf("TRAINER ⋆");

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(CL_HINT);
    M5Cardputer.Display.setCursor(centerX("M5Cardputer ADV", 1), 75);
    M5Cardputer.Display.printf("M5Cardputer ADV");

    // Dot animation
    M5Cardputer.Display.setCursor(centerX("Loading", 1), 95);
    M5Cardputer.Display.printf("Loading");
    for (int i = 0; i < 3; i++) {
        M5Cardputer.Display.printf(".");
        delay(400);
    }

    // Play MORSE intro
    delay(200);
    playChar('M', DOT_TIME_SLOW); delay(getDotTime() * 3);
    playChar('O', DOT_TIME_SLOW); delay(getDotTime() * 3);
    playChar('R', DOT_TIME_SLOW); delay(getDotTime() * 3);
    playChar('S', DOT_TIME_SLOW); delay(getDotTime() * 3);
    playChar('E', DOT_TIME_SLOW);
}

void loop() {
    showMenu();

    while (true) {
        char key = waitForKey();
        switch (key) {
            case '1': modeReference(); break;
            case '2': modeLearn(); break;
            case '3': modeListenQuiz(); break;
            case '4': modeTypeCode(); break;
            case '5': modeTest(); break;
            case '6': modeSpeed(); break;
            default: continue;
        }
        while (getKeyPress()) delay(30);
        break;
    }
}
