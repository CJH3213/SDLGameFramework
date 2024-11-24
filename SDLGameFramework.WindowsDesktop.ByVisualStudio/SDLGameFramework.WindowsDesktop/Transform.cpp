#include "Transform.h"

void Transform::SeverWithParent()
{
	// �Ƴ�֮ǰ���õĸ���ϵ
	if (mParentTrans != nullptr)
	{
		auto& parentChildren = mParentTrans->mChildren;
		auto it = std::find(parentChildren.begin(), parentChildren.end(), this);
		if (it != parentChildren.end())
			parentChildren.erase(it);

		mParentTrans = nullptr;
	}
}

Vector2 Transform::GetPosition() const
{
	//Transform* parent = mParentTrans;
	//Vector2 sumPos;
	//Vector2 locPos = mLocalPosition;

	//while (parent)
	//{
	//	double a = parent->mLocalRotation;
	//	a = fmod(a, 360.0) / 360.0 * 2.0 * M_PI;
	//	double cosa = cos(a), sina = sin(a);
	//	Vector2 newPos;
	//	// ��ʱ�� ��ת����
	//	newPos.X = locPos.X * cosa - locPos.Y * sina;
	//	newPos.Y = locPos.X * sina + locPos.Y * cosa;

	//	sumPos += newPos;
	//	locPos = parent->mLocalPosition;
	//	parent = parent->mParentTrans;
	//}

	//sumPos += locPos;

	//if(mParentTrans)
	//	sumPos *= mParentTrans->GetScale();

	//return sumPos;

	auto M = Matrix4x4::Eye();
	if (mParentTrans)
		M = mParentTrans->LocalToWorldMatrix();
	Vector3 M2 = M * Vector3{ mLocalPosition.X, mLocalPosition.Y, 0.0f };
	return M2;
}

void Transform::SetPosition(Vector2 p)
{
	//Transform* parent = mParentTrans;
	//while (parent)
	//{
	//	p -= parent->mLocalPosition;
	//	parent = parent->mParentTrans;
	//}

	Vector2 newPos = p;

	if (mParentTrans)
	{
		Vector2 parentPos;
		double a = 0;

		parentPos = mParentTrans->GetPosition();
		a = mParentTrans->GetRotation();

		a = fmod(a, 360.0);
		a = a / 360.0 * 2.0 * M_PI;
		double cosa = cos(a);
		double sina = sin(a);
		p -= parentPos;
		// ˳ʱ�� ��ת����
		newPos.X = p.X * cosa + p.Y * sina;
		newPos.Y = p.X * (- sina ) + p.Y * cosa;
	}

	if (mParentTrans)
		newPos /= mParentTrans->GetScale();

	mLocalPosition = newPos;
}

void Transform::SetRotation(float f)
{
	Transform* parent = mParentTrans;
	while (parent)
	{
		f -= parent->mLocalRotation;
		parent = parent->mParentTrans;
	}
	mLocalRotation = f;
}

float Transform::GetRotation() const
{
	Transform* parent = mParentTrans;
	float rotSum = mLocalRotation;
	while (parent)
	{
		rotSum += parent->mLocalRotation;
		parent = parent->mParentTrans;
	}
	return fmod(rotSum, 360);
}

void Transform::SetScale(Vector2 v)
{
	Transform* parent = mParentTrans;
	while (parent)
	{
		v /= parent->mLocalScale;
		parent = parent->mParentTrans;
	}
	mLocalScale = v;
}

void Transform::SetParent(Transform* parent)
{
	// �Ƴ�֮ǰ���õĸ���ϵ
	SeverWithParent();

	// ��¼�¸�����ı任���
	mParentTrans = parent;

	// �򸸶����¼�Ӷ���
	if (parent != nullptr)
		parent->mChildren.emplace_back(this);
}

Matrix4x4 Transform::LocalToWorldMatrix()
{
	if (mParentTrans == nullptr)
		return Matrix4x4::Translate({ mLocalPosition.X, mLocalPosition.Y, 0.0f })
		* Matrix4x4::RotateZ(fmod(mLocalRotation, 360.0) / 180.0 * M_PI)
		* Matrix4x4::Scale({ mLocalScale.X, mLocalScale.Y, 1.0f });

	return  mParentTrans->LocalToWorldMatrix()
		* Matrix4x4::Translate({ mLocalPosition.X, mLocalPosition.Y, 0.0f })
		* Matrix4x4::RotateZ(fmod(mLocalRotation, 360.0) / 180.0 * M_PI)
		* Matrix4x4::Scale({ mLocalScale.X, mLocalScale.Y, 1.0f });
}

void Transform::OnDestroy()
{
	// �����ǰ������Ӷ����ϵ���Ӷ������һ����������
	for (auto child : mChildren)
	{
		child->mParentTrans = mParentTrans;
	}

	// �Ƴ�֮ǰ���õĸ���ϵ
	SeverWithParent();
}

Vector2 Transform::GetScale() const
{
	Transform* parent = mParentTrans;
	Vector2 scaSum = mLocalScale;
	while (parent)
	{
		scaSum *= parent->mLocalScale;
		parent = parent->mParentTrans;
	}

	return scaSum;
}
