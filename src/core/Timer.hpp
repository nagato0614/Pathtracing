//
// Created by 長井亨 on 2019/01/30.
//

#ifndef PATHTRACING_TIMER_HPP
#define PATHTRACING_TIMER_HPP
#include <chrono>

namespace nagato {

    /**
     * 時間計測用クラス
     * start()を実行せずにend(), getTime()を実行した場合の動作は未定義
     */
    class Timer {
        using sys = std::chrono::system_clock;
     public:
        Timer();

        void start();

        void stop();

        /**
         * 計測時間を返す
         * @tparam T 取得したい時間の単位
         * @return 計測時間
         */
        template <typename T = std::chrono::seconds>
        float getTime(){
            return std::chrono::duration_cast<T>(endTime - startTime).count();
        }

     private:

        sys::time_point startTime;
        sys::time_point endTime;
    };
}

#endif //PATHTRACING_TIMER_HPP
