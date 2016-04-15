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
// �C���N���[�h�t�@�C��
// --------------------------------------------------------------------------------
#include "audioeffectx.h"
#include "KeyController.h"
#include "Oscillater.h"

// --------------------------------------------------------------------------------
// VSTi�̎d�l���
// --------------------------------------------------------------------------------
const int PRESET_NUM = 1;				// �v���Z�b�g�v���O�������i0�ɂ��Ȃ����ƁI�j
const int PARAM_NUM = 1;				// �p�����[�^�[��
const int OUTPUT_CH = 2;				// �o�̓`�����l����
const int HI_KEY = 96;					// �ō��m�[�g�i���o�[
const int LW_KEY = 36;					// �Œ�m�[�g�i���o�[
const int N_KEY = HI_KEY - LW_KEY + 1;	// ���Ր�
const int N_POLY = 10;					// �ő哯��������

const double NORM = 0.2;				// ���ʐ��K���W��

// �v���O�C���̌ŗLID
// ���쐬����VST�v���O�C������ʌ��J����ۂ́A���L�T�C�g�Ń��j�[�NID���擾����
//   http://ygrabit.steinberg.de/~ygrabit/public_html/index.html
const int UNIQ_ID = 'SMPL';


// --------------------------------------------------------------------------------
// MIDI���b�Z�[�W�\���̂̒�`
// --------------------------------------------------------------------------------
struct MidiMsg
{
	VstInt32		deltaFrames;
	unsigned char	message;			// MIDI���b�Z�[�W�ԍ�
	unsigned char	channel;			// �`�����l��
	unsigned char	data1;				// MIDI�f�[�^2
	unsigned char	data2;				// MIDI�f�[�^1
};


// --------------------------------------------------------------------------------
// VST�̊�{�N���X�̒�`
// AudioEffectX�N���X���p��
// --------------------------------------------------------------------------------
class TestSynth : public AudioEffectX
{
public:

	// �R���X�g���N�^ & �f�X�g���N�^
	TestSynth(audioMasterCallback audioMaster);
	~TestSynth();

	// MIDI�C�x���g���������邽�߂̃����o�֐�
	virtual VstInt32 processEvents(VstEvents *events);

	// �������������邽�߂̃����o�֐�
	virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);

	// �v���O�C�����Ɋւ��郁���o�֐�
	bool getEffectName(char* name);
	bool getVendorString(char* vendor);
	VstInt32 getVendorVersion();

	// �T���v�����O���[�g�̐ݒ�
	virtual void setSampleRate(float sampleRate);

	// �p�����[�^�Ɋւ��郁���o�֐�
	virtual void  setParameter(VstInt32 index, float value);
	virtual float getParameter(VstInt32 index);
	virtual void  getParameterName(VstInt32 index, char* text);
	virtual void  getParameterLabel(VstInt32 index, char* label);
	virtual void  getParameterDisplay(VstInt32 index, char* text);


private:

	// �T���v�����O���[�g
	float m_Fs;

	// MIDI�C�x���g�o�b�t�@�p�̃����o�ϐ�
	MidiMsg *MidiBuffer;				// MIDI�C�x���g�o�b�t�@�p�̃|�C���^
	int m_MidiBufSize;					// MIDI�C�x���g�o�b�t�@�̃T�C�Y
	int m_MidiBufNum;					// �o�b�t�@�Ɋi�[����Ă���MIDI�C�x���g�̐�

	// KeyController�p�̃|�C���^
	KeyController* Key[N_KEY];

	// Oscillater�p�̃|�C���^
	Oscillater* Osc[N_POLY];

	// ���Տ�Ԃ��X�V���邽�߂̃����o�֐�
	void KeyUpdate(MidiMsg* Event);

	// �z�X�g����n�����p�����[�^�i���ׂ�0.0�`1.0�j
	float m_Volume;						// index: 0  �{�����[��

	// �p�����[�^��\���l�ɕϊ����郁���o�֐�
	double dVolume(float value);		// �{�����[���i-64.0[dB]�`0.0[dB]�j

};