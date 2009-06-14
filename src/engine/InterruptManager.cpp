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


#include "engine/InterruptManager.h"


/***************************************************************************
 *   InterruptManager abstract class implementation                        *
 ***************************************************************************/


InterruptManager::InterruptManager()
   {
   this->interruptsCount = 0;
   this->interrupts = NULL;
   this->numIntSources = 0;
   this->intSource = -1;
   this->unlimitRegeneration = false;
   this->destribution = NULL;
   };


InterruptManager::InterruptManager(
   int numIntSources,
   bool unlimitRegeneration,
   Destribution * destribution
   )
   {
   // Clear faults counter;
   this->interruptsCount = 0;

   // Initialize interrupts array with NULL;
   this->interrupts = NULL;

   // Try to allocate memory for interrupts;
   if ( numIntSources > 0 )
      {
      this->interrupts = new double[ numIntSources ];
      }

   if ( this->interrupts != NULL )
      {
      // Set number of interrupt sources;
      this->numIntSources = numIntSources;

      // Set regeneration limit;
      this->unlimitRegeneration = unlimitRegeneration;

      // Copy destribution;
      this->destribution = destribution;

      // Capture object;
      if ( destribution != NULL ) destribution->capture();

      // Generate interrupts;
      for ( int i = 0; i < numIntSources; i ++ )
         {
         this->interrupts[ i ] = this->destribution->generateTime();
         }

      // Find out interrupt source;
      this->findOutIntSource();
      }
   else
      {
      this->numIntSources = 0;
      this->intSource = -1;
      this->unlimitRegeneration = false;
      this->destribution = NULL;
      }
   };


InterruptManager::~InterruptManager()
   {
   // Delete interrupts array;
   if ( this->interrupts != NULL ) delete[] this->interrupts;

   // Release object;
   if ( this->destribution != NULL ) this->destribution->release();
   };


double InterruptManager::getInterrupt()
   {
   return ( ( this->intSource >= 0 ) ? this->interrupts[ this->intSource ] : -1.0 );
   };


unsigned int InterruptManager::getInterruptsCount()
   {
   return this->interruptsCount;
   };


void InterruptManager::handleInterrupt()
   {
   if ( this->intSource >= 0 )
      {
      if ( this->unlimitRegeneration )
         {
         // Generate new interrupt for current source;
         this->interrupts[ this->intSource ] = this->destribution->generateTime();
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


int InterruptManager::getIntSource()
   {
   return this->intSource;
   };


void InterruptManager::findOutIntSource()
   {
   double min = -1.0;
   this->intSource = -1;

   int i = 0;

   // Initialize minimum;
   while ( i < this->numIntSources )
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
   while ( i < this->numIntSources )
      {
      if ( this->interrupts[ i ] >= 0.0 && this->interrupts[ i ] < min )
         {
         min = this->interrupts[ i ];
         this->intSource = i;
         }

      i ++;
      }
   };
