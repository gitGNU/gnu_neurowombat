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


#include "components/abstract/AbstractWeights.h"


#include <string.h>


AbstractWeights::AbstractWeights( unsigned int count )
   : KernelObject()
   {
   this->numWeights = count;

   if ( count > 0 )
      {
      this->weights = new double[ count ];
      }
   else
      {
      this->weights = NULL;
      }
   };


AbstractWeights::~AbstractWeights()
   {
   if ( this->weights != NULL ) delete[] this->weights;
   };


unsigned int AbstractWeights::count() const
   {
   return this->numWeights;
   };


void AbstractWeights::setupWeights( unsigned int baseIndex, double * weights, unsigned int count )
   {
   memcpy( & this->weights[ baseIndex ], weights, count * sizeof( double ) );
   };


void AbstractWeights::setWeight( unsigned int index, double weight )
   {
   this->weights[ index ] = weight;
   };


double AbstractWeights::getWeight( unsigned int index ) const
   {
   return this->weights[ index ];
   };


/***************************************************************************
 *   AbstractWeightsManager class implementation                           *
 ***************************************************************************/


AbstractWeightsManager::AbstractWeightsManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AbstractWeightsManager::AbstractWeightsManager(
   Destribution * destribution,
   AbstractWeights * abstractWeights
   )
   : InterruptManager(
      ( abstractWeights != NULL ) ? abstractWeights->count() : 0,
      false,
      destribution
      )
   {
   this->abstractWeights = abstractWeights;

   // Capture object;
   if ( abstractWeights != NULL ) abstractWeights->capture();
   };


AbstractWeightsManager::~AbstractWeightsManager()
   {
   // Release captured object;
   if ( abstractWeights != NULL ) abstractWeights->release();
   };


void AbstractWeightsManager::handleInterrupt()
   {
   // Break up wire;
   int resistorIndex = this->getIntSource();
   if ( resistorIndex >= 0 && this->abstractWeights != NULL )
      {
      this->abstractWeights->setWeight( resistorIndex, 0.0 );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };
