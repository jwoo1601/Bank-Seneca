#pragma once

#include <string>
#include <unordered_map>

class UserInfo
{
public:

	UserInfo(const std::string &username, const std::string &password)
		: m_username(username),
		  m_password(password),
		  m_balance(0)
	{

	}

	inline bool MatchesPassword(const std::string &password) const
	{
		return m_password == password;
	}

	inline const std::string& GetUsername() const
	{
		return m_username;
	}

	inline const std::string& GetPassword() const
	{
		return m_password;
	}

	inline unsigned GetBalance() const
	{
		return m_balance;
	}

	inline void Debit(unsigned amount)
	{
		m_balance += amount;
	}

	inline void Credit(unsigned amount)
	{
		if (amount > m_balance)
		{
			m_balance = 0;
		}

		else
		{
			m_balance -= amount;
		}
	}

private:
	std::string m_username;
	std::string m_password;
	unsigned    m_balance;
};

class Database
{
public:

	UserInfo* FindUser(const std::string &username)
	{
		auto found = m_userDB.find(username);
		if (found != m_userDB.end())
		{
			return &found->second;
		}

		return nullptr;
	}

	Database& AddUser(const std::string &username, const std::string &password)
	{
		if (!FindUser(username))
		{
			m_userDB.emplace(std::make_pair(username, UserInfo(username, password)));
		}

		return *this;
	}

	void SaveTo(Buffer &buffer)
	{
		for (auto iter = m_userDB.begin(); iter != m_userDB.end(); iter++)
		{
			buffer << iter->second.GetUsername() << iter->second.GetPassword();
		}
	}

private:
	std::unordered_map<std::string, UserInfo> m_userDB;
};