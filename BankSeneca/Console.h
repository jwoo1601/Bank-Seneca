#pragma once

#include <Windows.h>

class Position
{
public:

	Position();
	Position(int x, int y);

	int GetX() const;
	int GetY() const;

	void SetX(int x);
	void SetY(int y);

	void AddX(int dx);
	void AddY(int dy);

	void Scale(int scalar);
	void Negate();
	void SetZero();

	Position operator+(const Position &p) const;
	Position operator-(const Position &p) const;

	Position& operator+=(const Position &p);
	Position& operator-=(const Position &p);

	bool operator==(const Position &p) const;
	bool operator!=(const Position &p) const;

public:
	static const Position Zero;

private:
	int m_x;
	int m_y;
};

enum class EConsoleColor : unsigned char
{
	Default,
	Black,
	White,
	Red,
	Green,
	Blue,
	Yellow, // Red + Green
	Magenta, // Red + Blue
	Cyan // Green + Blue
};

enum class EConsoleStyle : unsigned char
{
	None      = 0x00,
	Intensify = 0x01,

	Default   = 0xFF
};

class Console
{
public:

	Console& Display();
	Console& Hide();

	Console& ShowCursor();
	Console& HideCursor();
	Position GetCursorPosition() const;
	Console& SetCursorPosition(const Position &pos);

	int CreateBuffer();
	void RemoveBuffer(int bufferId);

	void SetActiveBuffer(int bufferId);
	int GetActiveBuffer() const;

	Console& Write(const char *str,
		           EConsoleColor foregroundColor = EConsoleColor::Default,
		           EConsoleColor backgroundColor = EConsoleColor::Default,
		           EConsoleStyle textStyle = EConsoleStyle::Default);
	Console& Read(char *readBuffer, size_t bufferSize);

	Console& Printf();
	Console& Scanf();

	Console& SetForegroundColor(EConsoleColor color);
	Console& SetBackgroundColor(EConsoleColor color);
	Console& SetStyle(EConsoleStyle style);

	static Console& GetCurrentConsole();

private:

	Console();
	Console(HANDLE handle);
	Console(HANDLE handle, int width, int height);
	~Console();

private:
	HANDLE m_handle;
	int m_width, m_height;
	int *m_screenBuffers;
	Position m_cursor;
};