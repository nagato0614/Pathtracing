//
// Created by 長井亨 on 2018/07/10.
//

#ifndef PATHTRACING_VECTOR4_HPP
#define PATHTRACING_VECTOR4_HPP

#include <cmath>
#include "Matrix4.hpp"

namespace nagato {
	class Vector4
	{
	public:
			double x;
			double y;
			double z;
			double w;

			Vector4(double v = 0)
					: Vector4(v, v, v, v)
			{}

			Vector4(double x, double y, double z, double w)
					: x(x), y(y), z(z), w(w)
			{}
			double operator[](int i)
			{
				return (&x)[i];
			}

			inline Vector4 operator+(Vector4 b) const
			{
				return Vector4(x + b.x, y + b.y, z + b.z, w + b.w);
			}

			inline Vector4 operator-(Vector4 b) const
			{
				return Vector4(x - b.x, y - b.y, z - b.z, w - b.w);
			}

			inline Vector4 operator*(Vector4 b) const
			{
				return Vector4(x * b.x, y * b.y, z * b.z, w * b.w);
			}

			inline Vector4 operator*(double a) const
			{
				return Vector4(x * a, y * a, z * a, w * a);
			}

			inline Vector4 operator*(Vector4 a, Matrix4 b)
			{
				Vector4 v(0);

				v.x = a.x * b.data[0][0] + a.y * b.data[1][0] + a.z * b.data[2][0];
				v.y = a.x * b.data[0][1] + a.y * b.data[1][1] + a.z * b.data[2][1];
				v.z = a.x * b.data[0][2] + a.y * b.data[1][2] + a.z * b.data[2][2];

				return v;
			}

			inline Vector4 operator*(Matrix4 a, Vector4 b)
			{
				Vector4 v(0);

				v.x = a.data[0][0] * b.x + a.data[0][1] * b.y + a.data[0][1] * b.z;
				v.y = a.data[1][0] * b.x + a.data[1][1] * b.y + a.data[1][1] * b.z;
				v.z = a.data[2][0] * b.x + a.data[2][1] * b.y + a.data[2][1] * b.z;

				return v;
			}

			inline Vector4 operator/(Vector4 b) const
			{
				return Vector4(x / b.x, y / b.y, z / b.z, w / b.w);
			}

			inline Vector4 operator/(double b) const
			{
				return Vector4(x / b, y / b, z / b, w / b);
			}

			inline Vector4 operator-() const
			{
				return Vector4(-x, -y, -z, -w);
			}

	};

	inline double dot(Vector4 a, Vector4 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * a.w;
	}

//	inline Vector4 cross(Vector4 a, Vector4 b)
//	{
//		return Vector4(a.y * b.z - a.z * b.y,
//									 a.z * b.x - a.x * b.z,
//									 a.x * b.y - a.y * b.x);
//	}

	inline Vector4 normalize(Vector4 v)
	{
		return v / sqrt(dot(v, v));
	}
}

#endif //PATHTRACING_VECTOR4_HPP
