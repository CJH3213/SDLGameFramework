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

	// ��ʽת��SDL_Point
	inline operator SDL_Point() const
	{
		return { static_cast<int>(X), static_cast<int>(Y) };
	}
	inline operator SDL_FPoint() const
	{
		return { X, Y };
	}

	// �Ƚ���������XY�Ƿ����
	inline bool operator==(const Vector2& other) const
	{
		return X == other.X && Y == other.Y;
	}

	//�������
	inline Vector2 operator-(const Vector2& v2) const
	{
		Vector2 v(*this);		//������ʱ����
		v.X -= v2.X;				//����
		v.Y -= v2.Y;
		return v;					//������ʱ����
	}

	//�����Լ�
	inline Vector2& operator-=(const Vector2& v2)
	{
		this->X -= v2.X;
		this->Y -= v2.Y;
		return *this;
	}

	//�������
	inline Vector2 operator+(const Vector2& v2) const
	{
		Vector2 v(*this);		//������ʱ����
		v.X += v2.X;				//����
		v.Y += v2.Y;
		return v;					//������ʱ����
	}

	//�����Լ�
	inline Vector2& operator+=(const Vector2& v2)
	{
		this->X += v2.X;
		this->Y += v2.Y;
		return *this;
	};

	//���������˷�
	inline Vector2 operator*(const float f) const
	{
		Vector2 v(*this);		//������ʱ����
		v.X *= f;				//����
		v.Y *= f;
		return v;					//������ʱ����
	}

	//�������˷�����Ԫ�س˷����ֳƹ��������
	// ����ʹ�õ���Ͳ������ʹ��dot��cross����
	inline Vector2 operator*(const Vector2& v2) const
	{
		Vector2 v(*this);		//������ʱ����
		v.X *= v2.X;				//����
		v.Y *= v2.Y;
		return v;					//������ʱ����
	}

	//���������Գ�
	inline Vector2& operator*=(const float f)
	{
		this->X *= f;
		this->Y *= f;
		return *this;
	};

	//�����Գˣ���Ԫ�س˷����ֳƹ��������
	// ����ʹ�õ���Ͳ������ʹ��dot��cross����
	inline Vector2& operator*=(const Vector2& v2)
	{
		this->X *= v2.X;
		this->Y *= v2.Y;
		return *this;
	}

	//������������
	inline Vector2 operator/(const float f) const
	{
		Vector2 v(*this);		//������ʱ����
		v.X /= f;					//����
		v.Y /= f;
		return v;					//������ʱ����
	}

	// �����Գ�
	inline Vector2& operator/=(const Vector2& v2)
	{
		this->X /= v2.X;
		this->Y /= v2.Y;
		return *this;
	}

	//����ģ����δ������ͨ�������Ƚϴ�С��������ʵ�ʳ��ȣ�
	inline float LengthUNSquared() const
	{
		return (this->X * this->X + this->Y * this->Y);
	};

	//����ģ����������ʵ�ʳ��ȣ��ȽϺ�ʱ��
	inline float LengthSquared() const
	{
		return sqrtf(LengthUNSquared());
	};

	//����������׼������������תΪ��λ����1��
	inline bool NormalizeThis()
	{
		//�������ǰ����ģ��
		float lengthS = LengthSquared();

		//��������ӽ�0����Ҫִ�г���
		if (Math::NearZero(lengthS))
			return false;

		lengthS = 1 / lengthS;		//ȷ����ĸ��ģ��������0�����Լ��㵹��
		(*this) *= lengthS;			//��ǰ�����Գ�ģ���ĵ��������ڳ�������	

		return true;
	}

	//����һ����������׼������������������תΪ��λ����1��
	inline Vector2 NormalizeNew() const
	{
		Vector2 v(*this);
		v.NormalizeThis();

		return v;
	}

	// ����������������ʽ��ֵ��
	inline float Dot(const Vector2& v) const
	{
		return (this->X * v.X + this->Y * v.Y);
	};

	// ��ǰ��������ֵ��
	inline Vector2& AbsThis()
	{
		this->X = fabsf(this->X);
		this->Y = fabsf(this->Y);

		return *this;
	}
	// ȡ��������ֵ
	inline Vector2 AbsNew() const
	{
		Vector2 v;		//������ʱ����
		v.X = fabsf(this->X);
		v.Y = fabsf(this->Y);
		return v;					//������ʱ����
	}

	/// <summary>
	/// ����һԪһ�κ�����ͨ��X����Y
	/// </summary>
	/// <param name="X">�����Ա���X</param>
	/// <param name="v1">�����ϵ�һ����</param>
	/// <param name="v2">�����ϵ���һ����</param>
	/// <returns>����������Y</returns>
	static inline float LinearEquation(float X, const Vector2& v1, const Vector2& v2)
	{
		float k = (v1.Y - v2.Y) / (v1.X - v2.X);
		float b = v1.Y - k * v1.X;

		return k * X + b;
	}
};


