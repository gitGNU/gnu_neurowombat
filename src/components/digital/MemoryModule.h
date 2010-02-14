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


#ifndef MEMORYMODULE_H
#define MEMORYMODULE_H


#include "components/ComponentsSet.h"
#include "engine/InterruptManager.h"
#include "objects/CustomFunction.h"


/***************************************************************************
 *   MemoryModule class declaration                                        *
 ***************************************************************************/


class MemoryModule : public ComponentsSet < double >
   {
   public:
      MemoryModule( unsigned int count = 0 );
      virtual ~MemoryModule();
   };


/***************************************************************************
 *   MemoryModuleManager class declaration                                 *
 ***************************************************************************/


class MemoryModuleManager : public InterruptManager
   {
   public:
      MemoryModuleManager();
      MemoryModuleManager(
         Distribution * distribution,
         MemoryModule * memoryModule,
         CustomFunction * fixFunction
         );

      virtual ~MemoryModuleManager();

      virtual void simulateInterrupt( unsigned int intSource );
      virtual void handleInterrupt();
      virtual void reinit();

   private:
      MemoryModule * memoryModule;
      CustomFunction * fixFunction;
      double * backup;
   };


#endif
