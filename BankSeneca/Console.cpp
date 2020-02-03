#include "Console.h"

Position const Position::Zero = Position(0, 0);

Position::Position()
	: Position(0, 0)
{

}

Position::Position(int x, int y)
	: m_x(x),
	  m_y(y)
{

}

inline int Position::GetX() const
{
	return m_x;
}

inline int Position::GetY() const
{
	return m_y;
}

inline void Position::SetX(int x)
{
	m_x = x;
}

inline void Position::SetY(int y)
{
	m_y = y;
}

inline void Position::AddX(int dx)
{
	m_x += dx;
}

inline void Position::AddY(int dy)
{
	m_y += dy;
}

inline void Position::Scale(int scalar)
{
	m_x *= scalar;
	m_y *= scalar;
}

inline void Position::Negate()
{
	m_x *= -1;
	m_y *= -1;
}

inline void Position::SetZero()
{
	m_x = 0;
	m_y = 0;
}

inline Position Position::operator+(const Position &p) const
{
	return Position(m_x + p.m_x, m_y + p.m_y);
}

inline Position Position::operator-(const Position &p) const
{
	return Position(m_x - p.m_x, m_y - p.m_y);
}

inline Position& Position::operator+=(const Position &p)
{
	m_x += p.m_x;
	m_y += p.m_y;

	return *this;
}

inline Position& Position::operator-=(const Position &p)
{
	m_x -= p.m_x;
	m_y -= p.m_y;

	return *this;
}

inline bool Position::operator==(const Position &p) const
{
	return m_x == p.m_x && m_y == p.m_y;
}

inline bool Position::operator!=(const Position &p) const
{
	return !(*this == p);
}