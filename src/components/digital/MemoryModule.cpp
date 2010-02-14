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


#include "components/digital/MemoryModule.h"


/***************************************************************************
 *   MemoryModule class implementation                                     *
 ***************************************************************************/


MemoryModule::MemoryModule( unsigned int count )
   : ComponentsSet < double >::ComponentsSet( count )
   {
   // Do nothing;
   };


MemoryModule::~MemoryModule()
   {
   // Do nothing;
   };


/***************************************************************************
 *   MemoryModuleManager class implementation                              *
 ***************************************************************************/


MemoryModuleManager::MemoryModuleManager()
   : InterruptManager()
   {
   // Do nothing;
   };


MemoryModuleManager::MemoryModuleManager(
   Distribution * distribution,
   MemoryModule * memoryModule,
   CustomFunction * fixFunction
   )
   : InterruptManager(
      ( memoryModule != NULL ) ? memoryModule->count() * 64 : 0,
      false,
      distribution
      )
   {
   this->memoryModule = memoryModule;

   // Capture object;
   if ( memoryModule != NULL ) memoryModule->capture();

   this->fixFunction = fixFunction;

   if ( fixFunction != NULL )
      {
      fixFunction->capture();
      backup = NULL;
      }
   else
      {
      // Create backup;
      unsigned backupLength = memoryModule->count();
      if ( backupLength > 0 )
         {
         backup = new double[ backupLength ];
         for ( unsigned int i = 0; i < backupLength; i ++ )
            {
            backup[ i ] = memoryModule->at( i );
            }
         }
      else
         {
         backup = NULL;
         }
      }
   };


MemoryModuleManager::~MemoryModuleManager()
   {
   // Release captured object;
   if ( memoryModule != NULL ) memoryModule->release();
   if ( fixFunction != NULL ) fixFunction->release();

   if ( backup != NULL ) delete[] backup;
   };


void MemoryModuleManager::simulateInterrupt( unsigned int intSource )
   {
   if ( intSource < intSourcesCount && memoryModule != NULL )
      {
      unsigned int wordIndex = intSource / 64;
      unsigned int bitInWordIndex = intSource % 64;
      unsigned char mask = ~ ( 0x01 << ( 7 - bitInWordIndex % 8 ) );
      double word = memoryModule->at( wordIndex );
      ( ( unsigned char * ) & word )[ bitInWordIndex / 8 ] &= mask;
      memoryModule->at( wordIndex ) = word;
      }
   };


void MemoryModuleManager::handleInterrupt()
   {
   // Break up bit;
   int bitIndex = getIntSource();
   if ( bitIndex >= 0 && memoryModule != NULL )
      {
      unsigned int wordIndex = bitIndex / 64;
      unsigned int bitInWordIndex = bitIndex % 64;
      unsigned char mask = ~ ( 0x01 << ( 7 - bitInWordIndex % 8 ) );
      double word = memoryModule->at( wordIndex );
      ( ( unsigned char * ) & word )[ bitInWordIndex / 8 ] &= mask;
      memoryModule->at( wordIndex ) = word;
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };


void MemoryModuleManager::reinit()
   {
   // Call base implementation;
   InterruptManager::reinit();

   if ( fixFunction != NULL )
      {
      fixFunction->call();
      }
   else
      {
      // Restore words from backup;
      for ( unsigned int i = 0; i < memoryModule->count(); i ++ )
         {
         memoryModule->at( i ) = backup[ i ];
         }
      }
   };
