//
// Created by kiki on 2018/06/26.
//

#ifndef PATHTRACING_VECTOR_HPP
#define PATHTRACING_VECTOR_HPP

namespace nagato {
	class Vector
	{
	public:
			double x;
			double y;
			double z;
			Vector(double v = 0)
					: Vector(v, v, v)
			{}
			Vector(double x, double y, double z)
					: x(x), y(y), z(z)
			{}
			double operator[](int i)
			{
				return (&x)[i];
			}

			inline Vector operator+(Vector b) const
			{
				return Vector(x + b.x, y + b.y, z + b.z);
			}

			inline Vector operator-(Vector b) const
			{
				return Vector(x - b.x, y - b.y, z - b.z);
			}

			inline Vector operator*(Vector b) const
			{
				return Vector(x * b.x, y * b.y, z * b.z);
			}

			inline Vector operator*(double a) const
			{
				return Vector(x * a, y * a, z * a);
			}

			inline Vector operator/(Vector b) const
			{
				return Vector(x / b.x, y / b.y, z / b.z);
			}

			inline Vector operator/(double b) const
			{
				return Vector(x / b, y / b, z / b);
			}

			inline Vector operator-() const
			{
				return Vector(-x, -y, -z);
			}

	};

	inline double dot(Vector a, Vector b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline Vector cross(Vector a, Vector b)
	{
		return Vector(a.y * b.z - a.z * b.y,
									a.z * b.x - a.x * b.z,
									a.x * b.y - a.y * b.x);
	}

	inline Vector normalize(Vector v)
	{
		return v / sqrt(dot(v, v));
	}
}
#endif //PATHTRACING_VECTOR_HPP
