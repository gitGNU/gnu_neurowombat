/***************************************************************************
 *   Copyright (C) 2009, 2010 Andrew Timashov                              *
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


/***************************************************************************
 *   AnalogResistors class implementation                                  *
 ***************************************************************************/


AnalogResistors::AnalogResistors( unsigned int count )
   : ComponentsSet < double >::ComponentsSet( count )
   {
   // Do nothing;
   };


AnalogResistors::~AnalogResistors()
   {
   // Do nothing;
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
   Distribution * distribution,
   AnalogResistors * analogResistors,
   CustomFunction * fixFunction
   )
   : InterruptManager(
      ( analogResistors != NULL ) ? analogResistors->count() : 0,
      false,
      distribution
      )
   {
   this->analogResistors = analogResistors;

   // Capture object;
   if ( analogResistors != NULL ) analogResistors->capture();

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      backup = NULL;
      }
   else
      {
      // Create backup;
      unsigned int backupLength = analogResistors->count();
      if ( backupLength > 0 )
         {
         backup = new double[ backupLength ];
         for ( unsigned int i = 0; i < backupLength; i ++ )
            {
            backup[ i ] = analogResistors->at( i );
            }
         }
      else
         {
         backup = NULL;
         }
      }
   };


AnalogResistorsManager::~AnalogResistorsManager()
   {
   // Release captured object;
   if ( analogResistors != NULL ) analogResistors->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( backup != NULL ) delete[] backup;
   };


void AnalogResistorsManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && analogResistors != NULL )
      {
      analogResistors->at( intSource ) = 0.0;
      }
   };


void AnalogResistorsManager::handleInterrupt()
   {
   // Break up resistor;
   int resistorIndex = getIntSource();
   if ( resistorIndex >= 0 && analogResistors != NULL )
      {
      analogResistors->at( resistorIndex ) = 0.0;
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
         analogResistors->at( i ) = backup[ i ];
         }
      }
   };
