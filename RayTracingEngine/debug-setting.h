#pragma once


#define OFF 0
#ifdef _DEBUG
#define ON 1
#else
#define ON 0
#endif

//onになっていると、各ピクセルカラーが計算できた時点でそれぞれUDPで別アプリケーションに情報を転送します。
//基本的に視覚的なチェックの為に使うことを想定しています。
//またonになっているとアプリケーション側から初回の接続があるまでプログラムはブロッキングされます。 
#define REALTIME_GRAPHICAL_UDP_DEBUG OFF

//onの場合、マルチスレッドで動作している描画コードをシングルスレッド実行に切り替えます
#define ONLY_SINGLE_THREAD_DEBUG OFF