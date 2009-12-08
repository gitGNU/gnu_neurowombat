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


#ifndef ANALOGRESISTORS_H
#define ANALOGRESISTORS_H


#include "kernel/KernelObject.h"
#include "engine/InterruptManager.h"
#include "objects/CustomFunction.h"


/***************************************************************************
 *   AnalogResistors class declaration                                     *
 ***************************************************************************/


class AnalogResistors : public KernelObject
   {
   public:
      AnalogResistors( unsigned int count = 0 );
      ~AnalogResistors();

      unsigned int count() const;

      void setResistance( unsigned int index, double resistance );
      double getResistance( unsigned int index ) const;

   private:
      unsigned int numResistors;
      double * resistances;
   };


/***************************************************************************
 *   AnalogResistorsManager class declaration                              *
 ***************************************************************************/


class AnalogResistorsManager : public InterruptManager
   {
   public:
      AnalogResistorsManager();
      AnalogResistorsManager(
         Destribution * destribution,
         AnalogResistors * analogResistors,
         CustomFunction * fixFunction
         );

      virtual ~AnalogResistorsManager();

      virtual void simulateInterrupt( unsigned int intSource );
      virtual void handleInterrupt();
      virtual void reinit();

   private:
      AnalogResistors * analogResistors;
      CustomFunction * fixFunction;
      double * resistancesBackup;
   };


#endif
