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


#ifndef CONSTANTS_H
#define CONSTANTS_H


#include <lua.hpp>


void registerApiConstants( lua_State * L );


/***************************************************************************
 *   Commonn API functions declaration                                     *
 ***************************************************************************/


extern "C" int newIndexHandler( lua_State * L );


/***************************************************************************
 *   Registration functions declaration                                    *
 ***************************************************************************/

inline void registerActivationFunctions( lua_State * L );


inline void registerProcessingUnits( lua_State * L );


inline void registerCoefficientUsage( lua_State * L );


inline void registerDistributions( lua_State * L );


#endif
