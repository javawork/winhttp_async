#ifndef _EXT_PRETIMER_H
#define _EXT_PRETIMER_H

#include <stack>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>    // usleep()
#endif

// namespace, �����������, � ������ "ext", ��� ��� �������� ��� ����
// �������� ����������, ���� ����.
namespace ext {

	class PreciseTimer {
	public:
#ifdef WIN32
		// ��� int64 ��� Windows
		typedef LONGLONG Counter;
#else
		// ��� int64 ��� UNIX
		typedef long long Counter;
#endif
		PreciseTimer();

		Counter millisec();

		void mark();
		Counter release();

		static void sleepMs(int ms);
	private:
		// ��� ����� ��� �������� ������� �������.
		typedef std::stack< Counter > Counters;

		// ���� ��� �������� ������� �������.
		Counters __counters;

#ifdef WIN32
		// ��� Windows ���� ������� ��������� ������� �������.
		LARGE_INTEGER __freq;
#endif
	};

	void PreciseTimer::mark() {
		__counters.push(millisec());
	}

	PreciseTimer::Counter PreciseTimer::release() {
		if( __counters.empty() ) return -1;
		Counter val = millisec() - __counters.top();
		__counters.pop();
		return val;
	}

#ifdef WIN32

	PreciseTimer::PreciseTimer() {
		// ��� Windows � ������������ �������� ��������� ������� �������
		// (���������� ����� � �������).
		if (!QueryPerformanceFrequency(&__freq))
			__freq.QuadPart = 0;
	}

	PreciseTimer::Counter PreciseTimer::millisec() {
		LARGE_INTEGER current;
		if (__freq.QuadPart == 0 || !QueryPerformanceCounter(*t)) 
			return 0;
		// ������������� ���������� ��������� ����� � ������������.
		return current.QuadPart / (__freq.QuadPart / 1000);
	}

	void PreciseTimer::sleepMs(int ms) {
		Sleep(ms);
	}

#else // WIN32

	PreciseTimer::PreciseTimer() {}

	PreciseTimer::Counter PreciseTimer::millisec() {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}

	void PreciseTimer::sleepMs(int ms) {
		usleep(ms * 1000);
	}

#endif // WIN32

} // ext

#endif // _EXT_PRETIMER_H