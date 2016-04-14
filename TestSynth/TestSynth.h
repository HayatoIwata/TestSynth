// --------------------------------------------------------------------------------
// TestSynth.h
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
#include "audioeffectx.h"
#include "KeyController.h"
#include "Oscillater.h"

// --------------------------------------------------------------------------------
// VSTiの仕様情報
// --------------------------------------------------------------------------------
const int PRESET_NUM = 1;				// プリセットプログラム数（0にしないこと！）
const int PARAM_NUM = 1;				// パラメーター数
const int OUTPUT_CH = 2;				// 出力チャンネル数
const int HI_KEY = 96;					// 最高ノートナンバー
const int LW_KEY = 48;					// 最低ノートナンバー
const int N_KEY = HI_KEY - LW_KEY + 1;	// 鍵盤数
const int N_POLY = 10;					// 最大同時発音数

const double NORM = 0.2;				// 音量正規化係数

// プラグインの固有ID
// ※作成したVSTプラグインを一般公開する際は、下記サイトでユニークIDを取得する
//   http://ygrabit.steinberg.de/~ygrabit/public_html/index.html
const int UNIQ_ID = 'SMPL';


// --------------------------------------------------------------------------------
// MIDIメッセージ構造体の定義
// --------------------------------------------------------------------------------
struct MidiMsg
{
	VstInt32		deltaFrames;
	unsigned char	message;			// MIDIメッセージ番号
	unsigned char	channel;			// チャンネル
	unsigned char	data1;				// MIDIデータ2
	unsigned char	data2;				// MIDIデータ1
};


// --------------------------------------------------------------------------------
// VSTの基本クラスの定義
// AudioEffectXクラスを継承
// --------------------------------------------------------------------------------
class TestSynth : public AudioEffectX
{
public:

	// コンストラクタ & デストラクタ
	TestSynth(audioMasterCallback audioMaster);
	~TestSynth();

	// MIDIイベントを処理するためのメンバ関数
	virtual VstInt32 processEvents(VstEvents *events);

	// 音声処理をするためのメンバ関数
	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);

	// プラグイン情報に関するメンバ関数
	bool getEffectName(char* name);
	bool getVendorString(char* vendor);
	VstInt32 getVendorVersion();

	// サンプリングレートの設定
	virtual void setSampleRate(float sampleRate);

	// パラメータに関するメンバ関数
	virtual void  setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void  getParameterName(VstInt32 index, char* text);
	virtual void  getParameterLabel(VstInt32 index, char* label);
	virtual void  getParameterDisplay(VstInt32 index, char* text);


private:

	// サンプリングレート
	float m_Fs;

	// MIDIイベントバッファ用のメンバ変数
	MidiMsg *MidiBuffer;				// MIDIイベントバッファ用のポインタ
	int m_MidiBufSize;					// MIDIイベントバッファのサイズ
	int m_MidiBufNum;					// バッファに格納されているMIDIイベントの数

	// KeyController用のポインタ
	KeyController* Key[N_KEY];

	// Oscillater用のポインタ
	Oscillater* Osc[N_POLY];

	// 鍵盤状態を更新するためのメンバ関数
	void KeyUpdate(MidiMsg* Event);

	// ホストから渡されるパラメータ（すべて0.0～1.0）
	float m_Volume;						// index: 0  ボリューム

	// パラメータを表示値に変換するメンバ関数
	double dVolume(float value);		// ボリューム（-64.0[dB]～0.0[dB]）

};