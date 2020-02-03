#pragma once

#include <memory>
#include <string>
#include <cstring>
#include <cassert>

//class Buffer
//{
//public:
//
//	Buffer(size_t bufferSize)
//		: m_size(bufferSize),
//		  m_pos(0)
//	{
//		if (bufferSize > 0)
//		{
//			m_internal = new char[bufferSize];
//		}
//
//		else
//		{
//			m_internal = nullptr;
//		}
//	}
//
//	~Buffer()
//	{
//		if (m_internal)
//		{
//			delete[] m_internal;
//		}
//	}
//
//	inline size_t GetBufferSize() const
//	{
//		return m_size;
//	}
//
//	inline size_t GetPos() const
//	{
//		return m_pos;
//	}
//
//	inline size_t GetRemainingBytes() const
//	{
//		return m_size - m_pos;
//	}
//
//	inline void Clear()
//	{
//		m_pos = 0;
//	}
//
//	void Serialize(const void *pData, size_t dataSize)
//	{
//		if (m_size >= m_pos + dataSize)
//		{
//			memcpy(m_internal, pData, dataSize);
//			m_pos += dataSize;
//		}
//	}
//
//	template<typename T>
//	Buffer& SerializeArray(const T *arr, size_t arraySize)
//	{
//		Serialize(arr, sizeof(T) * arraySize);
//		*this << arraySize;
//
//		return *this;
//	}
//
//	void Deserialize(void *pData, size_t dataSize)
//	{
//		assert(m_pos - dataSize >= 0);
//
//		memcpy(pData, m_internal + m_pos - dataSize, dataSize);
//		m_pos -= dataSize;
//	}
//
//	size_t PeekArraySize() const
//	{
//		return (*((size_t*)(m_internal + m_pos - sizeof(size_t))));
//	}
//
//	template<typename T>
//	Buffer& DeserializeArray(T *pData)
//	{
//		size_t arraySize;
//		*this >> arraySize;
//		Deserialize(pData, arraySize);
//		
//		return *this;
//	}
//
//	template <typename T>
//	inline Buffer& operator<<(const T &data)
//	{
//		Serialize(&data, sizeof(T));
//
//		return *this;
//	}
//
//	inline Buffer& operator<<(const char *str)
//	{
//		SerializeArray(str, strlen(str) + 1);
//
//		return *this;
//	}
//
//	inline Buffer& operator<<(const std::string &str)
//	{
//		return *this << str.c_str();
//	}
//
//	template <typename T>
//	inline Buffer& operator>>(T &data)
//	{
//		Deserialize(&data, sizeof(T));
//
//		return *this;
//	}
//
//	inline Buffer& operator>>(char *str)
//	{
//		return DeserializeArray(str);
//	}
//
//	inline Buffer& operator>>(std::string &str)
//	{
//		char *tempStr = new char[PeekArraySize()];
//
//		*this >> tempStr;
//		str = tempStr;
//
//		delete[] tempStr;
//
//		return *this;
//	}
//
//private:
//	char *m_internal;
//	size_t m_size;
//	size_t m_pos;
//};

class Buffer
{
public:

	Buffer(size_t bufferSize)
		: m_size(bufferSize),
		  m_readIndex(0),
		  m_writeIndex(0)
	{
		if (bufferSize > 0)
		{
			m_internal = new char[bufferSize];
		}

		else
		{
			m_internal = nullptr;
		}
	}

	~Buffer()
	{
		if (m_internal)
		{
			delete[] m_internal;
		}
	}

	inline char* GetRawBuffer()
	{
		return m_internal;
	}

	inline const char* GetRawBuffer() const
	{
		return m_internal;
	}

	inline size_t GetBufferSize() const
	{
		return m_size;
	}

	inline size_t GetReadIndex() const
	{
		return m_readIndex;
	}

	inline size_t GetWriteIndex() const
	{
		return m_writeIndex;
	}

	void SetWriteIndex(size_t index)
	{
		m_writeIndex = index;
	}

	inline size_t GetRemainingBytesToRead() const
	{
		return m_writeIndex - m_readIndex;
	}

	inline size_t GetRemainingBytesToWrite() const
	{
		return m_size - m_writeIndex;
	}

	inline void ResetReadIndex()
	{
		m_readIndex = 0;
	}

	inline void Clear()
	{
		m_readIndex = 0;
		m_writeIndex = 0;
	}

	void Serialize(const void *pData, size_t dataSize)
	{
		if (m_size >= m_writeIndex + dataSize)
		{
			memcpy(m_internal + m_writeIndex, pData, dataSize);
			m_writeIndex += dataSize;
		}
	}

	template<typename T>
	Buffer& SerializeArray(const T *arr, size_t arraySize)
	{
		*this << arraySize;
		Serialize(arr, sizeof(T) * arraySize);

		return *this;
	}

	void Deserialize(void *pData, size_t dataSize)
	{
		assert(m_writeIndex - m_readIndex >= dataSize);

		memcpy(pData, m_internal + m_readIndex, dataSize);
		m_readIndex += dataSize;
	}

	size_t PeekArraySize() const
	{
		return *((size_t*)(m_internal + m_readIndex));
	}

	template<typename T>
	Buffer& DeserializeArray(T *pData)
	{
		size_t arraySize;
		*this >> arraySize;
		Deserialize(pData, sizeof(T) * arraySize);
		
		return *this;
	}

	template <typename T>
	inline Buffer& operator<<(const T &data)
	{
		Serialize(&data, sizeof(T));

		return *this;
	}

	inline Buffer& operator<<(const char *str)
	{
		SerializeArray(str, strlen(str) + 1);

		return *this;
	}

	inline Buffer& operator<<(const std::string &str)
	{
		return *this << str.c_str();
	}

	template <typename T>
	inline Buffer& operator>>(T &data)
	{
		Deserialize(&data, sizeof(T));

		return *this;
	}

	inline Buffer& operator>>(char *str)
	{
		return DeserializeArray(str);
	}

	inline Buffer& operator>>(std::string &str)
	{
		char *tempStr = new char[PeekArraySize()];

		*this >> tempStr;
		str = tempStr;

		delete[] tempStr;

		return *this;
	}

private:
	char *m_internal;
	size_t m_size;
	size_t m_readIndex;
	size_t m_writeIndex;
};