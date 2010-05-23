/*
    This file is part of the Meique project
    Copyright (C) 2010 Hugo Parente Lima <hugo.pl@gmail.com>

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

#include "compilabletarget.h"
#include "luacpputil.h"
#include "compiler.h"
#include "logger.h"
#include "config.h"
#include "os.h"
#include "filehash.h"
#include "compileroptions.h"
#include "linkeroptions.h"

CompilableTarget::CompilableTarget(const std::string& targetName, MeiqueScript* script)
    : Target(targetName, script), m_compilerOptions(0), m_linkerOptions(0)
{
}

CompilableTarget::~CompilableTarget()
{
    delete m_compilerOptions;
    delete m_linkerOptions;
}

void CompilableTarget::doRun(Compiler* compiler)
{
    // get sources
    getLuaField("_files");
    StringList files;
    readLuaList(luaState(), lua_gettop(luaState()), files);

    if (files.empty())
        Error() << "Compilable target '" << name() << "' has no files!";

    if (!m_compilerOptions)
        fillCompilerAndLinkerOptions();

    std::string sourceDir = config().sourceRoot() + directory();

    bool needLink = false;
    StringList objects;
    StringList::const_iterator it = files.begin();
    for (; it != files.end(); ++it) {
        std::string source = sourceDir + *it;
        std::string output = *it + ".o";

        std::string hash = FileHash(source).toString();
        if (!OS::fileExists(output) || hash != config().fileHash(source)) {
            if (!compiler->compile(source, output, m_compilerOptions))
                Error() << "Compilation fail!";
            needLink = true;
        }
        config().setFileHash(source, hash);
        objects.push_back(output);
    }

    if (needLink)
        compiler->link(name(), objects, m_linkerOptions);
    // send them to the compiler
}

void CompilableTarget::fillCompilerAndLinkerOptions()
{
    m_compilerOptions = new CompilerOptions;
    m_linkerOptions = new LinkerOptions;
    getLuaField("_packages");
    lua_State* L = luaState();
    // loop on all used packages
    int tableIndex = lua_gettop(L);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, tableIndex) != 0) {

        StringMap map;
        readLuaTable<StringMap>(L, lua_gettop(L), map);

        m_compilerOptions->addIncludePath(map["includePaths"]);
        m_compilerOptions->addCustomFlag(map["cflags"]);
        m_linkerOptions->addCustomFlag(map["linkerFlags"]);
        m_linkerOptions->addLibraryPath(map["libraryPaths"]);
        m_linkerOptions->addLibrary(map["linkLibraries"]);
        lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
    }
}
