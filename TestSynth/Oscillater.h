// --------------------------------------------------------------------------------
// Oscillater.h
//
// 2016 / 04 / 07
// Created by:	Hayato Iwata
// --------------------------------------------------------------------------------

#pragma once

// --------------------------------------------------------------------------------
// �N���X�̒�`
// --------------------------------------------------------------------------------
class Oscillater
{
public:

	// �R���X�g���N�^ & �f�X�g���N�^
	Oscillater(int noteNumber, double sampleRate);
	~Oscillater();

	// �l��Ԃ������o�֐�
	double waveGenerate();


private:

	int m_NoteNumber;			// �m�[�g�i���o�[
	double m_SampleRate;		// �T���v�����O���[�g

	double m_phase;				// ���݂̈ʑ�
	double m_dw;				// 1�T���v��������̈ʑ���(�p���g��)

};
