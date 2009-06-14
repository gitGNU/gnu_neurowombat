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


#ifndef ABSTRACTACTIVATORS_H
#define ABSTRACTACTIVATORS_H


#include "kernel/KernelObject.h"
#include "math/TransferFunction.h"


/***************************************************************************
 *   AbstractActivators class declaration                                  *
 ***************************************************************************/


class AbstractActivators : public KernelObject
   {
   public:
      AbstractActivators(
         unsigned int count = 0,
         TransferFunction * transferFunction = NULL
         );
      ~AbstractActivators();

      void setSignal( unsigned int index, double signal );
      double getSignal( unsigned int index ) const;

   private:
      unsigned int numNonLinearElements;
      double * analogSignals;
      TransferFunction * transferFunction;
   };


#endif
