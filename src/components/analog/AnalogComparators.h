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


#ifndef ANALOGCOMPARATORS_H
#define ANALOGCOMPARATORS_H


#include "kernel/KernelObject.h"
//#include "TransferFunction.h"


/***************************************************************************
 *   AnalogComparators class declaration                                   *
 ***************************************************************************/


class AnalogComparators : public KernelObject
   {
   public:
      AnalogComparators( unsigned int count = 0 );
      ~AnalogComparators();

      unsigned int count() const;

      double compare( unsigned int index, double neg, double pos );

   private:
      unsigned int numComparators;
      double * analogSignals;
   };


#endif
