#pragma once
#include "MathBase.h"
#include <SDL.h>

struct Vector2
{
	float X = 0, Y = 0;

	static const Vector2 Zero;
	static const Vector2 One;
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;

	Vector2() : X(0), Y(0) {};
	Vector2(float _x, float _y) :X(_x), Y(_y) {};

	// 隐式转换SDL_Point
	inline operator SDL_Point() const
	{
		return { static_cast<int>(X), static_cast<int>(Y) };
	}
	inline operator SDL_FPoint() const
	{
		return { X, Y };
	}

	// 比较两个向量XY是否相等
	inline bool operator==(const Vector2& other) const
	{
		return X == other.X && Y == other.Y;
	}

	//向量相减
	inline Vector2 operator-(const Vector2& v2) const
	{
		Vector2 v(*this);		//创建临时对象
		v.X -= v2.X;				//运算
		v.Y -= v2.Y;
		return v;					//返回临时对象
	}

	//向量自减
	inline Vector2& operator-=(const Vector2& v2)
	{
		this->X -= v2.X;
		this->Y -= v2.Y;
		return *this;
	}

	//向量相加
	inline Vector2 operator+(const Vector2& v2) const
	{
		Vector2 v(*this);		//创建临时对象
		v.X += v2.X;				//运算
		v.Y += v2.Y;
		return v;					//返回临时对象
	}

	//向量自加
	inline Vector2& operator+=(const Vector2& v2)
	{
		this->X += v2.X;
		this->Y += v2.Y;
		return *this;
	};

	//向量标量乘法
	inline Vector2 operator*(const float f) const
	{
		Vector2 v(*this);		//创建临时对象
		v.X *= f;				//运算
		v.Y *= f;
		return v;					//返回临时对象
	}

	//两向量乘法（逐元素乘法，又称哈达玛积）
	// 如需使用点积和叉积，请使用dot和cross方法
	inline Vector2 operator*(const Vector2& v2) const
	{
		Vector2 v(*this);		//创建临时对象
		v.X *= v2.X;				//运算
		v.Y *= v2.Y;
		return v;					//返回临时对象
	}

	//向量标量自乘
	inline Vector2& operator*=(const float f)
	{
		this->X *= f;
		this->Y *= f;
		return *this;
	};

	//向量自乘（逐元素乘法，又称哈达玛积）
	// 如需使用点积和叉积，请使用dot和cross方法
	inline Vector2& operator*=(const Vector2& v2)
	{
		this->X *= v2.X;
		this->Y *= v2.Y;
		return *this;
	}

	//向量标量除法
	inline Vector2 operator/(const float f) const
	{
		Vector2 v(*this);		//创建临时对象
		v.X /= f;					//运算
		v.Y /= f;
		return v;					//返回临时对象
	}

	// 向量自除
	inline Vector2& operator/=(const Vector2& v2)
	{
		this->X /= v2.X;
		this->Y /= v2.Y;
		return *this;
	}

	//向量模长（未开方，通常用来比较大小而不在意实际长度）
	inline float LengthUNSquared() const
	{
		return (this->X * this->X + this->Y * this->Y);
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
	inline Vector2 NormalizeNew() const
	{
		Vector2 v(*this);
		v.NormalizeThis();

		return v;
	}

	// 向量点积（点积代数式求值）
	inline float Dot(const Vector2& v) const
	{
		return (this->X * v.X + this->Y * v.Y);
	};

	// 当前向量绝对值化
	inline Vector2& AbsThis()
	{
		this->X = fabsf(this->X);
		this->Y = fabsf(this->Y);

		return *this;
	}
	// 取向量绝对值
	inline Vector2 AbsNew() const
	{
		Vector2 v;		//创建临时对象
		v.X = fabsf(this->X);
		v.Y = fabsf(this->Y);
		return v;					//返回临时对象
	}

	/// <summary>
	/// 解析一元一次函数并通过X计算Y
	/// </summary>
	/// <param name="X">传入自变量X</param>
	/// <param name="v1">函数上的一个点</param>
	/// <param name="v2">函数上的另一个点</param>
	/// <returns>算出的因变量Y</returns>
	static inline float LinearEquation(float X, const Vector2& v1, const Vector2& v2)
	{
		float k = (v1.Y - v2.Y) / (v1.X - v2.X);
		float b = v1.Y - k * v1.X;

		return k * X + b;
	}
};


