#pragma once

class ProgressBar {
    public:
        static void start();
        static void update(int items_processed, int total_item_count);

    private:
        static int getShellWidthInChars();
};
