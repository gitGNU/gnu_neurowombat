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


#include "components/analog/AnalogWires.h"


/***************************************************************************
 *   AnalogWires class implementation                                      *
 ***************************************************************************/


AnalogWires::AnalogWires( unsigned int count )
   : KernelObject()
   {
   this->numWires = count;

   if ( count > 0 )
      {
      this->electricPotentials = new double[ count ];
      }
   else
      {
      this->electricPotentials = NULL;
      }
   };


AnalogWires::~AnalogWires()
   {
   if ( this->electricPotentials != NULL ) delete[] this->electricPotentials;
   };


unsigned int AnalogWires::count() const
   {
   return this->numWires;
   };


void AnalogWires::copyPotential( unsigned int destIndex, unsigned int srcIndex )
   {
   this->electricPotentials[ destIndex ] = this->electricPotentials[ srcIndex ];
   };


void AnalogWires::setPotential( unsigned int index, double potential )
   {
   this->electricPotentials[ index ] = potential;
   };


double AnalogWires::getPotential( unsigned int index ) const
   {
   return this->electricPotentials[ index ];
   };


/***************************************************************************
 *   AnalogWiresManager class implementation                               *
 ***************************************************************************/


/*AnalogWiresManager::AnalogWiresManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AnalogWiresManager::AnalogWiresManager(
   Destribution & destribution,
   AnalogWires * analogWires
   )
   : InterruptManager(
      ( analogWires != NULL ) ? analogWires->getNumWires() : 0,
      false,
      destribution,
      analogWires
      )
   {
   // Do nothing;
   };


AnalogWiresManager::~AnalogWiresManager()
   {
   // Do nothing;
   };


void AnalogWiresManager::handleInterrupt()
   {
   // Break up wire;
   int wireIndex = this->getIntSource();
   if ( wireIndex >= 0 && this->entity != NULL )
      {
      ( ( AnalogWires * ) this->entity )->breakWire( wireIndex );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };*/
