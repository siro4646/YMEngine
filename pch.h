#pragma once
//===============================================
// プリコンパイル済みヘッダー
//  ここに書いたものは初回のみ解析されるため、コンパイル時間が高速になる。
//  全てのcppからインクルードされる必要がある。
//===============================================
//===============================================
// 基本
//===============================================
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"winmm.lib")

#define NOMINMAX

#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <stdio.h>
#include <winsock2.h>
#include <winsock.h>
#include <conio.h>
#pragma comment (lib, "Ws2_32.lib")
#include <Windows.h>
#include <iostream>
#include <cassert>

#include <wrl/client.h>
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

//===============================================
// STL
//===============================================

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <fileSystem>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
//===============================================
// Direct3D12
//===============================================
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#pragma comment(lib,"d3dcompiler.lib")
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <d3dcompiler.h>
#include "d3dx12.h"
//#include <d3dx12.h>
// DirectX Tool Kit
//#pragma comment(lib, "DirectXTK12.lib")
//#include <SimpleMath.h>

#include <DirectXMath.h>

// DirectX Tex
#include <DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")

// Boost
#include <boost/asio.hpp>

//Assimp
#include <C:/assimp/include/assimp/cimport.h>
#include <C:/assimp/include/assimp/Importer.hpp>
#include <C:/assimp/include/assimp/scene.h>
#include <C:/assimp/include/assimp/postprocess.h>
#include <C:/assimp/include/assimp/matrix4x4.h>
#pragma comment (lib, "assimp-vc143-mt.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "thirdParty/imgui/imgui.h"
#include "thirdParty/imgui/imgui_impl_dx12.h"
#include "thirdParty/imgui/imgui_impl_win32.h"

//nlohmann json
#include "thirdParty/nlohmann/json.hpp"
using json = nlohmann::json;

//Direct2D
#include <d2d1.h>

//using namespace
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;


//自作ヘッダー
//型
#include "define/type.h"
#include "utility/vector/i_vector.h"


//便利系
#include "utility/utility.h"
#include "utility/stringUtility.h"
#include "utility/timer.h"
#include "utility/debug/debugDraw.h"
#include "debug/debug.h"

#include "winAPI/window/DropFileManager.h"




#include "utility/inputSystem/base/inputDevice.h"
#include "utility/inputSystem/keyBoard/keyBoardInput.h"
#include "utility/inputSystem/mouse/mouseInput.h"
#include "utility/inputSystem/gamePad/gamePadInput.h"
#include "utility/inputSystem/inputManger/inputManger.h"


