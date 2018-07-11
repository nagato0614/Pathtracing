//
// Created by kiki on 2018/06/26.
//

#ifndef PATHTRACING_VECTOR_HPP
#define PATHTRACING_VECTOR_HPP

#include "Vector4.hpp"

namespace nagato {
	class Vector3
	{
	public:
			double x;
			double y;
			double z;
			Vector3(double v = 0)
					: Vector3(v, v, v)
			{}
			Vector3(double x, double y, double z)
					: x(x), y(y), z(z)
			{}
			double operator[](int i)
			{
				return (&x)[i];
			}

			inline Vector3 operator+(Vector3 b) const
			{
				return Vector3(x + b.x, y + b.y, z + b.z);
			}

			inline Vector3 operator-(Vector3 b) const
			{
				return Vector3(x - b.x, y - b.y, z - b.z);
			}

			inline Vector3 operator*(Vector3 b) const
			{
				return Vector3(x * b.x, y * b.y, z * b.z);
			}

			inline Vector3 operator*(double a) const
			{
				return Vector3(x * a, y * a, z * a);
			}

			inline Vector3 operator/(Vector3 b) const
			{
				return Vector3(x / b.x, y / b.y, z / b.z);
			}

			inline Vector3 operator/(double b) const
			{
				return Vector3(x / b, y / b, z / b);
			}

			inline Vector3 operator-() const
			{
				return Vector3(-x, -y, -z);
			}

	};

	inline Vector4 toVec4(Vector3 v, double a = 1) {
		return Vector4(v.x, v.y, v.z, a);
	}

	inline Vector3 toVec3(Vector4 v)
	{
		return Vector3(v.x / v.w, v.y / v.w, v.z / v.w);
	}

	inline double dot(Vector3 a, Vector3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline Vector3 cross(Vector3 a, Vector3 b)
	{
		return Vector3(a.y * b.z - a.z * b.y,
									 a.z * b.x - a.x * b.z,
									 a.x * b.y - a.y * b.x);
	}

	inline Vector3 normalize(Vector3 v)
	{
		return v / sqrt(dot(v, v));
	}
}

#endif //PATHTRACING_VECTOR_HPP
