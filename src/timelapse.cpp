#include <iostream>
#include <chrono>
#include <unistd.h>
using namespace std;

int main()
{
	auto start = chrono::steady_clock::now();

	// do some stuff here
	//sleep(3);
	int i = 0;
	while (i > 10000)
	{
		i++;
	}

	auto end = chrono::steady_clock::now();

	cout << "Elapsed time in nanoseconds: " <<
		chrono::duration_cast<chrono::nanoseconds > (end - start).count() <<
		" ns" << endl;

	cout << "Elapsed time in microseconds: " <<
		chrono::duration_cast<chrono::microseconds > (end - start).count() <<
		" µs" << endl;

	cout << "Elapsed time in milliseconds: " <<
		chrono::duration_cast<chrono::milliseconds > (end - start).count() <<
		" ms" << endl;

	cout << "Elapsed time in seconds: " <<
		chrono::duration_cast<chrono::seconds > (end - start).count() <<
		" sec";

	return 0;
}

/*
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    long i = 0;
    while (i > 1000000) { 	i++;
}

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference microseconds = " << 
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference nanoseconds = " << 
        std::chrono::duration_cast<std::chrono::nanoseconds > (end - begin).count() << "[ns]" << std::endl;

    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    std::cout << "Time difference milliseconds = " << 
        std::chrono::duration_cast<std::chrono::milliseconds > (end - begin).count() << "[ms]" << std::endl;

    std::cout << "Test: " << millis;
}
*/

/*
 {
    auto start = std::chrono::system_clock::now();
    int i = 0;
    while (i > 10000) { 	i++;
}
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
 }
 */