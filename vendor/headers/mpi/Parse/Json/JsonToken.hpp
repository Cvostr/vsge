#pragma once

#include <string>

namespace Mpi
{
	enum JsonTokenType
	{
		JSON_TOKEN_TYPE_PUNCT,
		JSON_TOKEN_TYPE_STRING,
		JSON_TOKEN_TYPE_NUMBER,
		JSON_TOKEN_TYPE_IDENTIFIER
	};

	class JsonToken
	{
	private:
		JsonTokenType	m_type;
		std::string		m_strValue;
		char			m_charValue;
	public:
		JsonToken(JsonTokenType type, unsigned int pos, const std::string& str);
		JsonToken(JsonTokenType type, unsigned int pos, const char chr);
		~JsonToken() {}
		JsonTokenType getType() const;
		const std::string& getString() const;
		const char getChar() const;
		bool checkPunct(char punct) const;
		bool checkIdentifier(const std::string& ident) const;
	};
}