/***************************************************************************
 *   Copyright (C) 2009, 2010 Andrew Timashov                              *
 *                                                                         *
 *   This file is part of NeuroWombat.                                     *
 *                                                                         *
 *   NeuroWombat is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   NeuroWombat is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with NeuroWombat.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "objects/CustomFunction.h"


#include "kernel/Kernel.h"


// It is better for API functions to use this pointer instead of
// Kernel::instance() and Kernel::freeInstance() methods;
extern Kernel * kernel;


/***************************************************************************
 *   CustomFunction abstract class implementation                          *
 ***************************************************************************/


CustomFunction::CustomFunction( int index )
   : KernelObject()
   {
   lua_State * L = kernel->getVM();
   lua_pushvalue( L, index );
   functionReference = luaL_ref( L, LUA_REGISTRYINDEX );
   };


CustomFunction::~CustomFunction()
   {
   luaL_unref( kernel->getVM(), LUA_REGISTRYINDEX, functionReference );
   };


void CustomFunction::call() const
   {
   lua_State * L = kernel->getVM();
   lua_rawgeti( L, LUA_REGISTRYINDEX, functionReference );
   lua_pcall( L, 0, 0, 0 );
   };


double CustomFunction::call( double x ) const
   {
   lua_State * L = kernel->getVM();
   lua_rawgeti( L, LUA_REGISTRYINDEX, functionReference );
   lua_pushnumber( L, x );
   lua_pcall( L, 1, 1, 0 );
   double result = lua_tonumber( L, -1 );
   lua_pop( L, 1 );
   return result;
   };


int CustomFunction::getFunctionReference() const
   {
   return functionReference;
   };


CustomFunction::CustomFunction()
   : KernelObject()
   {
   // Do nothing;
   };


CustomFunction::CustomFunction( const CustomFunction & other )
   {
   // Do nothing;
   };


CustomFunction & CustomFunction::operator =( const CustomFunction & other )
   {
   // Do nothing;
   return * this;
   };
