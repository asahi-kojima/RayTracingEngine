#pragma once
#include <random>
#include <WinSock2.h>
//#include <winsock.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include "typeinfo.h"

#pragma comment(lib, "ws2_32.lib")

class RandomGenerator
{
public:
	static f32 uniform_real(f32 a = 0.0f, f32 b = 1.0f)
	{
		return a + (b - a) * mUniform0_1Dist(mRandomGenerator);
	}

	static f32 signed_uniform_real(f32 a = -1.0f, f32 b = 1.0f)
	{
		return a + (b - a) * mUniform0_1Dist(mRandomGenerator);
	}

	static f32 normal(f32 mu = 0.0f, f32 sigma = 1.0f)
	{
		return mu + sigma * mNormalDist(mRandomGenerator);
	}

	static s32 uniform_int(s32 a = 0, s32 b = 1)
	{
		f32 rand = uniform_real(static_cast<f32>(a), static_cast<f32>(b));

		return static_cast<s32>(rand);
	}

	static s32 uniform_int_round(s32 center = 0, s32 radius = 1)
	{
		f32 rand = uniform_real(static_cast<f32>(center - radius), static_cast<f32>(center + radius));

		return static_cast<s32>(rand);
	}

	static std::random_device rd;
	static std::mt19937 mRandomGenerator;
	static std::normal_distribution<f32> mNormalDist;
	static std::uniform_real_distribution<f32> mUniform0_1Dist;
};

inline std::random_device RandomGenerator::rd;
inline std::mt19937 RandomGenerator::mRandomGenerator(rd());
inline std::normal_distribution<float> RandomGenerator::mNormalDist(0.0f, 1.0f);
inline std::uniform_real_distribution<f32> RandomGenerator::mUniform0_1Dist(0.0f, 1.0f);






//レイトレーシングの結果を別のアプリケーションにリアルタイム送信するためのクラス
//TCP程の慎重さは必要ないのでUDPで通信することにした
class UDPServer
{
public:

	void boot()
	{
		WORD wVersionRequested{};
		WSADATA wsaData;
		auto err = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (err != 0)
		{
			printf("WSAStartup failed with error: %d\n", err);
			return;
		}


		//ソケットの作成 : 失敗はほぼ起きないと思う。
		if ((sockNo = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			perror("failed");
			std::cout << WSAGetLastError() << std::endl;
			assert(0);
		}

		//IPアドレスとポートを指定してバインドする
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		{
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddress.sin_port = htons(ServerPort);
		}

		if (bind(sockNo, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(sockaddr_in)) < 0)
		{
			assert(0);
		}


		//データの送信先を保存するために、一度向こうからメッセージを送ってもらう。
		constexpr u32 MaxReceiveMessageLength = 256;
		char receiveMessageBuffer[MaxReceiveMessageLength];
		s32 addrDataSize = sizeof(sockaddr_in);
		auto messageSize = recvfrom(sockNo, receiveMessageBuffer, MaxReceiveMessageLength, 0, reinterpret_cast<struct sockaddr*>(&clientAddress), &addrDataSize);
		if (messageSize < 255)
		{
			receiveMessageBuffer[messageSize] = '\0';
		}
		else
		{
			assert(0);
		}
		std::cout << "Message Received : " << std::string(receiveMessageBuffer) << std::endl;
		std::cout << "Cliend Port Information : " << clientAddress.sin_port << std::endl;

		isConnectionSuccessed = true;
	}

	void send(f32 width, f32 height, const vec3& v)
	{
		if (!isConnectionSuccessed)
		{
			assert(0);
		}

		constexpr u32 MaxSendMessageLength = 5 * sizeof(f32);
		char messageBuffer[MaxSendMessageLength];
		{
			//ここでベクトルデータを格納する
			const f32 x = v.getX(); const f32 y = v.getY(); const f32 z = v.getZ();
			
			memcpy(messageBuffer + sizeof(f32) * 0, &width, sizeof(f32));
			memcpy(messageBuffer + sizeof(f32) * 1, &height, sizeof(f32));
			memcpy(messageBuffer + sizeof(f32) * 2, &x, sizeof(f32));
			memcpy(messageBuffer + sizeof(f32) * 3, &y, sizeof(f32));
			memcpy(messageBuffer + sizeof(f32) * 4, &z, sizeof(f32));
		}


		s32 sendMessageLenght = sendto(sockNo, messageBuffer, MaxSendMessageLength, 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(sockaddr_in));
	}


private:
	const static s16 ServerPort = 8100;
	s32 sockNo = -1;
	sockaddr_in serverAddress;
	sockaddr_in clientAddress;
	bool isConnectionSuccessed = false;
};


template <typename T>
void swap(T& a, T& b)
{
	T tmp = b;
	b = a;
	a = tmp;
}

template <typename T>
T clamp(T v, T v_min, T v_max)
{
	return min(max(v, v_min), v_max);
}