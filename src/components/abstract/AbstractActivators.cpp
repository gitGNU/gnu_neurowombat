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


#include "components/abstract/AbstractActivators.h"


/***************************************************************************
 *   AbstractActivators class implementation                               *
 ***************************************************************************/


AbstractActivators::AbstractActivators(
   unsigned int count,
   TransferFunction * transferFunction
   )
   : KernelObject()
   {
   this->numNonLinearElements = count;

   if ( count > 0 )
      {
      this->analogSignals = new double[ count ];
      }
   else
      {
      this->analogSignals = NULL;
      }

   if ( transferFunction != NULL )
      {
      this->transferFunction = transferFunction->clone();
      }
   else
      {
      this->transferFunction = NULL;
      }
   };


AbstractActivators::~AbstractActivators()
   {
   if ( this->analogSignals != NULL ) delete[] this->analogSignals;
   if ( this->transferFunction != NULL ) delete this->transferFunction;
   };


void AbstractActivators::setSignal( unsigned int index, double signal )
   {
   this->analogSignals[ index ] = signal;
   };


double AbstractActivators::getSignal( unsigned int index ) const
   {
   return this->transferFunction->evaluateFunction( this->analogSignals[ index ] );
   };
