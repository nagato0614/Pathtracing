//
// Created by 長井亨 on 2018/07/11.
//

#include <iostream>
#include <cstdio>
#include "../Matrix4.hpp"
#include "../Vector4.hpp"
#include "../Vector3.hpp"

using namespace nagato;

int main(void)
{
	std::cout << "TEST MATRIX" << std::endl;
//
//	double d[4][4] = {
//		{ 7, 9, 3, 8 },
//		{ 0, 2, 4, 8 },
//		{ 3, 9, 0, 5 },
//		{ 2, 2, 7, 3 }
//	};
//
//	Matrix4 d1(d);
//
//	printMatrix4(Matrix4::identity());
//	printMatrix4(d1);
//
//	auto d2 = d1 * d1;
//
//	printMatrix4(d2);
//
//	auto d3 = d1 + d1;
//	auto d4 = d1 - d1;
//
//	printMatrix4(d3);
//	printMatrix4(d4);
//
//	Vector4 v(1, 2, 3, 4);
//
//	printVector4(v);
//
//	auto v1 = v * d1;
//	auto v2 = d1 * v;
//
//	printVector4(v1);
//	printVector4(v2);

	double edge = 6;
	auto position = Vector3(0, 0, -3);
	auto normal = Vector3(0, 0, 1);
	Vector3 point[4];
	auto half = edge / 2.0;
	point[0] = Vector3{-half, 0, half};
	point[1] = Vector3{half, 0, half};
	point[2] = Vector3{half, 0, -half};
	point[3] = Vector3{-half, 0, -half};

	for (const auto &i : point)
		printVector3(i);

	std::cout << "--------------------------------" << std::endl;

	const auto trans = Matrix4::transform(position.x, position.y, position.z);
	printMatrix4(trans);

	const auto up = Vector3(0, 1, 0);

	const auto theta = acos(up.x * normal.x + up.y * normal.y);
	const auto phi = acos(up.x * normal.x + up.z * normal.z);

	std::cout << "--------------------------------" << std::endl;
	std::cout << theta << " " << phi << std::endl;
	std::cout << "--------------------------------" << std::endl;

	// z軸中心の回転
	const auto theta_m =  Matrix4::rotate(2, theta);

	// x軸中心の回転
	const auto phi_m =  Matrix4::rotate(1, phi);

//	printMatrix4(theta_m);
//	printMatrix4(phi_m);

	const auto rot = theta_m * phi_m;

	for (auto &i : point) {
		auto vec4 = toVec4(i, 1);
		i = toVec3(trans * rot * vec4);
		printVector3(i);
	}
}
