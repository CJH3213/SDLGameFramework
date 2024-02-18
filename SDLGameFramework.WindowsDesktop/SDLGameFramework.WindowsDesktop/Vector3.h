#pragma once
#include "Vector2.h"

// 右手坐标系
//				y
//				|
//				|_______x
//			   /
//			  /
//			z

class Vector3
{
public:
	float X, Y, Z;

	static const Vector3 Zero;
	static const Vector3 One;
	static const Vector3 Up;				// 上方向：向屏幕上方，Y轴
	static const Vector3 Down;		// 下方向：向屏幕下方，负Y轴
	static const Vector3 Left;			// 左方向：向屏幕左侧，-X轴
	static const Vector3 Right;		// 右方向：向屏幕右侧，X轴
	static const Vector3 Forward;	// 前方向：远离自己，-Z轴
	static const Vector3 Back;			// 后方向：接近自己，Z轴

	Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {};
	Vector3(float _x, float _y, float _z) :X(_x), Y(_y), Z(_z) {};

	// 隐式转换Vector2
	Vector3(const Vector2& v2) :X(v2.X), Y(v2.Y), Z(0.0f) {};
	inline operator Vector2() const{return { X, Y };}

	// 比较两个向量XY是否相等
	inline bool operator==(const Vector3& other) const
	{
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	//向量相减
	inline Vector3 operator-(const Vector3& v3) const
	{
		Vector3 v(*this);		//创建临时对象
		v.X -= v3.X;				//运算
		v.Y -= v3.Y;
		v.Z -= v3.Z;
		return v;					//返回临时对象
	}

	//向量自减
	inline Vector3& operator-=(const Vector3& v3)
	{
		this->X -= v3.X;
		this->Y -= v3.Y;
		this->Z -= v3.Z;
		return *this;
	}

	//向量相加
	inline Vector3 operator+(const Vector3& v3) const
	{
		Vector3 v(*this);		//创建临时对象
		v.X += v3.X;				//运算
		v.Y += v3.Y;
		v.Z += v3.Z;
		return v;					//返回临时对象
	}

	//向量自加
	inline Vector3& operator+=(const Vector3& v3)
	{
		this->X += v3.X;
		this->Y += v3.Y;
		this->Z += v3.Z;
		return *this;
	};

	//向量标量乘法
	inline Vector3 operator*(const float f) const
	{
		Vector3 v(*this);		//创建临时对象
		v.X *= f;				//运算
		v.Y *= f;
		v.Z *= f;
		return v;					//返回临时对象
	}

	//两向量乘法（逐元素乘法，又称哈达玛积）
	// 如需使用点积和叉积，请使用dot和cross方法
	inline Vector3 operator*(const Vector3& v3) const
	{
		Vector3 v(*this);		//创建临时对象
		v.X *= v3.X;				//运算
		v.Y *= v3.Y;
		v.Z *= v3.Z;
		return v;					//返回临时对象
	}

	//向量标量自乘
	inline Vector3& operator*=(const float f)
	{
		this->X *= f;
		this->Y *= f;
		this->Z *= f;
		return *this;
	};

	//向量自乘（逐元素乘法，又称哈达玛积）
	// 如需使用点积和叉积，请使用dot和cross方法
	inline Vector3& operator*=(const Vector3& v3)
	{
		this->X *= v3.X;
		this->Y *= v3.Y;
		this->Z *= v3.Z;
		return *this;
	}

	//向量标量除法
	inline Vector3 operator/(const float f) const
	{
		Vector3 v(*this);		//创建临时对象
		v.X /= f;					//运算
		v.Y /= f;
		v.Z /= f;
		return v;					//返回临时对象
	}

	// 向量自除
	inline Vector3& operator/=(const Vector3& v3)
	{
		this->X /= v3.X;
		this->Y /= v3.Y;
		this->Z /= v3.Z;
		return *this;
	}

	//向量模长（未开方，通常用来比较大小而不在意实际长度）
	inline float LengthUNSquared() const
	{
		return (this->X * this->X + this->Y * this->Y + this->Z * this->Z);
	};

	//向量模长（开方，实际长度，比较耗时）
	inline float LengthSquared() const
	{
		return sqrtf(LengthUNSquared());
	};

	//将本向量标准化（向量长度转为单位长度1）
	inline bool NormalizeThis()
	{
		//先算出当前向量模长
		float lengthS = LengthSquared();

		//如果除数接近0，不要执行除法
		if (Math::NearZero(lengthS))
			return false;

		lengthS = 1 / lengthS;		//确定分母（模长）不是0，可以计算倒数
		(*this) *= lengthS;			//当前向量自乘模长的倒数，等于除法运算	

		return true;
	}

	//返回一个本向量标准化的向量（向量长度转为单位长度1）
	inline Vector3 NormalizeNew() const
	{
		Vector3 v(*this);
		v.NormalizeThis();

		return v;
	}

	// 向量点积（点积代数式求值）
	inline float Dot(const Vector3& v) const
	{
		return (this->X * v.X + this->Y * v.Y + this->Z * v.Z);
	};

	// 向量叉积（垂直于两个向量的第三个向量）
	inline Vector3 Cross(const Vector3& rv) const
	{
		return { Y * rv.Z - Z * rv.Y, Z * rv.X - X * rv.Z, X * rv.Y - Y * rv.X };
	};

	// 当前向量绝对值化
	inline Vector3& AbsThis()
	{
		this->X = fabsf(this->X);
		this->Y = fabsf(this->Y);
		this->Z = fabsf(this->Z);

		return *this;
	}
	// 取向量绝对值
	inline Vector3 AbsNew() const
	{
		Vector3 v;		//创建临时对象
		v.X = fabsf(this->X);
		v.Y = fabsf(this->Y);
		v.Z = fabsf(this->Z);
		return v;					//返回临时对象
	}
};

