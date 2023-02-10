#include "GameScene.h"
#include "Collision.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

template<class T>
void safe_delete(T*& p) {
	delete p;
	p = nullptr;
}

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	// 3Eオブジェクトの解放
	safe_delete(objSphere);
	safe_delete(objFighter2);
	// モデルデータの解放
	safe_delete(modelFighter);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	//Object3d::SetEye(XMFLOAT3(1, 1, 1));

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	// 球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1); // 中心点座標
	sphere.radius = 1.0f; // 半径

	// 平面の初期値を設定
	plane.normal = XMVectorSet(0, 0.5f, 0, 0); // 法線ベクトル
	plane.distance = 0.0f; // 原点(0,0,0)からの距離

	// 三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1); // 左手前
	triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1); // 左奥
	triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1); // 左手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0); // 上向き

	// 3Dオブジェクト生成
	object3d = Object3d::Create();
	object3d->Update();

	// .objの名前を指定してモデルを読み込む
	modelSphere = Model::LoadFromOBJ("sphere");
	modelGround = Model::LoadFromOBJ("ground");
	modelSkydome = Model::LoadFromOBJ("skydome");

	// 3Dオブジェクトの生成
	objSphere = Object3d::Create();
	//objFighter2 = Object3d::Create();
	objGround = Object3d::Create();
	objSkydome = Object3d::Create();

	// 3Dオブジェクトにモデルを割り当てる
	objSphere->SetModel(modelSphere);
	//objFighter2->SetModel(modelFighter);
	objGround->SetModel(modelGround);
	objSkydome->SetModel(modelSkydome);

	objSphere->SetPosition(XMFLOAT3(0, 0, 100));
	objGround->SetPosition(XMFLOAT3(0, -20, 0));
	objSkydome->SetPosition({ 0,-20,0 });
	objSkydome->SetScale({ 5,5,5 });
}

void GameScene::Update()
{
	// 3Dオブジェクトの更新
	objSphere->Update();
	//objFighter2->Update();
	objGround->Update();
	objSkydome->Update();

	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// 現在の座標を取得
		XMFLOAT3 position = object3d->GetPosition();

		// 移動後の座標を計算
		if (input->PushKey(DIK_UP)) { position.y += 1.0f; }
		else if (input->PushKey(DIK_DOWN)) { position.y -= 1.0f; }
		if (input->PushKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { position.x -= 1.0f; }

		// 座標の変更を反映
		object3d->SetPosition(position);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W)) { Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f }); }
		else if (input->PushKey(DIK_S)) { Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f }); }
		if (input->PushKey(DIK_D)) { Object3d::CameraMoveVector({ +1.0f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_A)) { Object3d::CameraMoveVector({ -1.0f,0.0f,0.0f }); }
	}

	// 球の移動
	{
		// 現在の座標を取得
		XMFLOAT3 position = objSphere->GetPosition();
		moveTimer++;

		XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
		if (input->PushKey(DIK_UP)) { sphere.center += moveY; position.y += 1.0f;}
		else if (input->PushKey(DIK_DOWN)) { sphere.center -= moveY;position.y -= 1.0f; }

		XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (input->PushKey(DIK_RIGHT)) { sphere.center += moveX;position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { sphere.center -= moveX; position.x -= 1.0f;}

		// 移動後の座標を計算
		if (isMove == false) { sphere.center += moveY; position.y += 1.0f;
			if (position.y >= 40) { isMove = true; }
		}else{ sphere.center -= moveY; position.y -= 1.0f;
			if (position.y <= -20) { isMove = false; }
		} 

		// 座標の変更を反映
		objSphere->SetPosition(position);
	}
	// stringstreamで変数の値を埋め込んで整形する
	std::ostringstream spherestr;
	spherestr << "Sphere:("
		<< std::fixed << std::setprecision(2) // 小数点以下2桁まで
		<< sphere.center.m128_f32[0] << ","   // x
		<< sphere.center.m128_f32[1] << ","   // y
		<< sphere.center.m128_f32[2] << ")";   // z

	debugText.Print(spherestr.str(), 50, 180, 1.0f);

	// 球と平面の当たり判定
	bool hit = Collision::CheckSphere2Plane(sphere, plane);
	if (hit) {
		isHitColor = true;
		debugText.Print("HIT", 50, 200, 1.0f);
		// stringstreamをリセットし、交点座標を埋め込む
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2) // 小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","   // x
			<< sphere.center.m128_f32[1] << ","   // y
			<< sphere.center.m128_f32[2] << ")";   // z

		debugText.Print(spherestr.str(), 50, 220, 1.0f);
	}
	
	if (hit != true) { isHitColor = false; }
	if (isHitColor == true) { modelSphere->SetColor(XMFLOAT3(1, 0, 0)); }
	else { modelSphere->SetColor(XMFLOAT3(1, 1, 1)); }

	// 球と三角形の当たり判定
	XMVECTOR inter;
	bool hit = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
	if (hit) {
		debugText.Print("HIT", 50, 200, 1.0f);
		// stringstreamをリセットし、交点座標を埋め込む
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		debugText.Print(spherestr.str(), 50, 220, 1.0f);
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	objSphere->Draw();
	//objFighter2->Draw();
	objGround->Draw();
	objSkydome->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
