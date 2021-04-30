#include "tokenizer.h"

bool Tokenizer::isEOF()
{
    if (index < src.length() && lastChar != '\0' && lastChar != char(-1))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Tokenizer::isLCOF()
{
    if (index < src.length() - 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Tokenizer::advance(int offset)
{
    if ((size_t)offset + index < src.length())
    {
        index += (size_t)offset;
    }
    else
    {
        index = src.length();
    }
    lastChar = src[index];
}

void Tokenizer::pushChar()
{
    lastToken.value.push_back(lastChar);
    advance(1);
}

char Tokenizer::peek(int offset)
{
    if ((size_t)offset + index < src.length())
    {
        return src[index + (size_t)offset];
    }
    else
    {
        return src[src.length()];
    }
}

Tokenizer::Tokenizer(std::string data, Error &error) : src(data), errorHandler(error)
{

    lastChar = src[0];
    while (!isEOF())
    {
        lastToken.value.clear();
        while (isspace(lastChar))
        {
            advance(1);
        }

        if (lastChar == '/' && peek(1) == '*')
        {
            advance(2);
            while (!isEOF())
            {
                advance(1);
                if (lastChar == '*' && peek(1) == '/')
                {
                    advance(2);
                    break;
                }
            }
            if (isEOF())
            {
                errorHandler.syntax(Error::CLOSE_COMMENT, "You need to close the comment", src.c_str(), index);
            }
            continue;
        }
        else if (lastChar == '/' && peek(1) == '/')
        {
            while (!isEOF())
            {
                advance(1);
                if (lastChar == '\n')
                {
                    break;
                }
            }
            continue;
        }
        else if (lastChar == '"')
        {
            size_t startIndex = 0;
            while (!isEOF())
            {
                if (isLCOF() && lastChar != '"')
                {
                    errorHandler.syntax(Error::MISSING_QUOTATION_MARK, "Quated Char Must Be Finished", src.c_str(), index);
                }
                else if (startIndex == 1 && lastChar == '"')
                {
                    errorHandler.syntax(Error::NO_NULL_CHAR, "Char Type Must Be Initialized", src.c_str(), index);
                }
                else if (startIndex > 1 && lastChar == '"')
                {
                    if (peek(-1) == '\\')
                    {

                        pushChar();
                        ++startIndex;
                    }
                    else
                    {
                        lastToken.kind = T_STRING;
                        break;
                    }
                }
                else
                {

                    pushChar();
                    ++startIndex;
                }
            }
        }
        else if (lastChar == '\'')
        {
            pushChar();
            for (size_t sIndex = 0;; sIndex++)
            {
                if (isLCOF() && lastChar != '\'')
                {
                    errorHandler.syntax(Error::MISSING_APOSTROPHE_MARK, "Quated Char Must Be Finished", src.c_str(), index);
                    exit(1);
                }
                else if (sIndex == 1 && lastChar == '\'')
                {
                    errorHandler.syntax(Error::NO_NULL_CHAR, "Quated Char Must Be Initialized", src.c_str(), index);
                    break;
                }
                else if (lastChar == '\'' && peek(-1) != '\\')
                {
                    lastToken.kind = T_CHAR;
                    break;
                }
                else if (lastChar == '\\')
                {
                    if (peek(1) == 'a' || peek(1) == 'b' || peek(1) == 'e' || peek(1) == 'f' || peek(1) == 'n' || peek(1) == 'r' || peek(1) == 't' || peek(1) == 'v' || peek(1) == '\\' || peek(1) == '\'' || peek(1) == '"')
                    {

                        pushChar();
                        if (peek(1) != '\'')
                        {
                            errorHandler.syntax(Error::MORE_THAN_ONE_BYTE, "Char Type Can Only Contains 1 Byte", src.c_str(), index);
                            exit(1);
                        }
                    }
                    else if (peek(1) == 'x')
                    {
                    }
                    else
                    {
                        errorHandler.syntax(Error::UNSUPPORTED_ESCAPE_SEQUENCE, "Unsupported Escape Squanse", src.c_str(), index);
                        exit(1);
                    }
                }
                lastToken.value += lastChar;
                advance(1);
            }
        }
        else if (lastChar == '=')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_DOUBLE_EQUAL;
                pushChar();
            }
            else
            {
                lastToken.kind = T_EQUAL;
            }
        }
        else if (lastChar == '+')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_EQUAL_PLUS;
                pushChar();
            }
            else if (peek(1) == '+')
            {
                lastToken.kind = T_DOUBLE_PLUS;
                pushChar();
            }
            else
            {
                lastToken.kind = T_PLUS;
            }
        }
        else if (lastChar == '-')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_EQUAL_MINUS;
                pushChar();
            }
            else if (peek(1) == '-')
            {
                lastToken.kind = T_DOUBLE_MINUS;
                pushChar();
            }
            else
            {
                lastToken.kind = T_MINUS;
            }
        }
        else if (lastChar == '|')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_EQUAL_PIPE;
                pushChar();
            }
            else if (peek(1) == '|')
            {
                lastToken.kind = T_DOUBLE_PIPE;
                pushChar();
            }
            else
            {
                lastToken.kind = T_PIPE;
            }
        }
        else if (lastChar == '<')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_EQUAL_LESSER_BRACKET;
                pushChar();
            }
            else
            {
                lastToken.kind = T_LEFT_ANGLE_BRACKET;
            }
        }
        else if (lastChar == '>')
        {
            if (peek(1) == '=')
            {
                lastToken.kind = T_EQUAL_GREETER_BRACKET;
                pushChar();
            }
            else
            {
                lastToken.kind = T_RIGHT_ANGLE_BRACKET;
            }
        }
        else if (lastChar == '&')
        {
            if (peek(1) == '&')
            {
                lastToken.kind = T_LOGIC_AND;
                pushChar();
            }
            else
            {
                lastToken.kind = T_AND;
            }
        }
        else if (lastChar == '/')
        {
            lastToken.kind = T_SLASH;
        }
        else if (lastChar == '%')
        {
            lastToken.kind = T_PERCENT;
        }
        else if (lastChar == ';')
        {
            lastToken.kind = T_SEMICOLON;
        }
        else if (lastChar == '(')
        {
            lastToken.kind = T_LEFT_ROUNDED_BRACKET;
        }
        else if (lastChar == ')')
        {
            lastToken.kind = T_RIGHT_ROUNDED_BRACKET;
        }
        else if (lastChar == '{')
        {
            lastToken.kind = T_LEFT_CURLY_BRACKET;
        }
        else if (lastChar == '}')
        {
            lastToken.kind = T_RIGHT_CURLY_BRACKET;
        }
        else if (lastChar == '[')
        {
            lastToken.kind = T_LEFT_SQUARE_BRACKET;
        }
        else if (lastChar == ']')
        {
            lastToken.kind = T_RIGHT_SQUARE_BRACKET;
        }
        else if (isalpha(lastChar))
        {
            while (isalpha(lastChar))
            {
                pushChar();
            }
            if (lastToken.value == "namespace")
            {
                lastToken.kind = T_NAMESPACE;
            }
            else if (lastToken.value == "class")
            {
                lastToken.kind = T_CLASS;
            }
            else if (lastToken.value == "enum")
            {
                lastToken.kind = T_ENUM;
            }
            else if (lastToken.value == "interface")
            {
                lastToken.kind = T_INTERFACE;
            }
            else if (lastToken.value == "extends")
            {
                lastToken.kind = T_EXTENDS;
            }
            else if (lastToken.value == "implements")
            {
                lastToken.kind = T_IMPLEMENTS;
            }
            else if (lastToken.value == "abstract")
            {
                lastToken.kind = T_ABSTRACT;
            }
            else if (lastToken.value == "public")
            {
                lastToken.kind = T_PUBLIC;
            }
            else if (lastToken.value == "private")
            {
                lastToken.kind = T_PRIVATE;
            }
            else if (lastToken.value == "protected")
            {
                lastToken.kind = T_PROTECTED;
            }
            else if (lastToken.value == "static")
            {
                lastToken.kind = T_STATIC;
            }
            else if (lastToken.value == "if")
            {
                lastToken.kind = T_IF;
            }
            else if (lastToken.value == "else")
            {
                lastToken.kind = T_ELSE;
            }
            else if (lastToken.value == "for")
            {
                lastToken.kind = T_FOR;
            }
            else if (lastToken.value == "while")
            {
                lastToken.kind = T_WHILE;
            }
            else if (lastToken.value == "return")
            {
                lastToken.kind = T_RETURN;
            }
            else if (lastToken.value == "break")
            {
                lastToken.kind = T_BREAK;
            }
            else if (lastToken.value == "continue")
            {
                lastToken.kind = T_CONTINUE;
            }
            else if (lastToken.value == "using")
            {
                lastToken.kind = T_USING;
            }
            else if (lastToken.value == "final")
            {
                lastToken.kind = T_FINAL;
            }
            else if (lastToken.value == "true")
            {
                lastToken.kind = T_TRUE;
            }
            else if (lastToken.value == "false")
            {
                lastToken.kind = T_FALSE;
            }
            else if (lastToken.value == "this")
            {
                lastToken.kind = T_THIS;
            }
            else
            {
                errorHandler.syntax(Error::UNRECOGNIZED_KEYWORD, "Unrecognized Keyword", data.c_str(), index);
            }
            lastToken.index = index - lastToken.value.length();
            tokens.push_back(lastToken);
            continue;
        }
        else
        {
            errorHandler.syntax(Error::UNRECOGNIZED_TOKEN, "Unrecognized Token", data.c_str(), index);
        }
        lastToken.value += lastChar;
        lastToken.index = index - lastToken.value.length();
        tokens.push_back(lastToken);
        advance(1);
    }
}