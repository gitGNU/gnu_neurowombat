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


/*AnalogResistorsManager::AnalogResistorsManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AnalogResistorsManager::AnalogResistorsManager(
   Destribution & destribution,
   AnalogResistors * analogResistors
   )
   : InterruptManager(
      ( analogResistors != NULL ) ? analogResistors->count() : 0,
      false,
      destribution,
      analogResistors
      )
   {
   // Do nothing;
   };


AnalogResistorsManager::~AnalogResistorsManager()
   {
   // Do nothing;
   };


void AnalogResistorsManager::handleInterrupt()
   {
   // Break up wire;
   int resistorIndex = this->getIntSource();
   if ( resistorIndex >= 0 && this->entity != NULL )
      {
      ( ( AnalogResistors * ) this->entity )->setResistance( resistorIndex, 0.0 );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };*/