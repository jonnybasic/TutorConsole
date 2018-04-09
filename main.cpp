
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

#include "ANSIGameInput.h"
#include "ANSIGameOutput.h"

#include "GameEngine.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

const Color BLACK(0, 0, 0);
const Color WHITE(255, 255, 255);

struct ANSIPoint
{
	int16_t x;
	int16_t y;

	ANSIPoint() : x(0), y(0) { }
};
struct ANSISize
{
	int16_t w;
	int16_t h;

	ANSISize() : w(0), h(0) { }
};
struct ANSIBox
{
	ANSIPoint origin;
	ANSISize size;
};

class IANSIGameObject
{
public:
	virtual bool think(double frameTime) = 0;
	virtual void draw(ANSIGameOutput *output, double frameTime) = 0;
};

class StyledObject : public IANSIGameObject
{
public:
	StyledObject()
		: m_ForeColor(WHITE),
		m_BackColor(BLACK)
	{ }

	StyledObject(Color foreColor, Color backColor)
		: m_ForeColor(foreColor),
		m_BackColor(backColor)
	{ }

	virtual ~StyledObject() { }

	virtual void setBackColor(const Color &color) {
		m_BackColor = color;
	}
	virtual void setBackColor(const byte &r, const byte &g, const byte &b) {
		m_BackColor.r = r;
		m_BackColor.g = g;
		m_BackColor.b = b;
	}
	virtual Color getBackColor() const {
		return m_BackColor;
	}

	virtual void setForeColor(const Color &color) {
		m_ForeColor = color;
	}
	virtual void setForeColor(const byte &r, const byte &g, const byte &b) {
		m_ForeColor.r = r;
		m_ForeColor.g = g;
		m_ForeColor.b = b;
	}
	virtual Color getForeColor() const {
		return m_ForeColor;
	}
	
	virtual void draw(ANSIGameOutput *output, double frameTime) {
		output->setBackColor(m_BackColor);
		output->setForeColor(m_ForeColor);
	}

private:
	Color m_ForeColor;
	Color m_BackColor;
};

class SnakeObject : public StyledObject
{
public:
	SnakeObject()
		: StyledObject::StyledObject(),
		m_BoundsChanged(false),
		m_PositionChanged(false),
		m_Bounds(),
		m_Position()
	{ }

	SnakeObject(Color foreColor, Color backColor)
		: StyledObject::StyledObject(foreColor, backColor),
		m_BoundsChanged(false),
		m_PositionChanged(false),
		m_Bounds(),
		m_Position()
	{ }

	~SnakeObject() { }

	void setBounds(const ANSIBox &box) {
		m_Bounds = box;
		m_BoundsChanged = true;
	}
	void setBounds(
		const int16_t &x,
		const int16_t &y,
		const int16_t &w,
		const int16_t &h) {
		m_Bounds.origin.x = x;
		m_Bounds.origin.y = y;
		m_Bounds.size.w = w;
		m_Bounds.size.h = h;
		m_BoundsChanged = true;
	}

	void setStartingPosition(const ANSIPoint &position) {
		m_Position = position;
		m_PositionChanged = true;
	}
	void setStartingPosition(const int16_t &x, const int16_t &y) {
		m_Position.x = x;
		m_Position.y = y;
		m_PositionChanged = true;
	}

	void moveUp() {
		if (m_Position.y > m_Bounds.origin.y)
		{
			m_PositionHistory.push_back(m_Position);
			m_Position.y--;
			m_PositionChanged = true;
		}
	}
	void moveLeft() {
		if (m_Position.x > m_Bounds.origin.x)
		{
			m_PositionHistory.push_back(m_Position);
			m_Position.x--;
			m_PositionChanged = true;
		}
	}
	void moveRight() {
		if (m_Position.x < (m_Bounds.origin.x + m_Bounds.size.w + 1))
		{
			m_PositionHistory.push_back(m_Position);
			m_Position.x++;
			m_PositionChanged = true;
		}
	}
	void moveDown() {
		if (m_Position.y < (m_Bounds.origin.y + m_Bounds.size.h + 1))
		{
			m_PositionHistory.push_back(m_Position);
			m_Position.y++;
			m_PositionChanged = true;
		}
	}

	virtual bool think(double frameTime) {
		if (m_PositionChanged
			|| m_BoundsChanged)
		{
			if (m_BoundsChanged)
			{
				// reset
				m_PositionHistory.clear();
			}
			m_PositionChanged = false;
			m_BoundsChanged = false;
			return true;
		}
		return false;
	}

	virtual void draw(ANSIGameOutput *output, double frameTime) {
		StyledObject::draw(output, frameTime);
		// draw the position history (tail)
		for (const auto &p : m_PositionHistory)
		{
			output->moveTo(p.x, p.y);
			output->print(" ");
		}
		// draw head
		output->moveTo(m_Position.x, m_Position.y);
		output->print("@");
		output->reset();
	}

private:
	bool m_BoundsChanged;
	bool m_PositionChanged;
	ANSIBox m_Bounds;
	ANSIPoint m_Position;
	vector<ANSIPoint> m_PositionHistory;
};

class BorderObject : public StyledObject
{
public:
	BorderObject() 
		: StyledObject::StyledObject(),
		m_BorderChanged(false)
	{ }

	BorderObject(Color foreColor, Color backColor)
		: StyledObject::StyledObject(foreColor, backColor),
		m_BorderChanged(true)
	{ }

	virtual ~BorderObject() { }

	virtual void setBorder(const ANSIBox &box) {
		m_Border = box;
		m_BorderChanged = true;
	}
	virtual void setBorder(
		const int16_t &x,
		const int16_t &y,
		const int16_t &w,
		const int16_t &h) {
		m_Border.origin.x = x;
		m_Border.origin.y = y;
		m_Border.size.w = w;
		m_Border.size.h = h;
		m_BorderChanged = true;
	}
	virtual ANSIBox getBorder() const {
		return m_Border;
	}

	virtual void setOrigin(const ANSIPoint &origin) {
		m_Border.origin = origin;
		m_BorderChanged = true;
	}
	virtual void setOrigin(const int16_t &x, const int16_t &y) {
		m_Border.origin.x = x;
		m_Border.origin.y = y;
		m_BorderChanged = true;
	}
	virtual ANSIPoint getOrigin() const {
		return m_Border.origin;
	}

	virtual void setSize(const ANSISize &size) {
		m_Border.size = size;
		m_BorderChanged = true;
	}
	virtual void setSize(const int16_t &w, const int16_t &h) {
		m_Border.size.w = w;
		m_Border.size.h = h;
		m_BorderChanged = true;
	}
	virtual ANSISize getSize() const {
		return m_Border.size;
	}

	virtual bool think(double frameTime) { 
		if (m_BorderChanged) {
			m_BorderChanged = false;
			return true;
		}
		return false;
	}

	virtual void draw(ANSIGameOutput *output, double frameTime) {
		StyledObject::draw(output, frameTime);
		output->drawBox(m_Border.origin.x, m_Border.origin.y, m_Border.size.w, m_Border.size.h);
		output->reset();
	}

private:
	bool m_BorderChanged;
	ANSIBox m_Border;
};

class SampleGameObject : public StyledObject
{
public:
	SampleGameObject()
		: m_TitleChanged(false),
		m_PositionChanged(false)
	{ }

	SampleGameObject(string name)
		: m_TitleChanged(true),
		m_PositionChanged(false),
		m_Title(name)
	{ }

	virtual ~SampleGameObject()
	{ }

	void setTitle(const string &title) {
		m_Title = title;
		m_TitleChanged = true;
	}
	string getTitle() const {
		return m_Title;
	}

	void setPosition(const ANSIPoint &position) {
		m_Position = position;
		m_PositionChanged = true;
	}
	void setPosition(const int16_t &x, const int16_t &y) {
		m_Position.x = x;
		m_Position.y = y;
		m_PositionChanged = true;
	}
	ANSIPoint getPosition() const {
		return m_Position;
	}

	virtual void setBackColor(const Color &color) {
		StyledObject::setBackColor(color);
		m_Border.setBackColor(color);
	}
	virtual void setBackColor(const byte &r, const byte &g, const byte &b) {
		StyledObject::setBackColor(r, g, b);
		m_Border.setBackColor(r, g, b);
	}

	virtual void setForeColor(const Color &color) {
		StyledObject::setForeColor(color);
		m_Border.setForeColor(color);
	}
	virtual void setForeColor(const byte &r, const byte &g, const byte &b) {
		StyledObject::setForeColor(r, g, b);
		m_Border.setForeColor(r, g, b);
	}

	void move(const int16_t &dX, const int16_t &dY) {
		m_Position.x += dX;
		m_Position.y += dY;
		m_PositionChanged = true;
	}

	virtual bool think(double frameTime) {
		auto hasChanged = false;
		if (m_TitleChanged) {
			m_TitleChanged = false;
			const auto length = static_cast<int>(m_Title.size()) + 4;
			m_Border.setSize(length, 3);
			hasChanged = true;
		}
		if (m_PositionChanged) {
			m_PositionChanged = false;
			m_Border.setOrigin(m_Position);
			hasChanged = true;
		}
		return hasChanged;
	}

	virtual void draw(ANSIGameOutput *output, double frameTime) {
		m_Border.draw(output, frameTime);
		StyledObject::draw(output, frameTime);
		output->moveTo(m_Position.x + 2, m_Position.y + 1);
		output->print(m_Title.c_str());
		output->reset();
	}

private:
	bool m_TitleChanged;
	bool m_PositionChanged;
	BorderObject m_Border;
	ANSIPoint m_Position;
	string m_Title;
};

class MyGame : public GameEngine
{
public:
	MyGame()
		: GameEngine::GameEngine(new ANSIGameInput(), new ANSIGameOutput()),
		m_InfoBuffer(),
		m_DoUpdate(false),
		m_DoUpdateOutput(false),
		m_ScreenHeight(0),
		m_ScreenWidth(0),
		m_Snake(BLACK, WHITE),
		m_GameBoard(nullptr)
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

	static inline int ConvertToIndex(int x, int y, int width)
	{
		return (y * width) + x;
	}

private:
	bool m_DoUpdate;
	bool m_DoUpdateOutput;
	int m_ScreenHeight;
	int m_ScreenWidth;
	byte m_LastInputModifier;
	byte m_LastInput;
	string m_InfoBuffer;
	BorderObject m_WindowBorder;
	//SampleGameObject m_MessageBox;
	SnakeObject m_Snake;
	vector<IANSIGameObject *> m_GameObjects;
	int *m_GameBoard;
};

bool MyGame::initialize(IGameInput *input, IGameOutput *output)
{
	// run at 15Hz
	setUpdateRate(24.0);
	auto ansiOutput = static_cast<ANSIGameOutput *>(output);
	ansiOutput->setIsBuffered(true);

	// Initialize the window border
	m_WindowBorder.setForeColor(0, 255, 255);

	// Initialize the message box
	//m_MessageBox.setTitle("Hello World!");
	//m_MessageBox.setForeColor(255, 0, 0);
	//m_MessageBox.setPosition(6, 5);
	m_Snake.setStartingPosition(m_ScreenWidth / 2, m_ScreenHeight / 2);


	// Add objects to vector
	m_GameObjects.push_back(&m_WindowBorder);
	m_GameObjects.push_back(&m_Snake);
	//m_GameObjects.push_back(&m_MessageBox);

	// Schedule update
	m_DoUpdate = true;
	
	return true;
}

void MyGame::shutdown()
{
	m_DoUpdate = false;
	m_DoUpdateOutput = false;

	m_GameObjects.clear();

	if (m_GameBoard != nullptr)
	{
		delete m_GameBoard;
		m_GameBoard = nullptr;
	}
}

void MyGame::update(double frameTime)
{
	if (!m_DoUpdate) {
		return;
	}

	auto updateOutput = false;

	// Update game objects
	for (auto go : m_GameObjects)
	{
		// perform the update
		updateOutput |= go->think(frameTime);
	}

	if (updateOutput) {
		m_DoUpdateOutput = true;
	}

	// Continuously update
	m_DoUpdate = true;
}

void MyGame::updateInput(const IGameInput *input)
{
	// need input
	byte userInput = 0;
	if (!input->tryGetInput(userInput))
	{
		return;
	}

	auto ss = stringstream();
	switch (userInput)
	{
		// Special Keyboard modifiers
	case 224:
		m_LastInputModifier = userInput;
		m_LastInput = userInput;
		return;

	case 27:  // ESC
	case 'q':
		setIsRunning(false);
		return;

	case 8: // Delete
		ss << "DELETE";
		break;

	case 9: // Tab
		ss << "TAB_KEY";
		break;

	case 13: // Return
		ss << "ENTER";
		break;

	default:
		switch (m_LastInputModifier)
		{
		case 224:
			switch (userInput)
			{
			case 'H':
				ss << "UP_ARROW";
				//m_MessageBox.move(0, -1);
				m_Snake.moveUp();
				break;
			case 'P':
				ss << "DOWN_ARROW";
				//m_MessageBox.move(0, 1);
				m_Snake.moveDown();
				break;
			case 'K':
				ss << "LEFT_ARROW";
				//m_MessageBox.move(-1, 0);
				m_Snake.moveLeft();
				break;
			case 'M':
				ss << "RIGHT_ARROW";
				//m_MessageBox.move(1, 0);
				m_Snake.moveRight();
				break;
			case 'G':
				ss << "HOME";
				break;
			case 'O':
				ss << "END";
				break;
			case 'R':
				ss << "INSERT";
				break;
			case 'S':
				ss << "DEL";
				break;
			case 'I':
				ss << "PAGE_UP";
				break;
			case 'Q':
				ss << "PAGE_DOWN";
				break;
			default:
				ss << "KEY_" << (int)m_LastInputModifier
					<< "_" << (int)userInput
					<< " < " << userInput << " > ";
				break;
			}
			break;

		default:
			ss << "KEY_" << (int)userInput << " [ " << userInput << " ] ";
			break;
		}
		break;
	}
	if (m_LastInput != userInput)
	{
		m_InfoBuffer = ss.str();
		m_DoUpdate = true;
		m_DoUpdateOutput = true;
		// store last input
		m_LastInput = userInput;
		// reset modifier
		m_LastInputModifier = 0;
	}
}

void MyGame::updateOutput(IGameOutput *output, double frameTime)
{
	if (!m_DoUpdateOutput) {
		return;
	}

	// immutable ANSIGameOutput pointer cast from IGameOutput argument
	const auto ansiOutput = static_cast<ANSIGameOutput *>(output);

	// Clear screen
	ansiOutput->clear();

	// Draw game objects
	for (auto go : m_GameObjects)
	{
		// perform the render
		go->draw(ansiOutput, frameTime);
	}

	// Draw instructions
	ansiOutput->moveTo(2, m_ScreenHeight - 4);
	ansiOutput->print("Instructions.");
	ansiOutput->moveTo(3, m_ScreenHeight - 3);
	ansiOutput->print("-hit 'q' to exit-");
	ansiOutput->moveTo(3, m_ScreenHeight - 2);
	ansiOutput->print(m_InfoBuffer.c_str());

	ansiOutput->flush();
	m_DoUpdateOutput = false;
}

void MyGame::updateWindow(int width, int height)
{
	m_ScreenHeight = height;
	m_ScreenWidth = width;
	// Update the border
	m_WindowBorder.setSize(width, height);
	m_Snake.setBounds(0, 0, width, height);
	// Reset the gameboard
	const auto size = width * height;
	if (m_GameBoard != nullptr)
	{
		delete m_GameBoard;
		m_GameBoard = nullptr;
	}
	m_GameBoard = new int[size];
	for (auto i = 0; i < size; i++)
	{
		m_GameBoard[i] = 0;
	}
	m_DoUpdateOutput = true;
}

int main(int argc, char** argv)
{
	auto game = MyGame();

	game.run();

	return 0;
}