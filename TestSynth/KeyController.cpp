// --------------------------------------------------------------------------------
// KeyController.cpp
//
// 2016 / 04 / 07
// Created by:	Hayato Iwata
// Thanks to:	http://www39.atwiki.jp/vst_prog/
//				http://www.geocities.jp/daichi1969/
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// インクルードファイル
// --------------------------------------------------------------------------------
#include <stdlib.h>
#include "KeyController.h"


// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------
KeyController::KeyController(int NoteNumber) :

	// パラメータの初期化
	m_NoteOn(false),
	m_OscNumber(-1),
	m_Velocity(0),
	m_Attack(0),
	m_Decay(0),
	m_Release(0)
{
	// 初期化の処理をここに書く

	// ノートナンバーの設定
	m_NoteNumber = NoteNumber;
}


// --------------------------------------------------------------------------------
// デストラクタ
// --------------------------------------------------------------------------------
KeyController::~KeyController()
{
	// メモリの開放等の処理が必要な場合はここに書く

}


// --------------------------------------------------------------------------------
// ノートナンバーを返す関数
// --------------------------------------------------------------------------------
int KeyController::note()
{
	return m_NoteNumber;
}


// --------------------------------------------------------------------------------
// 使用中のオシレーターを返す関数
// --------------------------------------------------------------------------------
int KeyController::osc()
{
	return m_OscNumber;
}

// --------------------------------------------------------------------------------
// 発音しているかどうかを返す関数
// --------------------------------------------------------------------------------
bool KeyController::voice()
{
	// エンベロープジェネレーター未実装のため、鍵盤のの状態をそのまま返す
	return m_NoteOn;
}


// --------------------------------------------------------------------------------
// ベロシティーの値を返す関数
// --------------------------------------------------------------------------------
int KeyController::vel()
{
	return m_Velocity;
}


// --------------------------------------------------------------------------------
// MIDIノートオン信号を受け付けるための関数
// --------------------------------------------------------------------------------
void KeyController::noteOn(int OscNumber, int Velocity)
{
	// 鍵盤の状態とオシレータ・ベロシティーを更新
	m_NoteOn = true;
	m_OscNumber = OscNumber;
	m_Velocity = Velocity;
}


// --------------------------------------------------------------------------------
// MIDIノートオフ信号を受け付けるための関数
// --------------------------------------------------------------------------------
void KeyController::noteOff()
{
	// 鍵盤の状態とオシレーター・ベロシティーを更新
	m_NoteOn = false;
	m_Velocity = 0;

}


// --------------------------------------------------------------------------------
// エンベロープの状態を更新するための関数
// --------------------------------------------------------------------------------
void KeyController::processEnv()
{
	// エンベロープジェネレーター未実装

}