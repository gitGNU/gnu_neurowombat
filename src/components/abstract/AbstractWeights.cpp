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


void AbstractWeights::setWeight( unsigned int index, double weight )
   {
   this->weights[ index ] = weight;
   };


void AbstractWeights::incWeight( unsigned int index, double weight )
   {
   this->weights[ index ] += weight;
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
   AbstractWeights * abstractWeights,
   CustomFunction * fixFunction
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

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      weightsBackup = NULL;
      }
   else
      {
      // Create backup for weights;
      unsigned int weightsBackupLength = abstractWeights->count();
      if ( weightsBackupLength > 0 )
         {
         weightsBackup = new double[ weightsBackupLength ];
         for ( unsigned int i = 0; i < weightsBackupLength; i ++ )
            {
            weightsBackup[ i ] = abstractWeights->getWeight( i );
            }
         }
      else
         {
         weightsBackup = NULL;
         }
      }
   };


AbstractWeightsManager::~AbstractWeightsManager()
   {
   // Release captured object;
   if ( abstractWeights != NULL ) abstractWeights->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( weightsBackup != NULL ) delete[] weightsBackup;
   };


void AbstractWeightsManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && abstractWeights != NULL )
      {
      abstractWeights->setWeight( intSource, 0.0 );
      }
   };


void AbstractWeightsManager::handleInterrupt()
   {
   // Break up weight;
   int weightIndex = getIntSource();
   if ( weightIndex >= 0 && abstractWeights != NULL )
      {
      abstractWeights->setWeight( weightIndex, 0.0 );
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
         abstractWeights->setWeight( i, weightsBackup[ i ] );
         }
      }
   };
