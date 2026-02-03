#include <asm-generic/ioctls.h>
#include <print>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ProgressBar.h"

void ProgressBar::start() {
    std::println();
}

void ProgressBar::update(int items_processed, int total_item_count) {
    std::print("\r");

    auto bar_col_count = getShellWidthInChars();

    std::string start_line = "[";
    std::print("{}", start_line);
    bar_col_count -= start_line.length();

    constexpr auto end_line = "] {:02.0f}%";
    int end_line_length = 5;

    bar_col_count -= end_line_length;


    for (int i = 0; i < bar_col_count; i++) {
        if ((float)i / bar_col_count > (float)items_processed / total_item_count) {
            std::print("#");
        }
    }

    std::print(end_line, (float)items_processed / total_item_count);
}

int ProgressBar::getShellWidthInChars() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}
