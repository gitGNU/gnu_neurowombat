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


#ifndef ANALOGNEURON_H
#define ANALOGNEURON_H


#include "kernel/KernelObject.h"
#include "components/analog/AnalogComparators.h"
#include "components/analog/AnalogWires.h"
#include "components/analog/AnalogResistors.h"


/***************************************************************************
 *   AnalogNeuron class declaration                                        *
 ***************************************************************************/


class AnalogNeuron : public KernelObject
   {
   public:
      AnalogNeuron(
         unsigned int numInputs = 0,
         unsigned int * inputWires = NULL,
         unsigned int gndWireIndex = 0,
         unsigned int srcWireIndex = 0,
         AnalogComparators * comparators = NULL,
         unsigned int comparatorsBaseIndex = 0,
         AnalogResistors * resistors = NULL,
         unsigned int resistorsBaseIndex = 0,
         AnalogWires * wires = NULL,
         unsigned int wiresBaseIndex = 0
         );
      virtual ~AnalogNeuron();
      virtual AnalogNeuron * clone();

      unsigned int getNumInputs() const;
      unsigned int getResistorsBaseIndex() const;

      void compute();

   private:
      unsigned int numInputs;
      unsigned int * inputWires;

      unsigned int gndWireIndex;
      unsigned int srcWireIndex;

      AnalogComparators * comparators;
      unsigned int comparatorsBaseIndex;

      AnalogResistors * resistors;
      unsigned int resistorsBaseIndex;

      AnalogWires * wires;
      unsigned int wiresBaseIndex;
   };


#endif
