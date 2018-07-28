//
// Created by 長井亨 on 2018/07/28.
//

#ifndef PATHTRACING_LISTAABB_HPP
#define PATHTRACING_LISTAABB_HPP


#include "Scene.hpp"

namespace nagato
{
    class ListAABB
    {
     public:
        ListAABB(Scene *s);

        void makeAABB();

     private:
        Scene *scene;
    };
}

#endif //PATHTRACING_LISTAABB_HPP
