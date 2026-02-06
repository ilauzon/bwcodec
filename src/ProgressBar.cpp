#include <asm-generic/ioctls.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ProgressBar.h"
#include <string>
#include <iostream>

ProgressBar::ProgressBar(std::string title, int total_item_count): 
    title(title), 
    total_item_count(total_item_count) { }

void ProgressBar::update(int items_added) {
    items_processed += items_added;
    draw_progress();
}

void ProgressBar::update_increment() {
    items_processed++;
    draw_progress();
}

void ProgressBar::draw_progress() {
    // std::print("\r");
    //
    // auto bar_col_count = getShellWidthInChars();
    //
    // std::string start_line = "[";
    // std::print("{}", start_line);
    // bar_col_count -= start_line.length();
    //
    // constexpr auto end_line = "] {:02.0f}%";
    // int end_line_length = 5;
    //
    // bar_col_count -= end_line_length;
    //
    //
    // for (int i = 0; i < bar_col_count; i++) {
    //     if ((float)i / bar_col_count > (float)items_processed / total_item_count) {
    //         std::print("#");
    //     }
    // }

    auto percentage_done = std::format(
            "{:02.0f}%", 
            (float)items_processed * 100 / total_item_count
    );

    std::cout << title << " " << percentage_done << "\r" << std::flush;

}

//
// int ProgressBar::getShellWidthInChars() {
//     return 50;
//     struct winsize w;
//     ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//     return w.ws_col;
// }
