
#include <iostream>
#include <sstream>
#include <string>

#include "ANSIGameInput.h"
#include "ANSIGameOutput.h"

#include "GameEngine.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;

const Color BLACK(0, 0, 0);
const Color WHITE(255, 255, 255);

class MyGame : public GameEngine
{
public:
	MyGame() 
		: GameEngine::GameEngine(new ANSIGameInput(), new ANSIGameOutput()),
		m_InfoBuffer()
	{ }

	virtual ~MyGame()
	{ }

protected:
	virtual bool initialize(IGameInput *input, IGameOutput *output);
	virtual void shutdown();
	virtual void update(double frameTime);
	virtual void updateInput(const IGameInput *input);
	virtual void updateOutput(IGameOutput *output, double frameTime);
	virtual void updateWindow(int width, int height);

private:
	int m_ScreenHeight;
	int m_ScreenWidth;
	byte m_LastInput;
	string m_InfoBuffer;
};

bool MyGame::initialize(IGameInput *input, IGameOutput *output)
{
	// run at 15Hz
	setUpdateRate(15.0);
	return true;
}

void MyGame::shutdown()
{

}

void MyGame::update(double frameTime)
{
	//m_InfoBuffer.clear();
}

void MyGame::updateInput(const IGameInput *input)
{
	// clear info buffer
	m_InfoBuffer = "";
	byte userInput = 0;
	if (input->tryGetInput(userInput))
	{
		auto ss = stringstream();
		// store last input
		m_LastInput = userInput;
		switch (userInput)
		{
		case 'q':
			setIsRunning(false);
			break;
		}
		// write to the info buffer
		ss << "KEY_" << (int)m_LastInput << "    ";
		m_InfoBuffer = ss.str();
	}
}

void MyGame::updateOutput(IGameOutput *output, double frameTime)
{
	const auto ansiOutput = static_cast<ANSIGameOutput *>(output);

	// Clear screen
	//ansiOutput->clear();

	// Draw a border around the screen
	Color borderColor(0, 255, 255);
	ansiOutput->setForeColor(borderColor);
	ansiOutput->drawBox(0, 0, m_ScreenWidth, m_ScreenHeight);
	ansiOutput->reset();

	// Draw a title
	Color red(255, 0, 0);
	ansiOutput->setForeColor(red);
	ansiOutput->drawBox(4, 4, 15, 3);
	ansiOutput->moveTo(6, 5);
	ansiOutput->print("Hello World");
	ansiOutput->reset();

	// Draw instructions
	ansiOutput->moveTo(2, m_ScreenHeight - 4);
	ansiOutput->print("Instructions.");
	ansiOutput->moveTo(3, m_ScreenHeight - 3);
	ansiOutput->print("-hit 'q' to exit-");
	ansiOutput->moveTo(3, m_ScreenHeight - 2);
	ansiOutput->print(m_InfoBuffer.c_str());
}

void MyGame::updateWindow(int width, int height)
{
	m_ScreenHeight = height;
	m_ScreenWidth = width;
}

int main(int argc, char** argv) 
{
	auto game = MyGame();

	game.run();

	return 0;
}