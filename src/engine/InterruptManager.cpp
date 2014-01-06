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


#include "engine/InterruptManager.h"


/***************************************************************************
 *   InterruptManager abstract class implementation                        *
 ***************************************************************************/


InterruptManager::InterruptManager(
   unsigned int intSourcesCount,
   bool unlimitedRegeneration,
   Distribution * distribution
   )
   {
   // Clear interrupts counter;
   this->interruptsCount = 0;

   // Initialize interrupts array with NULL;
   this->interrupts = NULL;

   // Try to allocate memory for interrupts;
   if ( intSourcesCount > 0 )
      {
      this->interrupts = new double[ intSourcesCount ];
      }

   if ( this->interrupts != NULL )
      {
      // Set number of interrupt sources;
      this->intSourcesCount = intSourcesCount;

      // Set regeneration limit;
      this->unlimitedRegeneration = unlimitedRegeneration;

      // Copy distribution;
      this->distribution = distribution;

      // Capture object;
      if ( distribution != NULL ) distribution->capture();

      // Generate interrupts;
      for ( unsigned int i = 0; i < intSourcesCount; i ++ )
         {
         this->interrupts[ i ] = this->distribution->generateTime();
         }

      // Clear int source;
      this->intSource = -1;

      // Find out interrupt source;
      this->findOutIntSource();
      }
   else
      {
      this->intSourcesCount = 0;
      this->intSource = -1;
      this->lastIntSource = -1;
      this->unlimitedRegeneration = false;
      this->distribution = NULL;
      }
   };


InterruptManager::~InterruptManager()
   {
   // Delete interrupts array;
   if ( this->interrupts != NULL ) delete[] this->interrupts;

   // Release captured object;
   if ( this->distribution != NULL ) this->distribution->release();
   };


double InterruptManager::getInterrupt()
   {
   return ( ( this->intSource >= 0 ) ? this->interrupts[ this->intSource ] : -1.0 );
   };


int InterruptManager::getLastIntSource()
   {
   return this->lastIntSource;
   };


int InterruptManager::getIntSource()
   {
   return this->intSource;
   };


unsigned int InterruptManager::getIntSourcesCount() const
   {
   return intSourcesCount;
   };


unsigned int InterruptManager::getInterruptsCount() const
   {
   return interruptsCount;
   };


void InterruptManager::handleInterrupt()
   {
   if ( this->intSource >= 0 )
      {
      if ( this->unlimitedRegeneration )
         {
         // Generate new interrupt for current source;
         this->interrupts[ this->intSource ] = this->distribution->generateTime();
         }
      else
         {
         // Mask current source;
         this->interrupts[ this->intSource ] = -1.0;
         }

      // Increment interrupts counter;
      this->interruptsCount ++;

      // Find out interrupt source;
      this->findOutIntSource();
      }
   };


void InterruptManager::reinit()
   {
   // Clear interrupts counter;
   this->interruptsCount = 0;

   // Generate interrupts;
   for ( unsigned int i = 0; i < intSourcesCount; i ++ )
      {
      this->interrupts[ i ] = this->distribution->generateTime();
      }

   // Clear int source;
   this->intSource = -1;

   // Find out interrupt source;
   this->findOutIntSource();
   };


InterruptManager::InterruptManager()
   {
   // Do nothing;
   };


void InterruptManager::findOutIntSource()
   {
   // Store last interrupt source;
   this->lastIntSource = this->intSource;

   double min = -1.0;
   this->intSource = -1;

   unsigned int i = 0;

   // Initialize minimum;
   while ( i < intSourcesCount )
      {
      if ( this->interrupts[ i ] >= 0.0 )
         {
         min = this->interrupts[ i ];
         this->intSource = i;
         break;
         }

      i ++;
      }

   // Search for minimum;
   while ( i < intSourcesCount )
      {
      if ( this->interrupts[ i ] >= 0.0 && this->interrupts[ i ] < min )
         {
         min = this->interrupts[ i ];
         this->intSource = i;
         }

      i ++;
      }
   };
