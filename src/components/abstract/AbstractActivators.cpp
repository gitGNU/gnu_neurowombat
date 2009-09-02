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
   ActivationFunction * activationFunction
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

   this->activationFunction = activationFunction;
   if ( activationFunction != NULL ) activationFunction->capture();
   };


AbstractActivators::~AbstractActivators()
   {
   if ( analogSignals != NULL ) delete[] analogSignals;

   // Release captured object;
   if ( activationFunction != NULL ) activationFunction->release();
   };


void AbstractActivators::setSignal( unsigned int index, double signal )
   {
   this->analogSignals[ index ] = signal;
   };


double AbstractActivators::getSignal( unsigned int index ) const
   {
   return this->activationFunction->evaluateFunction( this->analogSignals[ index ] );
   };


double AbstractActivators::evaluateDerivative( double x )
   {
   return this->activationFunction->evaluateDerivative( x );
   };
