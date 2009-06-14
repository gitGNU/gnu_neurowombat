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


#include "components/abstract/AbstractConnectors.h"


/***************************************************************************
 *   AbstractConnectors class implementation                               *
 ***************************************************************************/


AbstractConnectors::AbstractConnectors( unsigned int count )
   : KernelObject()
   {
   this->numWires = count;

   if ( count > 0 )
      {
      this->analogSignals = new double[ count ];
      this->broken = new bool[ count ];
      this->repareAll();
      }
   else
      {
      this->analogSignals = NULL;
      this->broken = NULL;
      }
   };


AbstractConnectors::~AbstractConnectors()
   {
   if ( this->analogSignals != NULL ) delete[] this->analogSignals;
   if ( this->broken != NULL ) delete[] this->broken;
   };


unsigned int AbstractConnectors::count() const
   {
   return this->numWires;
   };


void AbstractConnectors::copySignal( unsigned int destIndex, unsigned int srcIndex )
   {
   this->analogSignals[ destIndex ] = this->analogSignals[ srcIndex ];
   };


void AbstractConnectors::setSignal( unsigned int index, double signal )
   {
   this->analogSignals[ index ] = signal;
   };


double AbstractConnectors::getSignal( unsigned int index ) const
   {
   return ( broken[ index ] ) ? 0.0 : this->analogSignals[ index ];
   };


void AbstractConnectors::repareAll()
   {
   for ( unsigned int i = 0; i < numWires; i ++ )
      {
      this->broken[ i ] = false;
      }
   };


void AbstractConnectors::breakWire( unsigned int index )
   {
   this->broken[ index ] = true;
   };


void AbstractConnectors::repareWire( unsigned int index )
   {
   this->broken[ index ] = false;
   };


/***************************************************************************
 *   AbstractConnectorsManager class implementation                        *
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
