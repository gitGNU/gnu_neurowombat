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


#include "api/constants.h"
#include "math/ActivationFunction.h"
#include "math/ProcessingUnit.h"
#include "math/Distribution.h"


#include <stdio.h>


void registerApiConstants( lua_State * L )
   {
   registerActivationFunctions( L );
   registerProcessingUnits( L );
   registerCoefficientUsage( L );
   registerDistributions( L );
   };


/***************************************************************************
 *   Abstract neuron API functions implementation                          *
 ***************************************************************************/


int newIndexHandler( lua_State * L )
   {
   printf( "Error!!!\n" );
   return 0;
   };


/***************************************************************************
 *   Registration functions implementation                                 *
 ***************************************************************************/


void registerActivationFunctions( lua_State * L )
   {
   // Create an empty table;
   lua_newtable( L );

   // Create metatable;
   lua_newtable( L );
   lua_pushstring( L, "__index" );

   // Create table to be set as __index;
   lua_newtable( L );
   lua_pushstring( L, "CUSTOM" );
   lua_pushnumber( L, ACT_FUNC::CUSTOM );
   lua_rawset( L, -3 );
   lua_pushstring( L, "GAUSSIAN" );
   lua_pushnumber( L, ACT_FUNC::GAUSSIAN );
   lua_rawset( L, -3 );
   lua_pushstring( L, "LIM" );
   lua_pushnumber( L, ACT_FUNC::LIM );
   lua_rawset( L, -3 );
   lua_pushstring( L, "LINEAR" );
   lua_pushnumber( L, ACT_FUNC::LINEAR );
   lua_rawset( L, -3 );
   lua_pushstring( L, "LIMLINEAR" );
   lua_pushnumber( L, ACT_FUNC::LIMLINEAR );
   lua_rawset( L, -3 );
   lua_pushstring( L, "POSLINEAR" );
   lua_pushnumber( L, ACT_FUNC::POSLINEAR );
   lua_rawset( L, -3 );
   lua_pushstring( L, "SIGMOID" );
   lua_pushnumber( L, ACT_FUNC::SIGMOID );
   lua_rawset( L, -3 );
   lua_pushstring( L, "THSIGMOID" );
   lua_pushnumber( L, ACT_FUNC::THSIGMOID );
   lua_rawset( L, -3 );

   // Set this table as __index field for metatable;
   lua_rawset( L, -3 );

   lua_pushstring( L, "__newindex" );
   lua_pushcfunction( L, newIndexHandler );
   lua_rawset( L, -3 );

   // Set metatable to an empty table;
   lua_setmetatable( L, -2 );

   // Register this table;
   lua_setglobal( L, "ACT_FUNC" );
   };


void registerProcessingUnits( lua_State * L )
   {
   // Create an empty table;
   lua_newtable( L );

   // Create metatable;
   lua_newtable( L );
   lua_pushstring( L, "__index" );

   // Create table to be set as __index;
   lua_newtable( L );
   lua_pushstring( L, "CUSTOM" );
   lua_pushnumber( L, PROC_UNIT::CUSTOM );
   lua_rawset( L, -3 );
   lua_pushstring( L, "RADIAL_BASIS" );
   lua_pushnumber( L, PROC_UNIT::RADIAL_BASIS );
   lua_rawset( L, -3 );
   lua_pushstring( L, "SCALAR" );
   lua_pushnumber( L, PROC_UNIT::SCALAR );
   lua_rawset( L, -3 );
   lua_pushstring( L, "WEIGHTED_SUM" );
   lua_pushnumber( L, PROC_UNIT::WEIGHTED_SUM );
   lua_rawset( L, -3 );

   // Set this table as __index field for metatable;
   lua_rawset( L, -3 );

   lua_pushstring( L, "__newindex" );
   lua_pushcfunction( L, newIndexHandler );
   lua_rawset( L, -3 );

   // Set metatable to an empty table;
   lua_setmetatable( L, -2 );

   // Register this table;
   lua_setglobal( L, "PROC_UNIT" );
   };


void registerCoefficientUsage( lua_State * L )
   {
   // Create an empty table;
   lua_newtable( L );

   // Create metatable;
   lua_newtable( L );
   lua_pushstring( L, "__index" );

   // Create table to be set as __index;
   lua_newtable( L );
   lua_pushstring( L, "NOP" );
   lua_pushnumber( L, COEFF_USAGE::NOP );
   lua_rawset( L, -3 );
   lua_pushstring( L, "ADD_TO" );
   lua_pushnumber( L, COEFF_USAGE::ADD_TO );
   lua_rawset( L, -3 );
   lua_pushstring( L, "MUL_BY" );
   lua_pushnumber( L, COEFF_USAGE::MUL_BY );
   lua_rawset( L, -3 );
   lua_pushstring( L, "SUB_IT_FROM" );
   lua_pushnumber( L, COEFF_USAGE::SUB_IT_FROM );
   lua_rawset( L, -3 );
   lua_pushstring( L, "SUB_FROM_IT" );
   lua_pushnumber( L, COEFF_USAGE::SUB_FROM_IT );
   lua_rawset( L, -3 );
   lua_pushstring( L, "DIV_IT_BY" );
   lua_pushnumber( L, COEFF_USAGE::DIV_IT_BY );
   lua_rawset( L, -3 );
   lua_pushstring( L, "DIV_BY_IT" );
   lua_pushnumber( L, COEFF_USAGE::DIV_BY_IT );
   lua_rawset( L, -3 );

   // Set this table as __index field for metatable;
   lua_rawset( L, -3 );

   lua_pushstring( L, "__newindex" );
   lua_pushcfunction( L, newIndexHandler );
   lua_rawset( L, -3 );

   // Set metatable to an empty table;
   lua_setmetatable( L, -2 );

   // Register this table;
   lua_setglobal( L, "COEFF_USAGE" );
   };


void registerDistributions( lua_State * L )
   {
   // Create an empty table;
   lua_newtable( L );

   // Create metatable;
   lua_newtable( L );
   lua_pushstring( L, "__index" );

   // Create table to be set as __index;
   lua_newtable( L );
   lua_pushstring( L, "CUSTOM" );
   lua_pushnumber( L, DISTR::CUSTOM );
   lua_rawset( L, -3 );
   lua_pushstring( L, "EXP" );
   lua_pushnumber( L, DISTR::EXP );
   lua_rawset( L, -3 );
   lua_pushstring( L, "WEIBULL" );
   lua_pushnumber( L, DISTR::WEIBULL );
   lua_rawset( L, -3 );

   // Set this table as __index field for metatable;
   lua_rawset( L, -3 );

   lua_pushstring( L, "__newindex" );
   lua_pushcfunction( L, newIndexHandler );
   lua_rawset( L, -3 );

   // Set metatable to an empty table;
   lua_setmetatable( L, -2 );

   // Register this table;
   lua_setglobal( L, "DISTR" );
   };
