#include "Matrix4x4.h"

Matrix4x4::Matrix4x4(std::initializer_list<float> list)
{
	float* p = &Mat[0][0];
	for (auto f : list)
		*p++ = f;
}

Matrix4x4 Matrix4x4::ViewMatrix(Vector3 eye, Vector3 at)
{
	// 先将摄像机的坐标移动至世界原点
	Matrix4x4 trans = {
		1, 0, 0, -eye.X,
		0, 1, 0, -eye.Y,
		0, 0, 1, -eye.Z,
		0, 0, 0, 1 };

	// 观察点指向摄像机的 单位方向 作为摄像机Z轴单位方向
	Vector3 ez = (eye - at).NormalizeNew();
	// 获得垂直于 世界Y-ez平面 的ex轴
	Vector3 ex = Vector3::Up.Cross(ez).NormalizeNew();
	// 获得垂直于 ez-ex平面 的ey轴
	Vector3 ey = ez.Cross(ex).NormalizeNew();

	// 再将相机坐标轴旋转对齐世界坐标轴
	// 可以理解为 变换矩阵M * 世界坐标轴WxWyWz = 摄像机坐标轴ExEyEz
	// 现在要求一个变换矩阵Mt ，能够将摄像机坐标轴 转回到 世界坐标轴
	// 这个Mt就是M的逆矩阵，即M的转置，（不会推导）
	Matrix4x4 rot = {
		ex.X,  ex.Y,  ex.Z, 0,
		ey.X,  ey.Y,  ey.Z, 0,
		ez.X,  ez.Y,  ez.Z, 0,
		0, 0, 0, 1};

	Matrix4x4 view = rot * trans;

	return view;
}
