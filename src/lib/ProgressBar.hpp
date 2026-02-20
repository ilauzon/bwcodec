#pragma once

#include <string>

class ProgressBar {
    public:
        const std::string title;
        const int total_item_count;

        ProgressBar(std::string title, int total_item_count);

        void update(int items_added);

        void update_increment();

    private:
        int items_processed = 0;

        void draw_progress(); 
};
