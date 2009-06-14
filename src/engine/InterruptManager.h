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


#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H


#include "kernel/KernelObject.h"
#include "math/Destribution.h"


/***************************************************************************
 *   InterruptManager abstract class declaration                           *
 ***************************************************************************/


class InterruptManager : public KernelObject
   {
   public:
      InterruptManager();
      InterruptManager(
         int numIntSources,
         bool unlimitRegeneration,
         Destribution * destribution
         );

      virtual ~InterruptManager();

      double getInterrupt();
      unsigned int getInterruptsCount();

      // Base method should be called at the end of reimplementation;
      virtual void handleInterrupt() = 0;

   protected:
      int getIntSource();

   private:
      // Most time-consuming operation;
      void findOutIntSource();

      unsigned int interruptsCount;

      int numIntSources;
      double * interrupts;

      int intSource;

      bool unlimitRegeneration;
      Destribution * destribution;
   };


#endif
