
#include <iostream>

#include "ANSIGameInput.h"
#include "ANSIGameOutput.h"

using std::cout;
using std::cerr;
using std::endl;

const Color BLACK(0, 0, 0);
const Color WHITE(255, 255, 255);

int main(int argc, char** argv) 
{
	auto gameInput = ANSIGameInput();
	auto gameOutput = ANSIGameOutput();

	if (gameInput.initialize() 
		&& gameOutput.initialize())
	{
		int height;
		int width;
		gameOutput.getViewport(width, height);

		// Draw a border around the screen
		Color borderColor(0, 255, 255);
		gameOutput.setForeColor(borderColor);
		gameOutput.drawBox(0, 0, width, height);
		gameOutput.reset();

		// Draw a title
		Color red(255, 0, 0);
		gameOutput.setForeColor(red);
		gameOutput.drawBox(4, 4, 15, 3);
		gameOutput.moveTo(6, 5);
		gameOutput.print("Hello World");
		gameOutput.reset();

		// Draw instructions
		gameOutput.moveTo(2, height - 3);
		gameOutput.print("Instructions.");
		gameOutput.moveTo(3, height - 2);
		gameOutput.print("-hit any key to exit-");
		
		auto inChar = gameInput.getInput();

		gameOutput.shutdown();
		gameInput.shutdown();
		return 0;
	}
	else
	{
		cerr << "Unable to initialize engine!" << endl;
		return -1;
	}
}