// graphics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "renderer.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
	renderer* r = new renderer();

	while (true) {
		r->update();
		Sleep(5);
	}
}

