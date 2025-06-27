// Singleton.h
#pragma once

// T: シングルトン化したいクラス自身を指定します
template<typename T>
class Singleton
{
public:
    // インスタンス取得。はじめて呼ばれたときに１回だけ生成される
    static T &Instance()
    {
        static T instance;  // 関数内静的変数（C++11以降はスレッドセーフ）
        return instance;
    }

    // コピー禁止（シングルトンは唯一のインスタンスなのでコピーさせない）
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
protected:
    Singleton() = default;  // protected にして外から new できないように
    ~Singleton() = default;

};
