/***************************************************************************
 *   Copyright (C) 2009 Andrew Timashov                                    *
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


#ifndef KERNEL_H
#define KERNEL_H


#include <map>
#include "lua.hpp"


#include "kernel/KernelObject.h"
#include "patterns/Singleton.h"


/***************************************************************************
 *   KernelObjectTable class declaration                                   *
 ***************************************************************************/


class KernelObjectTable
   {
   public:
      enum CONSTANTS
         {
         MIN_ID = 0x00000001,
         MAX_ID = 0xFFFFFFFF
         };

      KernelObjectTable();
      virtual ~KernelObjectTable();

      // This function is NULL safe;
      KernelObjectId createId( KernelObject * object );
      void closeId( KernelObjectId id );
      void clear();

      KernelObject * getObject( KernelObjectId id );
   private:
      bool idSearchRequired;
      unsigned int idCount;
      KernelObjectId currentId;

      std::map < KernelObjectId, KernelObject * > objects;
   };


/***************************************************************************
 *   Kernel singleton class declaration                                    *
 ***************************************************************************/


class Kernel : public Singleton < Kernel >
   {
   friend class Singleton < Kernel >;

   public:
      // This function is NULL safe;
      KernelObjectId insertObject( KernelObject * object );
      void deleteObject( KernelObjectId id );
      KernelObject * getObject( KernelObjectId id );

      void doFile( const char * fileName );

   protected:
      Kernel();
      virtual ~Kernel();
   private:
      KernelObjectTable objectTable;
      lua_State * luaVM;
   };


#endif
