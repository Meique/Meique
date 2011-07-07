/*
    This file is part of the Meique project
    Copyright (C) 2009-2010 Hugo Parente Lima <hugo.pl@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NOCOLOR
    #define COLOR_END "\033[0m"
    #define COLOR_WHITE "\033[1;37m"
    #define COLOR_YELLOW "\033[1;33m"
    #define COLOR_GREEN "\033[0;32m"
    #define COLOR_RED "\033[0;31m"
    #define COLOR_BLUE "\033[1;34m"
    #define COLOR_MAGENTA "\033[1;35m"
    #define COLOR_CYAN "\033[0;36m"
#else
    #define COLOR_END ""
    #define COLOR_WHITE ""
    #define COLOR_YELLOW ""
    #define COLOR_GREEN ""
    #define COLOR_RED ""
    #define COLOR_BLUE ""
    #define COLOR_MAGENTA ""
    #define COLOR_CYAN ""
#endif


#include "logger.h"

LogWriter::LogWriter(const LogWriter& other) : m_stream(other.m_stream), m_options(other.m_options & Quiet)
{
}

LogWriter::~LogWriter()
{
    if (m_options & Quiet)
        return;
    *this << nocolor();
    if (!(m_options & NoBreak))
        m_stream << std::endl;
}

bool MeiqueError::errorAlreadyset = false;

MeiqueError::MeiqueError()
{
    errorAlreadyset = true;
}

template<>
LogWriter& LogWriter::operator<<<green>(const green&)
{
    m_stream << COLOR_GREEN;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<red>(const red&)
{
    m_stream << COLOR_RED;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<yellow>(const yellow&)
{
    m_stream << COLOR_YELLOW;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<nocolor>(const nocolor&)
{
    m_stream << COLOR_END;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<blue>(const blue&)
{
    m_stream << COLOR_BLUE;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<cyan>(const cyan&)
{
    m_stream << COLOR_CYAN;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<magenta>(const magenta&)
{
    m_stream << COLOR_MAGENTA;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<white>(const white&)
{
    m_stream << COLOR_WHITE;
    return *this;
}

template<>
LogWriter& LogWriter::operator<<<nobreak>(const nobreak&)
{
    m_options |= NoBreak;
    return *this;
}

Log::Log(const std::string& fileName) : m_stream(fileName.c_str(), std::ios::out)
{
}
