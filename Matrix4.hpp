//
// Created by 長井亨 on 2018/07/10.
//

#ifndef PATHTRACING_MATRIX4_HPP
#define PATHTRACING_MATRIX4_HPP

namespace nagato
{
	class Matrix4
	{
	public:
			double data[4][4] = {0};

			Matrix4()
			= default;

			explicit Matrix4(double a)
			{
				for (auto &i : data)
					for (double &j : i)
						j = a;
			}

			explicit Matrix4(double a[4][4])
			{
				for (int i = 0 ;i < 4; i++)
					for (int j= 0; j < 4; j++)
						data[i][j] = a[i][j];
			}

			inline Matrix4 operator+(Matrix4 a, Matrix4 b)
			{
				Matrix4 m;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						m.data[i][j] = a.data[i][j] + b.data[i][j];
					}
				}
				return m;
			}

			inline Matrix4 operator-(Matrix4 a, Matrix4 b)
			{
				Matrix4 m;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						m.data[i][j] = a.data[i][j] - b.data[i][j];
					}
				}
				return m;
			}

			inline Matrix4 operator*(Matrix4 a, Matrix4 b)
			{
				Matrix4 m;

				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						for (int k = 0; k < 4; k++) {
							m.data[i][j] += a.data[i][k] * b.data[k][j];
						}
					}
				}
				return m;
			}

			inline static Matrix4 identity()
			{
				Matrix4 m;

				for (int i = 0; i < 4; i++)
					m.data[i][i] = 0;

				return m;
			}
	};
}

#endif //PATHTRACING_MATRIX4_HPP
