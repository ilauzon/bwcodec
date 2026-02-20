#include "ProgressBar.hpp"
#include <asm-generic/ioctls.h>
#include <format>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

ProgressBar::ProgressBar(std::string title, int total_item_count)
    : title(title), total_item_count(total_item_count) {}

void ProgressBar::update(int items_added) {
    items_processed += items_added;
    draw_progress();
}

void ProgressBar::update_increment() {
    items_processed++;
    draw_progress();
}

void ProgressBar::draw_progress() {
    auto percentage_done = std::format("{:02.0f}%", (float)items_processed *
                                                        100 / total_item_count);

    std::cout << title << " " << percentage_done << "\r" << std::flush;
}

//
// int ProgressBar::getShellWidthInChars() {
//     return 50;
//     struct winsize w;
//     ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//     return w.ws_col;
// }
