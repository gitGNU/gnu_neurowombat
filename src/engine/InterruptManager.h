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


#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H


#include "kernel/KernelObject.h"
#include "math/Distribution.h"


/***************************************************************************
 *   InterruptManager abstract class declaration                           *
 ***************************************************************************/


class InterruptManager : public KernelObject
   {
   public:
      InterruptManager(
         unsigned int intSourcesCount,
         bool unlimitedRegeneration,
         Distribution * distribution
         );

      virtual ~InterruptManager();

      double getInterrupt();
      int getLastIntSource();
      int getIntSource();
      unsigned int getIntSourcesCount() const;
      unsigned int getInterruptsCount() const;

      virtual void simulateInterrupt( unsigned int intSource ) = 0;

      // Base method should be called at the end of reimplementation;
      virtual void handleInterrupt() = 0;

      // Base method should be called in reimplementation;
      virtual void reinit() = 0;

   protected:
      InterruptManager();

      unsigned int intSourcesCount;

   private:
      // Most time-consuming operation;
      void findOutIntSource();

      unsigned int interruptsCount;

      double * interrupts;

      int intSource;
      int lastIntSource;

      bool unlimitedRegeneration;
      Distribution * distribution;
   };


#endif
