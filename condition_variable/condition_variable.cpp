#include <iostream>
#include<windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

using int32 = __int32;
using namespace std;

mutex m;
queue<int32> q;
						//condition_variable은 참고로 handleevent와 다르게 유저레벨에서 동작한다
condition_variable cv;//c++표준이라 mutex헤더안에 포함되어있다고한다. mutex랑 묶여서 사용

#include<condition_variable> // 좀더 표준적인 사용을위해서는 헤더를 선언해야 condition_vafiable_any등 사용가능
//condition_variable_any cv;



void Producer() {

	while (true) {
		//1)lock을잡고 
		//2)공유 변수 값을 수정
		//3)lock을 풀고
		//4)조건변수 통해 다른 쓰레드에게 통지
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one();//하나의 스레드에게 알림.
		//cv.notify_all();//모든 스레드에게 알림.
		this_thread::sleep_for(10000ms);
	}


}

void Consumer() {

	while (true) {

		unique_lock<mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });
		// 1) Lock을 잡고
		// 2) 조건 확인
		//  -만족0=> 빠져 나와서 이어서 코드를 진행
		//  -만족x=>lock을 풀어주고 대기 상태
		//  wait의 경우 lock_guard가 아닌 unique_lock만 인자로 되어있다. 이유는 wait처럼 조건에 따라 작동을 해야하기때문인듯
		
		// notify_one을 통해 락을 통해 wait에서 깨어 났다면 무조건 조건식을 만족하는게 아닌지? => 아님 notify one()이끝나자마자 다시 락을 취득하기전에 다른 곳에서 끼어치기 하고 공유 자원을 건드릴수있음.
		// 이런 상황을 Spurious Wakeup (가짜 기상) 이라고하는데 그렇기에 wait에 조건으로 자원을 한번 더 확인

		{
			int32 data = q.front();
			q.pop();
			cout << data << endl; //스레드로 실행중인 영역에서 cout등 커널을다시부르는 습관은 좋지 않다고 한다. 현재는 공부용이니 진행
		}
	}

}


int main()
{
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();
}
