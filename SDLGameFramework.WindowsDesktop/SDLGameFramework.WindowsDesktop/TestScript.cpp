#include "TestScript.h"
#include "Input.h"
#include "RenderManager.h"
#include "ColliderTestPF.h"
#include "Scene.h"
#include "Debug.h"
#include "Matrix4x4.h"

CoRet TestScript::CoFun_WaitSec()
{
	size_t i = 0;
	while (true)
	{
		Debug::Log(u8"协程测试: %d", i);
		Yield WaitForSeconds(2);
	}

	co_return 0;
}

void TestScript::Awake()
{
	auto mainCam = RenderManager::GetMainCamera();

	CoRet_WaitSec = CoFun_WaitSec();

	Obj obj{};
	auto i = (obj);		// 加括号，但是实体类型
	decltype(auto) a = obj;	// 加括号，但是左值引用类型
}

void TestScript::Update()
{
	// 在鼠标点击位置生成碰撞器
	if (Input::GetMouseButtonDown(2))
	{
		auto mainCam = RenderManager::GetMainCamera();
		Vector2 clickPos = Input::GetMousePosition();
		Vector2 worldPos = (*mainCam).WinToWorldPoint(clickPos);

		auto gobj = std::make_shared<ColliderTestPF>();
		Instantiate(gobj);
		gobj->GetTransform().SetPosition(worldPos);
		gobj->GetTransform().SetScale({ 3, 3 });
	}

	if (Input::GetKeyDown(SDL_SCANCODE_O))
	{
		CoRet_WaitSec.Stop();
	}

	float moveSpeed = 0.05f;
	if (Input::GetKey(SDL_SCANCODE_UP))
		mCamMove += Vector3::Up * moveSpeed;
	if (Input::GetKey(SDL_SCANCODE_DOWN))
		mCamMove += Vector3::Down * moveSpeed;
	if (Input::GetKey(SDL_SCANCODE_LEFT))
		mCamMove += Vector3::Left * moveSpeed;
	if (Input::GetKey(SDL_SCANCODE_RIGHT))
		mCamMove += Vector3::Right * moveSpeed;

	if (Input::GetKey(SDL_SCANCODE_A))
		GetTransform().SetPosition(GetTransform().GetPosition() + Vector2{-moveSpeed, 0.0f});
	if (Input::GetKey(SDL_SCANCODE_D))
		GetTransform().SetPosition(GetTransform().GetPosition() + Vector2{ moveSpeed, 0.0f });
	if (Input::GetKey(SDL_SCANCODE_W))
		GetTransform().SetRotation(GetTransform().GetRotation() + 5.0);
	if (Input::GetKey(SDL_SCANCODE_S))
		GetTransform().SetPosition(GetTransform().GetPosition() + Vector2{ 0.0f, moveSpeed });

	// 立方体顶点坐标
	std::array<Vector3, 8> cubeVertices = { {
		{-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}
	} };

	// 立方体边关系
	std::array<std::array<size_t, 2>, 12> cubeSides{{
		{ 0, 1 }, { 1, 3 }, { 2, 3 }, { 0, 2 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
		{ 4, 5 }, { 5, 7 }, { 6, 7 }, { 4, 6 }
		}};

	// 坐标轴三根线
	std::array<Vector3, 4> axisVertices = { {
		{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
	} };
	// 坐标轴轴与顶点关系：依次为XYZ
	std::array<std::array<size_t, 2>, 3> axisSides{{
		{0, 1}, { 0, 2 }, {0, 3}
		}};
	// 轴线颜色
	std::array<Color, 3> colors{{Color::Red, Color::Green, Color::Blue}};


	Matrix4x4 M = Matrix4x4::Eye();
	// 获取透视矩阵
	Matrix4x4 pM = Matrix4x4::Perspective(1000);		// 焦距
	// 视口矩阵
	Matrix4x4 vM = Matrix4x4::ViewMatrix(mCamMove + Vector3{ 0, 0, 10 }, { 0, 0, 0 });
	// 初始矩阵
	Matrix4x4 camM = pM * vM;


	Matrix4x4 OM = GetTransform().LocalToWorldMatrix();
	// 绘制世界坐标轴
	{
		// 变换XYZ坐标轴顶点
		std::array<Vector3, 4> newAxisVertices;
		for (size_t i = 0; i < axisVertices.size(); i++)
			newAxisVertices[i] = (camM * OM * axisVertices[i]).NormalizationThis();
		// 绘制XYZ坐标轴
		int i = 0;
		for (auto& axis : axisSides)
		{
			Vector3 v3[2] = { newAxisVertices[axis[0]], newAxisVertices[axis[1]] };
			Debug::DrawLine({ v3[0].X, v3[0].Y }, { v3[1].X, v3[1].Y }, colors.at(i++));
		}
	}


	// 方块相对变换
	Matrix4x4 cubeM = Matrix4x4::Translate({ 2, -1, 0 }) * M;

	// 绘制立方体中心坐标轴
	//{
	//	// 变换XYZ坐标轴顶点
	//	decltype(axisVertices) newAxisVertices;
	//	for (size_t i = 0; i < axisVertices.size(); i++)
	//		newAxisVertices[i] = (camM * cubeM * axisVertices[i]).NormalizationThis();
	//	// 绘制XYZ坐标轴
	//	int i = 0;
	//	for (auto& axis : axisSides)
	//	{
	//		Vector3 v3[2] = { newAxisVertices[axis[0]], newAxisVertices[axis[1]] };
	//		Debug::DrawLine({ v3[0].X, v3[0].Y }, { v3[1].X, v3[1].Y }, colors.at(i++));
	//	}
	//}

	// 绘制立方体
	//{
	//	// 变换立方体顶点
	//	decltype(cubeVertices) newCubeVertices;
	//	for (size_t i = 0; i < cubeVertices.size(); i++)
	//		newCubeVertices[i] = (camM * cubeM * cubeVertices[i]).NormalizationThis();
	//	// 绘制立方体
	//	for (auto& side : cubeSides)
	//	{
	//		Vector3 v3[2] = { newCubeVertices[side[0]], newCubeVertices[side[1]] };
	//		Debug::DrawLine({ v3[0].X, v3[0].Y }, { v3[1].X, v3[1].Y }, Color::Black);
	//	}
	//}

	bool b = true;
}

//Matrix MultiplyMatrix(Matrix A, Matrix B)
//{
//	Matrix C = CreateMatrix(A.row, B.col);
//
//	for (size_t ya = 0; ya < A.row; ya++)
//		for (size_t xb = 0; xb < B.col; xb++)
//			for (size_t yb = 0; yb < B.row; yb++)
//				C.mat[ya][xb] += A.mat[ya][yb] * B.mat[yb][xb];
//
//	return C;
//}
//
//Matrix4X4 IdentityMatrix()
//{	
//	Matrix4X4 M;
//	M.mat[0][0] = 1.0f;
//	M.mat[1][1] = 1.0f;
//	M.mat[2][2] = 1.0f;
//	M.mat[3][3] = 1.0f;
//	return M;
//}
//
//Matrix4X1 MultiplyMatrix(Matrix4X4 S, Matrix4X1 V)
//{
//	Matrix4X1 C;
//	
//	for (size_t ya = 0; ya < 4; ya++)
//		for (size_t xb = 0; xb < 1; xb++)
//			for (size_t yb = 0; yb < 4; yb++)
//				C.mat[ya][xb] += S.mat[ya][yb] * V.mat[yb][xb];
//
//	return C;
//}
//
//Matrix4X4 MultiplyMatrix(Matrix4X4 S, Matrix4X4 V)
//{
//	Matrix4X4 C;
//
//	for (size_t ya = 0; ya < 4; ya++)
//		for (size_t xb = 0; xb < 4; xb++)
//			for (size_t yb = 0; yb < 4; yb++)
//				C.mat[ya][xb] += S.mat[ya][yb] * V.mat[yb][xb];
//
//	return C;
//}
//
//Matrix4X1 HomogeneousCoordinates(Vector3 v3)
//{
//	Matrix4X1 M;
//	M.mat[0][0] = v3.X;
//	M.mat[1][0] = v3.Y;
//	M.mat[2][0] = v3.Z;
//	M.mat[3][0] = 1.0f;
//
//	return M;
//}
//
//Matrix4X1 Normalization(Matrix4X1 V)
//{
//	Matrix4X1 M;
//	M.mat[0][0] = V.mat[0][0] / V.mat[3][0];
//	M.mat[1][0] = V.mat[1][0] / V.mat[3][0];
//	M.mat[2][0] = V.mat[2][0] / V.mat[3][0];
//	M.mat[3][0] = V.mat[3][0] / V.mat[3][0];
//	return M;
//}
//
//Vector3 TransVector3(Matrix4X4 transM, Matrix4X4 camM, Vector3 V)
//{
//	// 矩阵乘法顺序：
//	// 变换矩阵 ：世界坐标 = 观察矩阵 * 变换矩阵 * 顶点坐标， 
//	// 视口矩阵：相机空间坐标 = 视口矩阵 * 世界坐标
//	// 透视投影矩阵：投影坐标 = 透视矩阵 * 相机空间坐标
//	Matrix4X1 mv3 = HomogeneousCoordinates(V);
//	mv3 = MultiplyMatrix(transM, mv3);
//	mv3 = MultiplyMatrix(camM, mv3);
//	mv3 = Normalization(mv3);
//	return {mv3.mat[0][0], mv3.mat[1][0], mv3.mat[2][0]};
//}
//
//Matrix4X4 Scale(Matrix4X4 M, Vector3 sv3)
//{
//	Matrix4X4 S;
//	S.mat[0][0] = sv3.X;
//	S.mat[1][1] = sv3.Y;
//	S.mat[2][2] = sv3.Z;
//	S.mat[3][3] = 1.0f;
//	return MultiplyMatrix(S, M);
//}
//
//Matrix4X4 RotateX(Matrix4X4 M, float a)
//{
//	Matrix4X4 R;
//	R.mat[1][1] = cosf(a);
//	R.mat[1][2] = -sinf(a);
//	R.mat[2][1] = sinf(a);
//	R.mat[2][2] = cosf(a);
//	R.mat[0][0] = 1.0f;
//	R.mat[3][3] = 1.0f;
//	return MultiplyMatrix(R, M);
//}
//
//Matrix4X4 RotateY(Matrix4X4 M, float a)
//{
//	// 注意：Y轴旋转矩阵sin的正负和其他旋转矩阵不同
//	Matrix4X4 R;
//	R.mat[0][0] = cosf(a);
//	R.mat[0][2] = sinf(a);
//	R.mat[2][0] = -sinf(a);
//	R.mat[2][2] = cosf(a);
//	R.mat[1][1] = 1.0f;
//	R.mat[3][3] = 1.0f;
//	return MultiplyMatrix(R, M);
//}
//
//Matrix4X4 RotateZ(Matrix4X4 M, float a)
//{
//	// Z轴指向我们，绕Z轴旋转时XY平面逆时针旋转
//	Matrix4X4 R;
//	R.mat[0][0] = cosf(a);
//	R.mat[0][1] = -sinf(a);
//	R.mat[1][0] = sinf(a);
//	R.mat[1][1] = cosf(a);
//	R.mat[2][2] = 1.0f;
//	R.mat[3][3] = 1.0f;
//	return MultiplyMatrix(R, M);
//}
//
//Matrix4X4 Translate(Matrix4X4 M, Vector3 tv3)
//{
//	Matrix4X4 T;
//	T.mat[0][3] = tv3.X;
//	T.mat[1][3] = tv3.Y;
//	T.mat[2][3] = tv3.Z;
//	T.mat[0][0] = 1.0f;
//	T.mat[1][1] = 1.0f;
//	T.mat[2][2] = 1.0f;
//	T.mat[3][3] = 1.0f;
//	return MultiplyMatrix(T, M);
//}
//
//Matrix4X4 ViewMatrix(Vector3 eye, Vector3 at)
//{
//	// 先将摄像机的坐标移动至世界原点
//	Matrix4X4 trans = { {
//		{1, 0, 0, -eye.X},
//		{0, 1, 0, -eye.Y},
//		{0, 0, 1, -eye.Z},
//		{0, 0, 0, 1} } };
//
//	// 观察点指向摄像机的 单位方向 作为摄像机Z轴单位方向
//	Vector3 ez = (eye - at).NormalizeNew();
//	// 获得垂直于 世界Y-ez平面 的ex轴
//	Vector3 ex = Vector3::Up.Cross(ez).NormalizeNew();
//	// 获得垂直于 ez-ex平面 的ey轴
//	Vector3 ey = ez.Cross(ex).NormalizeNew();
//
//	// 再将相机坐标轴旋转对齐世界坐标轴
//	// 可以理解为 变换矩阵M * 世界坐标轴WxWyWz = 摄像机坐标轴ExEyEz
//	// 现在要求一个变换矩阵Mt ，能够将摄像机坐标轴 转回到 世界坐标轴
//	// 这个Mt就是M的逆矩阵，即M的转置，（不会推导）
//	Matrix4X4 rot = { {
//		{ex.X, ex.Y, ex.Z, 0},
//		{ey.X, ey.Y, ey.Z, 0},
//		{ez.X, ez.Y, ez.Z, 0},
//		{0, 0, 0, 1} } };
//
//	Matrix4X4 view = MultiplyMatrix( rot,  trans);
//
//	return view;
//}
//
//Matrix4X4 Perspective(float n)
//{
//	Matrix4X4 M;
//	M.mat[0][0] = n;
//	M.mat[1][1] = n;
//	M.mat[2][2] = n;	// Z轴不用计算的
//	M.mat[3][2] = -1.0f;
//	//M.mat[3][3] = 1.0f;
//	return M;
//}
//
//Matrix CreateMatrix(size_t row, size_t col)
//{
//	Matrix M;
//	M.mat = new float* [row];
//	M.row = row;
//	M.col = col;
//	for (size_t i = 0; i < row; i++)
//		M.mat[i] = new float[col]();	// 自动清零
//
//	return M;
//}
//
//void DestroyMatrix(Matrix A)
//{
//	for (size_t i = 0; i < A.row; i++)
//		delete[] A.mat[i];
//
//	delete[] A.mat;
//	A.mat = nullptr;
//
//	A.col = A.row = 0;
//}
//
//void CopyMatrix(Matrix A, Matrix B)
//{
//	for (size_t y = 0; y < A.row; y++)
//		for (size_t x = 0; x < A.col; x++)
//			B.mat[y][x] = A.mat[y][x];
//}
