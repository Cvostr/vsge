#pragma once

#include <string>
#include <vector>
#include "../CharBuffer.hpp"
#include "JsonToken.hpp"

namespace Mpi
{

	class JsonLexer
	{
	private:
		CharBuffer m_charBuffer;
		std::vector<JsonToken*> m_tokens;
		int back_offset;

		JsonToken* ReadNextToken();
		JsonToken* ReadNumber();
		JsonToken* ReadString();
		JsonToken* ReadIdentifier();
		JsonToken* ReadPunct();
	public:
		JsonLexer(CharBuffer& buffer);
		~JsonLexer() {}

		void SkipSpaces();
		std::string ReadToDivisor();

		JsonToken* NextToken();
		JsonToken* PrevToken();
		JsonToken* CurrToken();
		JsonToken* PrevToken(int amount);
		bool IsLastToken();
	};
}