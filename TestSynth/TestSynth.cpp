// --------------------------------------------------------------------------------
// TestSynth.cpp
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

#define _USE_MATH_DEFINES
#include <math.h>

#include "TestSynth.h"


// --------------------------------------------------------------------------------
// VSTiのインスタンスを生成するための関数
// --------------------------------------------------------------------------------
AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new TestSynth(audioMaster);
}


// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------
TestSynth::TestSynth(audioMasterCallback audioMaster) :
	AudioEffectX(audioMaster, PRESET_NUM, PARAM_NUM),

	m_MidiBufSize(0),			// MIDIイベントバッファのサイズ
	m_MidiBufNum(0),			// バッファに格納されているMIDIイベントの数

	// ここにパラメータの初期値を書く
	m_Volume(1.0)
{
	// 初期設定
	setNumInputs(0);			// 入力チャンネル数の設定(シンセなので入力は無し)
	setNumOutputs(OUTPUT_CH);	// 出力チャンネル数の設定
	setUniqueID(UNIQ_ID);		// ユニークIDの設定
	isSynth(true);				// ture: 音源  false: エフェクター
	canProcessReplacing();		// 音声処理の可否

	// KeyControllerのインスタンスを生成
	for (int i = 0; i < N_KEY; i++)
	{
		Key[i] = new KeyController(i + LW_KEY);
	}

	// Oscillater用のポインタを初期化
	for (int i = 0; i < N_POLY; i++)
	{
		Osc[i] = NULL;
	}

	// MIDIイベントバッファを初期化
	MidiBuffer = NULL;
}


// --------------------------------------------------------------------------------
// デストラクタ
// --------------------------------------------------------------------------------
TestSynth::~TestSynth()
{
	// メモリの開放等の処理が必要な場合はここに書く

	for (int i = 0; i < N_KEY; i++)
	{
		delete Key[i];
	}

	for (int i = 0; i < N_POLY; i++)
	{
		delete Osc[i];
	}

}


// --------------------------------------------------------------------------------
// MIDIイベントの処理を行う関数
// ホストから送られてきたMIDIイベントをバッファに一時保存する
// --------------------------------------------------------------------------------
VstInt32 TestSynth::processEvents(VstEvents *ev)
{
	// MIDIバッファのパラメータをリセット
	m_MidiBufNum = 0;

	if (ev->numEvents > 0)
	{
		// バッファサイズが小さい場合バッファを拡張
		if (ev->numEvents > m_MidiBufSize)
		{
			if (MidiBuffer) delete[] MidiBuffer;
			m_MidiBufSize = ev->numEvents;
			MidiBuffer = new MidiMsg[m_MidiBufSize];
		}

		m_MidiBufNum = ev->numEvents;

		// MIDIシステムメッセージ以外のMIDIイベントならバッファへコピー
		for (int i = 0; i < m_MidiBufNum; i++)
		{
			VstMidiEvent* pE = (VstMidiEvent*)ev->events[i];

			if ((pE->type == kVstMidiType) && ((pE->midiData[0] & 0xf0) != 0xf0))
			{
				// バッファへ追加
				MidiBuffer[i].deltaFrames = pE->deltaFrames;		// deltaFrames
				MidiBuffer[i].message = pE->midiData[0] & 0xf0;		// MIDIメッセージ
				MidiBuffer[i].channel = pE->midiData[0] & 0x0f;		// MIDIチャンネル
				MidiBuffer[i].data1 = pE->midiData[1];				// MIDIデータ1
				MidiBuffer[i].data2 = pE->midiData[2];				// MIDIデータ2
			}
		}

	}

	return 1;
}


// --------------------------------------------------------------------------------
// 音声処理を行う関数
// 鍵盤状態をもとに音の発音を行う
// --------------------------------------------------------------------------------
void TestSynth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* output_L = outputs[0];	// 出力 - Lチャンネル
	float* output_R = outputs[1];	// 出力 - Rチャンネル

	double vol = pow(10, dVolume(m_Volume) / 10);

	for (int sample = 0; sample < sampleFrames; sample++)
	{
		double value = 0;

		// MIDIイベントの処理
		for (int k = 0; k < m_MidiBufNum; k++)
		{
			if (sample == MidiBuffer[k].deltaFrames)
			{
				KeyUpdate(&(MidiBuffer[k]));
			}
		}

		// エンベロープの処理
		for (int i = 0; i < N_KEY; i++)
		{
			Key[i]->processEnv();
		}

		// 波形の生成
		for (int i = 0; i < N_POLY; i++)
		{
			if (Osc[i] != NULL)
			{
				value += Osc[i]->waveGenerate();
			}
		}

		output_L[sample] = (float)(NORM * vol * value);
		output_R[sample] = (float)(NORM * vol * value);
	}

}


// --------------------------------------------------------------------------------
// 鍵盤の状態を更新するためのメンバ関数
// 引数として送られたMIDIイベントをもとに鍵盤の状態を更新する
// --------------------------------------------------------------------------------
void TestSynth::KeyUpdate(MidiMsg* Event)
{
	// 鍵盤の有効範囲を限定する
	if (((int)Event->data1 >= LW_KEY) && ((int)Event->data1 <= HI_KEY))
	{
		// 送られてきた信号がMIDIノートオンのとき
		if (Event->message == 0x90)
		{
			// MIDIデータの読み込み
			int NoteNumber = (int)Event->data1;
			int Velocity = (int)Event->data2;

			// オシレータに空きがあるか探す
			int k = 0;
			while (Osc[k] != NULL)
			{
				k++;
				if (k >= N_POLY)
				{
					k = -1;
					break;
				}
			}

			// オシレータに空きがある場合、インスタンスを生成
			if (k >= 0)
			{
				Osc[k] = new Oscillater(NoteNumber, m_Fs);
			}

			// 鍵盤の状態を更新
			Key[NoteNumber - LW_KEY]->noteOn(k, Velocity);
		}

		// 送られてきた信号がMIDIノートオフのとき、
		if (Event->message == 0x80)
		{
			// MIDIデータの読み込み
			int NoteNumber = (int)Event->data1;

			// オシレーターのインスタンスを消去
			if (Key[NoteNumber - LW_KEY]->osc() >= 0)
			{
				delete Osc[Key[NoteNumber - LW_KEY]->osc()];
				Osc[Key[NoteNumber - LW_KEY]->osc()] = NULL;
			}

			// 鍵盤の状態を更新
			Key[NoteNumber - LW_KEY]->noteOff();
		}
	}

}


// --------------------------------------------------------------------------------
// プラグインの名前を設定するメンバ関数
// --------------------------------------------------------------------------------
bool TestSynth::getEffectName(char* name)
{
	vst_strncpy(name, "Test Synth", kVstMaxEffectNameLen);
	return true;
}


// --------------------------------------------------------------------------------
// プラグインのベンダ名を設定するメンバ関数
// --------------------------------------------------------------------------------
bool TestSynth::getVendorString(char* vendor)
{
	vst_strncpy(vendor, "", kVstMaxVendorStrLen);
	return true;
}


// --------------------------------------------------------------------------------
// プラグインのバージョンを設定するすメンバ関数
// --------------------------------------------------------------------------------
VstInt32 TestSynth::getVendorVersion()
{
	return 1000;
}


// --------------------------------------------------------------------------------
// サンプリングレートを設定するすメンバ関数
// --------------------------------------------------------------------------------
void TestSynth::setSampleRate(float sampleRate)
{
	m_Fs = sampleRate;
}


// --------------------------------------------------------------------------------
// パラメータの設定を行うメンバ関数
// プラグインのGUIが操作された場合に呼び出され、パラメータのindexとvalueが渡される
// --------------------------------------------------------------------------------
void TestSynth::setParameter(VstInt32 index, float value)
{
	// indexで指定されたパラメータの値を設定する
	switch (index)
	{
	case 0:
		m_Volume = value;
		break;

	default:
		break;
	}

}


// --------------------------------------------------------------------------------
// パラメータの生の値をホストアプリケーションに返すメンバ関数
// GUIの更新時などに必要な、indexで指定されたパラメータの値を戻り値として返す
// --------------------------------------------------------------------------------
float TestSynth::getParameter(VstInt32 index)
{
	float value = 0.;

	// indexで指定されたパラメータの値をvalueに代入する
	switch (index)
	{
	case 0:
		value = m_Volume;
		break;

	default:
		break;
	}

	return value;
}


// --------------------------------------------------------------------------------
// パラメータの名前をホストアプリケーションに返すメンバ関数
// GUIの更新時などに必要な、indexで指定されたパラメータの名前をポインタで返す
// --------------------------------------------------------------------------------
void TestSynth::getParameterName(VstInt32 index, char* text)
{
	// indexで指定されたパラメータの名前をtextに格納する
	// vst_strncpy(text, "名前", kVstMaxParamStrLen)
	// 　…いい感じに"名前"の文字数を調整してtextに格納してくれるやつ
	switch (index)
	{
	case 0:
		vst_strncpy(text, "Volume", kVstMaxParamStrLen);
		break;
		
	default:
		break;
	}

}


// --------------------------------------------------------------------------------
// パラメータの単位をホストアプリケーションに返すメンバ関数
// GUIの更新時などに必要な、indexで指定されたパラメータの単位をポインタで返す
// --------------------------------------------------------------------------------
void TestSynth::getParameterLabel(VstInt32 index, char* label)
{
	// indexで指定されたパラメータの単位をtextに格納する
	// vst_strncpy(label, "単位", kVstMaxParamStrLen)
	// 　…いい感じに"単位"の文字数を調整してlabelに格納してくれるやつ
	switch (index)
	{
	case 0:
		vst_strncpy(label, "dB", kVstMaxParamStrLen);
		break;

	default:
		break;
	}

}

// --------------------------------------------------------------------------------
// パラメータの表示値をホストアプリケーションに返すメンバ関数
// 各パラメータのvalueは0.0～1.0で返ってくるが、表示値は任意のものにすることができる
// GUIの更新時などに必要な、indexで指定されたパラメータの表示値をポインタで返す
// --------------------------------------------------------------------------------
void TestSynth::getParameterDisplay(VstInt32 index, char* text)
{
	// indexで指定されたパラメータの表示値を文字列に変換した上でtextに格納する
	// float2string(値, text, kVstMaxParamStrLen)
	// 　…値を文字列に変換した上で文字数を調整してtextに格納してくれるやつ
	switch (index)
	{
	case 0:
		float2string((float)dVolume(m_Volume), text, kVstMaxParamStrLen);
		break;

	default:
		break;
	}

}


// --------------------------------------------------------------------------------
// パラメータ: Volumeを表示値に変換
// value（0.0～1.0）を表示値（-64.0～0.0）に変換
// --------------------------------------------------------------------------------
double TestSynth::dVolume(float value)
{
	return 64. * value - 64.;
}