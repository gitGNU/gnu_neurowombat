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


#include "components/abstract/AbstractWeights.h"


/***************************************************************************
 *   AbstractWeights class implementation                                  *
 ***************************************************************************/


AbstractWeights::AbstractWeights( unsigned int count )
   : ComponentsSet < double >::ComponentsSet( count )
   {
   // Do nothing;
   };


AbstractWeights::~AbstractWeights()
   {
   // Do nothing;
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
   Distribution * distribution,
   AbstractWeights * abstractWeights,
   CustomFunction * fixFunction
   )
   : InterruptManager(
      ( abstractWeights != NULL ) ? abstractWeights->count() : 0,
      false,
      distribution
      )
   {
   this->abstractWeights = abstractWeights;

   // Capture object;
   if ( abstractWeights != NULL ) abstractWeights->capture();

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      backup = NULL;
      }
   else
      {
      // Create backup;
      unsigned int backupLength = abstractWeights->count();
      if ( backupLength > 0 )
         {
         backup = new double[ backupLength ];
         for ( unsigned int i = 0; i < backupLength; i ++ )
            {
            backup[ i ] = abstractWeights->at( i );
            }
         }
      else
         {
         backup = NULL;
         }
      }
   };


AbstractWeightsManager::~AbstractWeightsManager()
   {
   // Release captured object;
   if ( abstractWeights != NULL ) abstractWeights->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( backup != NULL ) delete[] backup;
   };


void AbstractWeightsManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && abstractWeights != NULL )
      {
      abstractWeights->at( intSource ) = 0.0;
      }
   };


void AbstractWeightsManager::handleInterrupt()
   {
   // Break up weight;
   int weightIndex = getIntSource();
   if ( weightIndex >= 0 && abstractWeights != NULL )
      {
      abstractWeights->at( weightIndex ) = 0.0;
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };


void AbstractWeightsManager::reinit()
   {
   // Call base implementation;
   InterruptManager::reinit();

   if ( fixFunction != NULL )
      {
      fixFunction->call();
      }
   else
      {
      // Restore weights from backup;
      for ( unsigned int i = 0; i < abstractWeights->count(); i ++ )
         {
         abstractWeights->at( i ) = backup[ i ];
         }
      }
   };
