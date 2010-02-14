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


#include "components/analog/AnalogCapacitors.h"


/***************************************************************************
 *   AnalogCapacitors class implementation                                 *
 ***************************************************************************/


AnalogCapacitors::AnalogCapacitors( unsigned int count )
   : ComponentsSet < double >::ComponentsSet( count )
   {
   // Do nothing;
   };


AnalogCapacitors::~AnalogCapacitors()
   {
   // Do nothing;
   };


/***************************************************************************
 *   AnalogCapacitorsManager class implementation                          *
 ***************************************************************************/


AnalogCapacitorsManager::AnalogCapacitorsManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AnalogCapacitorsManager::AnalogCapacitorsManager(
   Distribution * distribution,
   AnalogCapacitors * analogCapacitors,
   CustomFunction * fixFunction
   )
   : InterruptManager(
      ( analogCapacitors != NULL ) ? analogCapacitors->count() : 0,
      false,
      distribution
      )
   {
   this->analogCapacitors = analogCapacitors;

   // Capture object;
   if ( analogCapacitors != NULL ) analogCapacitors->capture();

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      backup = NULL;
      }
   else
      {
      // Create backup;
      unsigned int backupLength = analogCapacitors->count();
      if ( backupLength > 0 )
         {
         backup = new double[ backupLength ];
         for ( unsigned int i = 0; i < backupLength; i ++ )
            {
            backup[ i ] = analogCapacitors->at( i );
            }
         }
      else
         {
         backup = NULL;
         }
      }
   };


AnalogCapacitorsManager::~AnalogCapacitorsManager()
   {
   // Release captured object;
   if ( analogCapacitors != NULL ) analogCapacitors->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( backup != NULL ) delete[] backup;
   };


void AnalogCapacitorsManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && analogCapacitors != NULL )
      {
      analogCapacitors->at( intSource ) = 0.0;
      }
   };


void AnalogCapacitorsManager::handleInterrupt()
   {
   // Break up capacitor;
   int capacitorIndex = getIntSource();
   if ( capacitorIndex >= 0 && analogCapacitors != NULL )
      {
      analogCapacitors->at( capacitorIndex ) = 0.0;
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };


void AnalogCapacitorsManager::reinit()
   {
   // Call base implementation;
   InterruptManager::reinit();

   if ( fixFunction != NULL )
      {
      fixFunction->call();
      }
   else
      {
      // Restore capacitances from backup;
      for ( unsigned int i = 0; i < analogCapacitors->count(); i ++ )
         {
         analogCapacitors->at( i ) = backup[ i ];
         }
      }
   };
