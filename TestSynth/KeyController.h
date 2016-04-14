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
// �C���N���[�h�t�@�C��
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// KeyController�N���X�̒�`
// --------------------------------------------------------------------------------
class KeyController
{
public:

	// �R���X�g���N�^ & �f�X�g���N�^
	KeyController(int NoteNumber);
	~KeyController();

	// �m�[�g�i���o�[��Ԃ��֐�
	int note();

	// �g�p���̃I�V���[�^��Ԃ��֐�
	int osc();

	// ���������ǂ�����Ԃ��֐�
	bool voice();

	// �x���V�e�B�[��Ԃ��֐�
	int vel();
	
	// ���Ղ̏�Ԃ�ύX����֐�
	void noteOn(int OscNumber, int Velocity);
	void noteOff();

	// �G���x���[�v�̍X�V���邽�߂̊֐�
	void processEnv();

private:

	// ���K���L�^���郁���o�ϐ�
	int m_NoteNumber;

	// �g�p���̃I�V���[�^�������|�C���^
	int m_OscNumber;

	// ���Ղ̏�Ԃ�ێ����郁���o�ϐ�
	bool m_NoteOn;				// �m�[�gON
	int m_Velocity;				// �x���V�e�B�[

	// �G���x���[�v�̏�Ԃ�ێ����郁���o�֐�
	int m_Attack;				// �A�^�b�N
	int m_Decay;				// �f�B�P�C
	int m_Release;				// �����[�X

};