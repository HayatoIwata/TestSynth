// --------------------------------------------------------------------------------
// KeyController.h
//
// 2016 / 04 / 07
// Created by:	Hayato Iwata
// Thanks to:	http://www39.atwiki.jp/vst_prog/
//				http://www.geocities.jp/daichi1969/
// --------------------------------------------------------------------------------

#pragma once

// --------------------------------------------------------------------------------
// インクルードファイル
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// KeyControllerクラスの定義
// --------------------------------------------------------------------------------
class KeyController
{
public:

	// コンストラクタ & デストラクタ
	KeyController(int NoteNumber);
	~KeyController();

	// ノートナンバーを返す関数
	int note();

	// 使用中のオシレータを返す関数
	int osc();

	// 発音中かどうかを返す関数
	bool voice();

	// ベロシティーを返す関数
	int vel();
	
	// 鍵盤の状態を変更する関数
	void noteOn(int OscNumber, int Velocity);
	void noteOff();

	// エンベロープの更新するための関数
	void processEnv();

private:

	// 音階を記録するメンバ変数
	int m_NoteNumber;

	// 使用中のオシレータを示すポインタ
	int m_OscNumber;

	// 鍵盤の状態を保持するメンバ変数
	bool m_NoteOn;				// ノートON
	int m_Velocity;				// ベロシティー

	// エンベロープの状態を保持するメンバ関数
	int m_Attack;				// アタック
	int m_Decay;				// ディケイ
	int m_Release;				// リリース

};