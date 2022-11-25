#pragma once

#include <DirectXMath.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <windows.h>

class Model
{
private:
	// Microsoft::WRLを省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr <T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

	// マテリアル
	struct Material
	{
		std::string name; // マテリアル名
		XMFLOAT3 ambient; // アンビエント影響度
		XMFLOAT3 diffuse; // ディフェーズ影響度
		XMFLOAT3 specular;// スペキュラー影響度
		float alpha;      // アルファ
		std::string textureFilename; // テクスチャファイル名
		// コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// パイプラインステイト
	ComPtr<ID3D12PipelineState> pipelinestate;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点データ配列
	//static VertexPosNormalUv vertices[vertexCount];
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	//static unsigned short indices[planeCount * 3];
	std::vector<unsigned short> indices;
	// マテリアル
	Material material;

private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void InitializeDescriptorHeap();

	/// <summary>
	/// カメラ初期化
	/// </summary>
	/// <param name="window_width">画面横幅</param>
	/// <param name="window_height">画面縦幅</param>
	static void InitializeCamera(int window_width, int window_height);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	static void LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// モデル作成
	/// </summary>
	static void CreateModel();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	/// <returns>成否</returns>
	static void LoadMaterial(const std::string& directoryPath, const std::string& filename);
};