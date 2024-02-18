#include "Matrix4x4.h"

Matrix4x4::Matrix4x4(std::initializer_list<float> list)
{
	float* p = &Mat[0][0];
	for (auto f : list)
		*p++ = f;
}

Matrix4x4 Matrix4x4::ViewMatrix(Vector3 eye, Vector3 at)
{
	// �Ƚ�������������ƶ�������ԭ��
	Matrix4x4 trans = {
		1, 0, 0, -eye.X,
		0, 1, 0, -eye.Y,
		0, 0, 1, -eye.Z,
		0, 0, 0, 1 };

	// �۲��ָ��������� ��λ���� ��Ϊ�����Z�ᵥλ����
	Vector3 ez = (eye - at).NormalizeNew();
	// ��ô�ֱ�� ����Y-ezƽ�� ��ex��
	Vector3 ex = Vector3::Up.Cross(ez).NormalizeNew();
	// ��ô�ֱ�� ez-exƽ�� ��ey��
	Vector3 ey = ez.Cross(ex).NormalizeNew();

	// �ٽ������������ת��������������
	// �������Ϊ �任����M * ����������WxWyWz = �����������ExEyEz
	// ����Ҫ��һ���任����Mt ���ܹ�������������� ת�ص� ����������
	// ���Mt����M������󣬼�M��ת�ã��������Ƶ���
	Matrix4x4 rot = {
		ex.X,  ex.Y,  ex.Z, 0,
		ey.X,  ey.Y,  ey.Z, 0,
		ez.X,  ez.Y,  ez.Z, 0,
		0, 0, 0, 1};

	Matrix4x4 view = rot * trans;

	return view;
}
