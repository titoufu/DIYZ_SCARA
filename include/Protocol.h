#pragma once
#include <stddef.h>
#include <stdint.h>
namespace Diyz {
constexpr size_t MAX_LINE=128;
enum class ParseResult:uint8_t { OK, EMPTY, TOO_LONG, INVALID_SYNTAX, INVALID_ID };
struct Command { char verb[20]{}; char id[17]{}; bool hasId=false; };
ParseResult parseCommand(const char* line, Command& out);
const char* errorName(ParseResult e);
}
