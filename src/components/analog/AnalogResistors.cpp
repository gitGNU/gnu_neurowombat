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


#include "components/analog/AnalogResistors.h"


AnalogResistors::AnalogResistors( unsigned int count )
   : KernelObject()
   {
   this->numResistors = count;

   if ( count > 0 )
      {
      this->resistances = new double[ count ];
      }
   else
      {
      this->resistances = NULL;
      }
   };


AnalogResistors::~AnalogResistors()
   {
   if ( this->resistances != NULL ) delete[] this->resistances;
   };


unsigned int AnalogResistors::count() const
   {
   return this->numResistors;
   };


void AnalogResistors::setResistance( unsigned int index, double resistance )
   {
   this->resistances[ index ] = resistance;
   };


double AnalogResistors::getResistance( unsigned int index ) const
   {
   return this->resistances[ index ];
   };


/***************************************************************************
 *   AnalogResistorsManager class implementation                           *
 ***************************************************************************/


AnalogResistorsManager::AnalogResistorsManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AnalogResistorsManager::AnalogResistorsManager(
   Destribution * destribution,
   AnalogResistors * analogResistors
   )
   : InterruptManager(
      ( analogResistors != NULL ) ? analogResistors->count() : 0,
      false,
      destribution
      )
   {
   this->analogResistors = analogResistors;

   // Capture object;
   if ( analogResistors != NULL ) analogResistors->capture();
   };


AnalogResistorsManager::~AnalogResistorsManager()
   {
   // Release captured object;
   if ( analogResistors != NULL ) analogResistors->release();
   };


void AnalogResistorsManager::handleInterrupt()
   {
   // Break up wire;
   int resistorIndex = this->getIntSource();
   if ( resistorIndex >= 0 && this->analogResistors != NULL )
      {
      this->analogResistors->setResistance( resistorIndex, 0.0 );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };
