//
// Created by 長井亨 on 2019-01-31.
//

#include "Progressbar.hpp"


namespace nagato {

    Progressbar::Progressbar(int max)
            : max(static_cast<float>(max)),
              progress(0) {

        // 現在使用している行の数を取得
        struct winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        columns = w.ws_col;
    }

    void Progressbar::reset(int max) {
        this->max = max;
        this->progress = 0;
    }

    void Progressbar::update() {
        this->progress++;
    }

    void Progressbar::update(int prog) {
        this->progress += prog;
    }

    void Progressbar::printBar() {
        // 6列未満のときはbarを表示できないため数字のみ表示させる.
        if (columns < 7) {
            printPercent();
            return;
        }

        auto percnet = progress / max;


        // 表示できる最大のバーの数
        int maxNumOfbar = columns - 6;

        // 現段階で表示するバーの数
        int numOfBar = static_cast<int>(std::floor(maxNumOfbar * percnet));

        std::printf("[");
        // 進捗表示
        for (int i = 0; i < numOfBar; i++) {
            std::printf("=");
        }

        for (int i = 0; i < maxNumOfbar - numOfBar; i++) {
            std::printf(" ");
        }
        std::printf("]");
        std::printf("%3d%%\r", static_cast<int>(percnet * 100));

        std::flush(std::cout);
    }

    void Progressbar::printPercent() {
        std::printf("\r%3d%%", static_cast<int>(progress / max * 100));
        std::flush(std::cout);
    }
}