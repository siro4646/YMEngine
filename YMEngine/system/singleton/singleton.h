// Singleton.h
#pragma once

// T: �V���O���g�����������N���X���g���w�肵�܂�
template<typename T>
class Singleton
{
public:
    // �C���X�^���X�擾�B�͂��߂ČĂ΂ꂽ�Ƃ��ɂP�񂾂����������
    static T &Instance()
    {
        static T instance;  // �֐����ÓI�ϐ��iC++11�ȍ~�̓X���b�h�Z�[�t�j
        return instance;
    }

    // �R�s�[�֎~�i�V���O���g���͗B��̃C���X�^���X�Ȃ̂ŃR�s�[�����Ȃ��j
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
protected:
    Singleton() = default;  // protected �ɂ��ĊO���� new �ł��Ȃ��悤��
    ~Singleton() = default;

};
