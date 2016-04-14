// --------------------------------------------------------------------------------
// TestSynth.cpp
//
// 2016 / 04 / 07
// Created by:	Hayato Iwata
// Thanks to:	http://www39.atwiki.jp/vst_prog/
//				http://www.geocities.jp/daichi1969/
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
// --------------------------------------------------------------------------------
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "TestSynth.h"


// --------------------------------------------------------------------------------
// VSTi�̃C���X�^���X�𐶐����邽�߂̊֐�
// --------------------------------------------------------------------------------
AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new TestSynth(audioMaster);
}


// --------------------------------------------------------------------------------
// �R���X�g���N�^
// --------------------------------------------------------------------------------
TestSynth::TestSynth(audioMasterCallback audioMaster) :
	AudioEffectX(audioMaster, PRESET_NUM, PARAM_NUM),

	m_MidiBufSize(0),			// MIDI�C�x���g�o�b�t�@�̃T�C�Y
	m_MidiBufNum(0),			// �o�b�t�@�Ɋi�[����Ă���MIDI�C�x���g�̐�

	// �����Ƀp�����[�^�̏����l������
	m_Volume(1.0)
{
	// �����ݒ�
	setNumInputs(0);			// ���̓`�����l�����̐ݒ�(�V���Z�Ȃ̂œ��͖͂���)
	setNumOutputs(OUTPUT_CH);	// �o�̓`�����l�����̐ݒ�
	setUniqueID(UNIQ_ID);		// ���j�[�NID�̐ݒ�
	isSynth(true);				// ture: ����  false: �G�t�F�N�^�[
	canProcessReplacing();		// ���������̉�

	// KeyController�̃C���X�^���X�𐶐�
	for (int i = 0; i < N_KEY; i++)
	{
		Key[i] = new KeyController(i + LW_KEY);
	}

	// Oscillater�p�̃|�C���^��������
	for (int i = 0; i < N_POLY; i++)
	{
		Osc[i] = NULL;
	}

	// MIDI�C�x���g�o�b�t�@��������
	MidiBuffer = NULL;
}


// --------------------------------------------------------------------------------
// �f�X�g���N�^
// --------------------------------------------------------------------------------
TestSynth::~TestSynth()
{
	// �������̊J�����̏������K�v�ȏꍇ�͂����ɏ���

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
// MIDI�C�x���g�̏������s���֐�
// �z�X�g���瑗���Ă���MIDI�C�x���g���o�b�t�@�Ɉꎞ�ۑ�����
// --------------------------------------------------------------------------------
VstInt32 TestSynth::processEvents(VstEvents *ev)
{
	// MIDI�o�b�t�@�̃p�����[�^�����Z�b�g
	m_MidiBufNum = 0;

	if (ev->numEvents > 0)
	{
		// �o�b�t�@�T�C�Y���������ꍇ�o�b�t�@���g��
		if (ev->numEvents > m_MidiBufSize)
		{
			if (MidiBuffer) delete[] MidiBuffer;
			m_MidiBufSize = ev->numEvents;
			MidiBuffer = new MidiMsg[m_MidiBufSize];
		}

		m_MidiBufNum = ev->numEvents;

		// MIDI�V�X�e�����b�Z�[�W�ȊO��MIDI�C�x���g�Ȃ�o�b�t�@�փR�s�[
		for (int i = 0; i < m_MidiBufNum; i++)
		{
			VstMidiEvent* pE = (VstMidiEvent*)ev->events[i];

			if ((pE->type == kVstMidiType) && ((pE->midiData[0] & 0xf0) != 0xf0))
			{
				// �o�b�t�@�֒ǉ�
				MidiBuffer[i].deltaFrames = pE->deltaFrames;		// deltaFrames
				MidiBuffer[i].message = pE->midiData[0] & 0xf0;		// MIDI���b�Z�[�W
				MidiBuffer[i].channel = pE->midiData[0] & 0x0f;		// MIDI�`�����l��
				MidiBuffer[i].data1 = pE->midiData[1];				// MIDI�f�[�^1
				MidiBuffer[i].data2 = pE->midiData[2];				// MIDI�f�[�^2
			}
		}

	}

	return 1;
}


// --------------------------------------------------------------------------------
// �����������s���֐�
// ���Տ�Ԃ����Ƃɉ��̔������s��
// --------------------------------------------------------------------------------
void TestSynth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* output_L = outputs[0];	// �o�� - L�`�����l��
	float* output_R = outputs[1];	// �o�� - R�`�����l��

	double vol = pow(10, dVolume(m_Volume) / 10);

	for (int sample = 0; sample < sampleFrames; sample++)
	{
		double value = 0;

		// MIDI�C�x���g�̏���
		for (int k = 0; k < m_MidiBufNum; k++)
		{
			if (sample == MidiBuffer[k].deltaFrames)
			{
				KeyUpdate(&(MidiBuffer[k]));
			}
		}

		// �G���x���[�v�̏���
		for (int i = 0; i < N_KEY; i++)
		{
			Key[i]->processEnv();
		}

		// �g�`�̐���
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
// ���Ղ̏�Ԃ��X�V���邽�߂̃����o�֐�
// �����Ƃ��đ���ꂽMIDI�C�x���g�����ƂɌ��Ղ̏�Ԃ��X�V����
// --------------------------------------------------------------------------------
void TestSynth::KeyUpdate(MidiMsg* Event)
{
	// ���Ղ̗L���͈͂����肷��
	if (((int)Event->data1 >= LW_KEY) && ((int)Event->data1 <= HI_KEY))
	{
		// �����Ă����M����MIDI�m�[�g�I���̂Ƃ�
		if (Event->message == 0x90)
		{
			// MIDI�f�[�^�̓ǂݍ���
			int NoteNumber = (int)Event->data1;
			int Velocity = (int)Event->data2;

			// �I�V���[�^�ɋ󂫂����邩�T��
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

			// �I�V���[�^�ɋ󂫂�����ꍇ�A�C���X�^���X�𐶐�
			if (k >= 0)
			{
				Osc[k] = new Oscillater(NoteNumber, m_Fs);
			}

			// ���Ղ̏�Ԃ��X�V
			Key[NoteNumber - LW_KEY]->noteOn(k, Velocity);
		}

		// �����Ă����M����MIDI�m�[�g�I�t�̂Ƃ��A
		if (Event->message == 0x80)
		{
			// MIDI�f�[�^�̓ǂݍ���
			int NoteNumber = (int)Event->data1;

			// �I�V���[�^�[�̃C���X�^���X������
			if (Key[NoteNumber - LW_KEY]->osc() >= 0)
			{
				delete Osc[Key[NoteNumber - LW_KEY]->osc()];
				Osc[Key[NoteNumber - LW_KEY]->osc()] = NULL;
			}

			// ���Ղ̏�Ԃ��X�V
			Key[NoteNumber - LW_KEY]->noteOff();
		}
	}

}


// --------------------------------------------------------------------------------
// �v���O�C���̖��O��ݒ肷�郁���o�֐�
// --------------------------------------------------------------------------------
bool TestSynth::getEffectName(char* name)
{
	vst_strncpy(name, "Test Synth", kVstMaxEffectNameLen);
	return true;
}


// --------------------------------------------------------------------------------
// �v���O�C���̃x���_����ݒ肷�郁���o�֐�
// --------------------------------------------------------------------------------
bool TestSynth::getVendorString(char* vendor)
{
	vst_strncpy(vendor, "", kVstMaxVendorStrLen);
	return true;
}


// --------------------------------------------------------------------------------
// �v���O�C���̃o�[�W������ݒ肷�邷�����o�֐�
// --------------------------------------------------------------------------------
VstInt32 TestSynth::getVendorVersion()
{
	return 1000;
}


// --------------------------------------------------------------------------------
// �T���v�����O���[�g��ݒ肷�邷�����o�֐�
// --------------------------------------------------------------------------------
void TestSynth::setSampleRate(float sampleRate)
{
	m_Fs = sampleRate;
}


// --------------------------------------------------------------------------------
// �p�����[�^�̐ݒ���s�������o�֐�
// �v���O�C����GUI�����삳�ꂽ�ꍇ�ɌĂяo����A�p�����[�^��index��value���n�����
// --------------------------------------------------------------------------------
void TestSynth::setParameter(VstInt32 index, float value)
{
	// index�Ŏw�肳�ꂽ�p�����[�^�̒l��ݒ肷��
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
// �p�����[�^�̐��̒l���z�X�g�A�v���P�[�V�����ɕԂ������o�֐�
// GUI�̍X�V���ȂǂɕK�v�ȁAindex�Ŏw�肳�ꂽ�p�����[�^�̒l��߂�l�Ƃ��ĕԂ�
// --------------------------------------------------------------------------------
float TestSynth::getParameter(VstInt32 index)
{
	float value = 0.;

	// index�Ŏw�肳�ꂽ�p�����[�^�̒l��value�ɑ������
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
// �p�����[�^�̖��O���z�X�g�A�v���P�[�V�����ɕԂ������o�֐�
// GUI�̍X�V���ȂǂɕK�v�ȁAindex�Ŏw�肳�ꂽ�p�����[�^�̖��O���|�C���^�ŕԂ�
// --------------------------------------------------------------------------------
void TestSynth::getParameterName(VstInt32 index, char* text)
{
	// index�Ŏw�肳�ꂽ�p�����[�^�̖��O��text�Ɋi�[����
	// vst_strncpy(text, "���O", kVstMaxParamStrLen)
	// �@�c����������"���O"�̕������𒲐�����text�Ɋi�[���Ă������
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
// �p�����[�^�̒P�ʂ��z�X�g�A�v���P�[�V�����ɕԂ������o�֐�
// GUI�̍X�V���ȂǂɕK�v�ȁAindex�Ŏw�肳�ꂽ�p�����[�^�̒P�ʂ��|�C���^�ŕԂ�
// --------------------------------------------------------------------------------
void TestSynth::getParameterLabel(VstInt32 index, char* label)
{
	// index�Ŏw�肳�ꂽ�p�����[�^�̒P�ʂ�text�Ɋi�[����
	// vst_strncpy(label, "�P��", kVstMaxParamStrLen)
	// �@�c����������"�P��"�̕������𒲐�����label�Ɋi�[���Ă������
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
// �p�����[�^�̕\���l���z�X�g�A�v���P�[�V�����ɕԂ������o�֐�
// �e�p�����[�^��value��0.0�`1.0�ŕԂ��Ă��邪�A�\���l�͔C�ӂ̂��̂ɂ��邱�Ƃ��ł���
// GUI�̍X�V���ȂǂɕK�v�ȁAindex�Ŏw�肳�ꂽ�p�����[�^�̕\���l���|�C���^�ŕԂ�
// --------------------------------------------------------------------------------
void TestSynth::getParameterDisplay(VstInt32 index, char* text)
{
	// index�Ŏw�肳�ꂽ�p�����[�^�̕\���l�𕶎���ɕϊ��������text�Ɋi�[����
	// float2string(�l, text, kVstMaxParamStrLen)
	// �@�c�l�𕶎���ɕϊ�������ŕ������𒲐�����text�Ɋi�[���Ă������
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
// �p�����[�^: Volume��\���l�ɕϊ�
// value�i0.0�`1.0�j��\���l�i-64.0�`0.0�j�ɕϊ�
// --------------------------------------------------------------------------------
double TestSynth::dVolume(float value)
{
	return 64. * value - 64.;
}