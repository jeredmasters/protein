// graphics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "renderer.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
	robot* _robot = new robot();
	renderer* _renderer = new renderer(500, 500);

	#pragma omp parallel for
	for (int i = 0; i < 10; i++) {
		cout << "thread: " << i << "\n";
	}

	while (true) {
		_renderer->update(_robot);
		Sleep(5);
	}
}

