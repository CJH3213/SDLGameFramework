#pragma once
#include "Vector2.h"

// ��������ϵ
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
	static const Vector3 Up;				// �Ϸ�������Ļ�Ϸ���Y��
	static const Vector3 Down;		// �·�������Ļ�·�����Y��
	static const Vector3 Left;			// ��������Ļ��࣬-X��
	static const Vector3 Right;		// �ҷ�������Ļ�Ҳ࣬X��
	static const Vector3 Forward;	// ǰ����Զ���Լ���-Z��
	static const Vector3 Back;			// ���򣺽ӽ��Լ���Z��

	Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {};
	Vector3(float _x, float _y, float _z) :X(_x), Y(_y), Z(_z) {};

	// ��ʽת��Vector2
	Vector3(const Vector2& v2) :X(v2.X), Y(v2.Y), Z(0.0f) {};
	inline operator Vector2() const{return { X, Y };}

	// �Ƚ���������XY�Ƿ����
	inline bool operator==(const Vector3& other) const
	{
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	//�������
	inline Vector3 operator-(const Vector3& v3) const
	{
		Vector3 v(*this);		//������ʱ����
		v.X -= v3.X;				//����
		v.Y -= v3.Y;
		v.Z -= v3.Z;
		return v;					//������ʱ����
	}

	//�����Լ�
	inline Vector3& operator-=(const Vector3& v3)
	{
		this->X -= v3.X;
		this->Y -= v3.Y;
		this->Z -= v3.Z;
		return *this;
	}

	//�������
	inline Vector3 operator+(const Vector3& v3) const
	{
		Vector3 v(*this);		//������ʱ����
		v.X += v3.X;				//����
		v.Y += v3.Y;
		v.Z += v3.Z;
		return v;					//������ʱ����
	}

	//�����Լ�
	inline Vector3& operator+=(const Vector3& v3)
	{
		this->X += v3.X;
		this->Y += v3.Y;
		this->Z += v3.Z;
		return *this;
	};

	//���������˷�
	inline Vector3 operator*(const float f) const
	{
		Vector3 v(*this);		//������ʱ����
		v.X *= f;				//����
		v.Y *= f;
		v.Z *= f;
		return v;					//������ʱ����
	}

	//�������˷�����Ԫ�س˷����ֳƹ��������
	// ����ʹ�õ���Ͳ������ʹ��dot��cross����
	inline Vector3 operator*(const Vector3& v3) const
	{
		Vector3 v(*this);		//������ʱ����
		v.X *= v3.X;				//����
		v.Y *= v3.Y;
		v.Z *= v3.Z;
		return v;					//������ʱ����
	}

	//���������Գ�
	inline Vector3& operator*=(const float f)
	{
		this->X *= f;
		this->Y *= f;
		this->Z *= f;
		return *this;
	};

	//�����Գˣ���Ԫ�س˷����ֳƹ��������
	// ����ʹ�õ���Ͳ������ʹ��dot��cross����
	inline Vector3& operator*=(const Vector3& v3)
	{
		this->X *= v3.X;
		this->Y *= v3.Y;
		this->Z *= v3.Z;
		return *this;
	}

	//������������
	inline Vector3 operator/(const float f) const
	{
		Vector3 v(*this);		//������ʱ����
		v.X /= f;					//����
		v.Y /= f;
		v.Z /= f;
		return v;					//������ʱ����
	}

	// �����Գ�
	inline Vector3& operator/=(const Vector3& v3)
	{
		this->X /= v3.X;
		this->Y /= v3.Y;
		this->Z /= v3.Z;
		return *this;
	}

	//����ģ����δ������ͨ�������Ƚϴ�С��������ʵ�ʳ��ȣ�
	inline float LengthUNSquared() const
	{
		return (this->X * this->X + this->Y * this->Y + this->Z * this->Z);
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
	inline Vector3 NormalizeNew() const
	{
		Vector3 v(*this);
		v.NormalizeThis();

		return v;
	}

	// ����������������ʽ��ֵ��
	inline float Dot(const Vector3& v) const
	{
		return (this->X * v.X + this->Y * v.Y + this->Z * v.Z);
	};

	// �����������ֱ�����������ĵ�����������
	inline Vector3 Cross(const Vector3& rv) const
	{
		return { Y * rv.Z - Z * rv.Y, Z * rv.X - X * rv.Z, X * rv.Y - Y * rv.X };
	};

	// ��ǰ��������ֵ��
	inline Vector3& AbsThis()
	{
		this->X = fabsf(this->X);
		this->Y = fabsf(this->Y);
		this->Z = fabsf(this->Z);

		return *this;
	}
	// ȡ��������ֵ
	inline Vector3 AbsNew() const
	{
		Vector3 v;		//������ʱ����
		v.X = fabsf(this->X);
		v.Y = fabsf(this->Y);
		v.Z = fabsf(this->Z);
		return v;					//������ʱ����
	}
};

