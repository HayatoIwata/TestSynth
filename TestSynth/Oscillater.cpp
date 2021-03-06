// --------------------------------------------------------------------------------
// Oscillater.cpp
//
// 2016 / 04 / 07
// Created by:	Hayato Iwata
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// インクルードファイル
// --------------------------------------------------------------------------------
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Oscillater.h"


// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------
Oscillater::Oscillater(int noteNumber, double sampleRate) :

	// パラメータの初期化
	m_phase(0.0)
{
	// 初期化の処理をここに書く

	// ノートナンバーとサンプリングレートの設定
	m_NoteNumber = noteNumber;
	m_SampleRate = sampleRate;

	// 角周波数の設定
	m_dw = 880. * M_PI * pow(2., (double)(m_NoteNumber - 69) / 12.) / m_SampleRate;

}


// --------------------------------------------------------------------------------
// デストラクタ
// --------------------------------------------------------------------------------
Oscillater::~Oscillater()
{
	// メモリの開放等の処理が必要な場合はここに書く

}


// --------------------------------------------------------------------------------
// 値を返すメンバ関数
// --------------------------------------------------------------------------------
double Oscillater::waveGenerate()
{
	double value;

	// 波形の生成

	// 正弦波
	value = sin(m_phase);

	// 矩形波
	//if (m_phase < M_PI)
	//{
	//	value = 1.0;
	//}
	//else
	//{
	//	value = -1.0;
	//}

	// ノコギリ波
	//value = m_phase / M_PI - 1.0;

	// 三角波
	//if (m_phase < 0.5 * M_PI)
	//{
	//	value = 2 * m_phase / M_PI;
	//}
	//else if (m_phase < 1.5 * M_PI)
	//{
	//	value = 2.0 - 2 * m_phase / M_PI;
	//}
	//else
	//{
	//	value = 2 * m_phase / M_PI - 4.0;
	//}

	// 位相のインクリメント
	m_phase += m_dw;
	if (m_phase >= 2 * M_PI)
	{
		m_phase -= 2 * M_PI;
	}

	return value;
}