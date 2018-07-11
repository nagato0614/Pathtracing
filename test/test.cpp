//
// Created by 長井亨 on 2018/07/11.
//

#include <iostream>
#include "../Matrix4.hpp"
#include "../Vector4.hpp"

using namespace nagato;

int main(void)
{
	std::cout << "TEST MATRIX" << std::endl;

	double d[4][4] = {
		{ 7, 9, 3, 8 },
		{ 0, 2, 4, 8 },
		{ 3, 9, 0, 5 },
		{ 2, 2, 7, 3 }
	};

	Matrix4 d1(d);

	printMatrix4(Matrix4::identity());
	printMatrix4(d1);

	auto d2 = d1 * d1;

	printMatrix4(d2);

	auto d3 = d1 + d1;
	auto d4 = d1 - d1;

	printMatrix4(d3);
	printMatrix4(d4);

	Vector4 v(1, 2, 3, 4);

	printVector4(v);

	auto v1 = v * d1;
	auto v2 = d1 * v;

	printVector4(v1);
	printVector4(v2);
}
