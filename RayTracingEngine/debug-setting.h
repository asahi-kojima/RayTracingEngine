#pragma once


#define OFF 0
#ifdef _DEBUG
#define ON 1
#else
#define ON 0
#endif

//on�ɂȂ��Ă���ƁA�e�s�N�Z���J���[���v�Z�ł������_�ł��ꂼ��UDP�ŕʃA�v���P�[�V�����ɏ���]�����܂��B
//��{�I�Ɏ��o�I�ȃ`�F�b�N�ׂ̈Ɏg�����Ƃ�z�肵�Ă��܂��B
//�܂�on�ɂȂ��Ă���ƃA�v���P�[�V���������珉��̐ڑ�������܂Ńv���O�����̓u���b�L���O����܂��B 
#define REALTIME_GRAPHICAL_UDP_DEBUG OFF

//on�̏ꍇ�A�}���`�X���b�h�œ��삵�Ă���`��R�[�h���V���O���X���b�h���s�ɐ؂�ւ��܂�
#define ONLY_SINGLE_THREAD_DEBUG OFF