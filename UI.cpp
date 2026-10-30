
#include "UI.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace {
    struct Theme {
        int foreground;
        int background;
        int winAttribute;
    };

    // Foreground and background use ANSI colors on Ubuntu.
    // Windows uses the corresponding console color attributes.
    const Theme themes[] = {
    {37, 41, 0x4F}, // White on red
    {37, 44, 0x1F}, // White on blue
    {30, 47, 0x70}, // Black on white
    {37, 40, 0x07}, // White on 
    {30, 46, 0xB0}, // Black on bright cyan
    {30, 43, 0xE0}, // Black on bright yellow
    {30, 42, 0xA0}, // Black on bright green
    {30, 45, 0xD0}, // Black on bright magenta
    {37, 46, 0x3F}, // White on cyan
    {37, 42, 0x2F}, // White on green
    {37, 43, 0x6F}, // White on yellow
    {37, 45, 0x5F}, // White on magenta
    {30, 47, 0x70}, // Black on white
    {37, 40, 0x07}  // White on black



    };

    const int themeCount = sizeof(themes) / sizeof(themes[0]);
    Theme currentTheme = themes[0];

    int consoleWidth() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(h, &info))
            return info.srWindow.Right - info.srWindow.Left + 1;
#else
        struct winsize size;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0
            && size.ws_col > 0)
            return size.ws_col;
#endif
        return 80;
    }

    int consoleHeight() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(h, &info))
            return info.srWindow.Bottom - info.srWindow.Top + 1;
#else
        struct winsize size;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0
            && size.ws_row > 0)
            return size.ws_row;
#endif
        return 25;
    }

    void setTheme(const Theme& theme) {
        currentTheme = theme;

#ifdef _WIN32
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(
            h, static_cast<WORD>(theme.winAttribute)
        );
#else
        std::cout << "\033["
                  << theme.foreground << ";"
                  << theme.background << "m";
#endif
    }

    void clearScreen() {
#ifdef _WIN32
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO info;

        if (GetConsoleScreenBufferInfo(h, &info)) {
            DWORD cells =
                static_cast<DWORD>(info.dwSize.X) *
                static_cast<DWORD>(info.dwSize.Y);
            DWORD written = 0;

            COORD home = {0, 0};

            FillConsoleOutputCharacter(
                h, ' ', cells, home, &written
            );
            FillConsoleOutputAttribute(
                h,
                static_cast<WORD>(currentTheme.winAttribute),
                cells, home, &written
            );
            SetConsoleCursorPosition(h, home);
        } else {
            std::cout << "\033[2J\033[H";
        }
#else
        std::cout << "\033[2J\033[H";
#endif
    }

  
void centerText(const std::string& text) {
    int width = consoleWidth();

    // Keep long text from producing negative padding.
    int padding = (width - static_cast<int>(text.size())) / 2;

    if (padding < 0)
        padding = 0;

    std::cout << std::string(padding, ' ')
              << text << '\n';
}


    void horizontalLine() {
        int width = consoleWidth() - 12;
        if (width > 66) width = 66;
        if (width < 20) width = 20;

        centerText(std::string(width, '='));
    }

    void randomizeTheme() {
        int index = std::rand() % themeCount;
        setTheme(themes[index]);
    }

    void pause(int milliseconds) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(milliseconds)
        );
    }
}

void showWelcome() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (GetConsoleMode(h, &mode)) {
        SetConsoleMode(
            h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
        );
    }
#endif

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Startup screen: black background and bright white text.
#ifdef _WIN32
    setTheme({37, 40, 0x0F});
#else
    setTheme({97, 40, 0});
#endif

    clearScreen();

    centerText("");
    centerText("");
    centerText("STUDENT TASK SCHEDULER");
    centerText("");
    centerText("Initializing application...");
    centerText("");

    int width = consoleWidth();
    int barWidth = width > 60 ? 40 : width / 2;
    if (barWidth < 10) barWidth = 10;

    std::string bar(barWidth, ' ');
    centerText("Please wait");

    for (int i = 0; i < barWidth; ++i) {
        bar[i] = '#';
        std::cout << "\033[s";
        centerText("[" + bar + "]");
        std::cout << "\033[u";
        std::cout.flush();
        pause(45);
    }

    pause(400);

    // Transition to the first full-screen color theme.
    randomizeTheme();
    clearScreen();

    int blankLines = (consoleHeight() - 12) / 2;
    if (blankLines < 1) blankLines = 1;

    for (int i = 0; i < blankLines; ++i)
        std::cout << '\n';

    horizontalLine();
    centerText("STUDENT TASK SCHEDULER");
    centerText("COLLEGE WORKLOAD MANAGEMENT");
    horizontalLine();
    centerText("");
    centerText("APPLICATION READY");
    centerText("");
    centerText("Your tasks. Your schedule. Your control.");
    horizontalLine();

    std::cout.flush();
    pause(900);
}

void showMenu() {
    randomizeTheme();
    clearScreen();

    int blankLines = (consoleHeight() - 17) / 2;
    if (blankLines < 1) blankLines = 1;

    for (int i = 0; i < blankLines; ++i)
        std::cout << '\n';

    horizontalLine();
    centerText("STUDENT TASK SCHEDULER");
    centerText("MAIN MENU");
    horizontalLine();
    centerText("");

       centerText("+--------------------------------+");
    centerText("|  [1]  ADD NEW TASK             |");
    centerText("|  [2]  VIEW ALL TASKS           |");
    centerText("|  [3]  VIEW URGENT TASKS        |");
    centerText("|  [4]  MARK TASK COMPLETED      |");
    centerText("|  [5]  DELETE TASK              |");
    centerText("|  [6]  SAVE TASKS               |");
    centerText("|  [0]  EXIT APPLICATION         |");
    centerText("+--------------------------------+");

    centerText("");
    horizontalLine();
    centerText("ENTER YOUR CHOICE:");
   centerText("> ENTER OPTION <");
std::cout << "\n" << std::flush;
}

void waitForReturn() {
    std::string input;

    std::cout << "\n\n";
    centerText("OPERATION FINISHED");
    centerText("Press ENTER to return to the main menu...");

    std::getline(std::cin, input);
}

void showGoodbye() {
    randomizeTheme();
    clearScreen();

    int blankLines = (consoleHeight() - 8) / 2;
    if (blankLines < 1) blankLines = 1;

    for (int i = 0; i < blankLines; ++i)
        std::cout << '\n';

    horizontalLine();
    centerText("TRANSACTION COMPLETE");
    centerText("");
    centerText("THANK YOU FOR USING");
    centerText("STUDENT TASK SCHEDULER");
    centerText("");
    centerText("HAVE A PRODUCTIVE DAY!");
    horizontalLine();

    std::cout.flush();
    pause(1500);

#ifdef _WIN32
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 0x07
    );
#else
    std::cout << "\033[0m";
#endif
}
