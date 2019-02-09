//
// Created by 長井亨 on 2019-02-09.
//

#ifndef PATHTRACING_IMAGEBASEDLIGHTING_HPP
#define PATHTRACING_IMAGEBASEDLIGHTING_HPP


#include "Sky.hpp"

namespace nagato {
    class ImageBasedLighting : public Sky {
     public:
        explicit ImageBasedLighting(const std::string &filename);

        Spectrum getRadiance(const Ray &ray) const override;

     private:

        void open(const std::string &filename);

        int width;
        int height;
        std::unique_ptr<Spectrum[]> image;
    };
}

#endif //PATHTRACING_IMAGEBASEDLIGHTING_HPP
