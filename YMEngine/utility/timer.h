#pragma once

namespace ym
{

    class Timer
    {
    public:
        Timer() {
            if (!flag) {
                flag = true;
                QueryPerformanceFrequency(&frequency);//ü”g”‚ğŠi”[
            }
            QueryPerformanceCounter(&start);
            QueryPerformanceCounter(&end);
        }
        ~Timer()
        {

        }
        double elapsedTime()
        {
            QueryPerformanceCounter(&end);
            return (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        }
        double elapsedMiliTime()
        {
            QueryPerformanceCounter(&end);
            return (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000.f;
        }
        double elapsedNanoTime()
        {
            QueryPerformanceCounter(&end);
            return (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000.f * 1000.f;
        }
        void reset()
        {
            QueryPerformanceCounter(&start);
            QueryPerformanceCounter(&end);
        }
    private:
        static LARGE_INTEGER frequency;//ü”g”‚ğŠi”[‚·‚é•Ï”
        static bool flag;
        LARGE_INTEGER start = {}, end = {};
    };
	
}
