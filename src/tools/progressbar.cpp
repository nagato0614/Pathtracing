//
// Created by 長井亨 on 2019-01-31.
//
#include <iostream>
#include <zconf.h>
#include <sys/ioctl.h>
#include <thread>
#include <cmath>

class Progressbar {
 public:
    explicit Progressbar(int max)
            : max(static_cast<float>(max)),
              progress(0) {

        // 現在使用している行の数を取得
        struct winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        columns = w.ws_col;
    }

    void reset(int max) {
        this->max = max;
        this->progress = 0;
    }

    void update() {
        this->progress++;
    }

    void update(int prog) {
        this->progress += prog;
    }

    void printBar() {
        // 6列未満のときはbarを表示できないため数字のみ表示させる.
        if (columns < 7) {
            printBar();
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

    void printPercent() {
        std::printf("\r%3d%%", static_cast<int>(progress / max * 100));
        std::flush(std::cout);
    }

    float max;
    float progress;
    int columns;
};


int main() {
    int task = 100;
    Progressbar progressbar{task};

    for (int i = 0; i < task; i++) {
        progressbar.update();
        progressbar.printBar();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << std::endl;
}