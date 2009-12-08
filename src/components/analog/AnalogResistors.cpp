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
   AnalogResistors * analogResistors,
   CustomFunction * fixFunction
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

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      resistancesBackup = NULL;
      }
   else
      {
      // Create backup for resistances;
      unsigned int resistancesBackupLength = analogResistors->count();
      if ( resistancesBackupLength > 0 )
         {
         resistancesBackup = new double[ resistancesBackupLength ];
         for ( unsigned int i = 0; i < resistancesBackupLength; i ++ )
            {
            resistancesBackup[ i ] = analogResistors->getResistance( i );
            }
         }
      else
         {
         resistancesBackup = NULL;
         }
      }
   };


AnalogResistorsManager::~AnalogResistorsManager()
   {
   // Release captured object;
   if ( analogResistors != NULL ) analogResistors->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( resistancesBackup != NULL ) delete[] resistancesBackup;
   };


void AnalogResistorsManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && analogResistors != NULL )
      {
      analogResistors->setResistance( intSource, 0.0 );
      }
   };


void AnalogResistorsManager::handleInterrupt()
   {
   // Break up wire;
   int resistorIndex = getIntSource();
   if ( resistorIndex >= 0 && analogResistors != NULL )
      {
      analogResistors->setResistance( resistorIndex, 0.0 );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };


void AnalogResistorsManager::reinit()
   {
   // Call base implementation;
   InterruptManager::reinit();

   if ( fixFunction != NULL )
      {
      fixFunction->call();
      }
   else
      {
      // Restore resistances from backup;
      for ( unsigned int i = 0; i < analogResistors->count(); i ++ )
         {
         analogResistors->setResistance( i, resistancesBackup[ i ] );
         }
      }
   };
