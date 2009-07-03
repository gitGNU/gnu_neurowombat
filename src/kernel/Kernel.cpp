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


#include "kernel/Kernel.h"


#include <cstdlib>
#include <time.h>


#include "api/api.h"


/***************************************************************************
 *   KernelObjectTable class implementation                                *
 ***************************************************************************/


KernelObjectTable::KernelObjectTable()
   {
   this->idSearchRequired = false;
   this->idCount = 0;
   this->currentId = MIN_ID;
   };


KernelObjectTable::~KernelObjectTable()
   {
   this->clear();
   };


KernelObjectId KernelObjectTable::createId( KernelObject * object )
   {
   if ( this->idCount >= MAX_ID - MIN_ID + 1 ||
      object == NULL
      ) return 0;

   if ( this->idSearchRequired )
      {
      while ( true )
         {
         if ( this->objects.find( this->currentId ) == this->objects.end() ) break;

         if ( this->currentId < MAX_ID )
            {
            this->currentId ++;
            }
         else
            {
            this->currentId = MIN_ID;
            }
         }
      }

   // Set id;
   KernelObjectId id = this->currentId;

   // Insert id and object pointer;
   this->objects.insert( std::make_pair( id, object ) );

   // Capture object;
   object->capture();

   // Increase current id;
   if ( this->currentId < MAX_ID )
      {
      this->currentId ++;
      }
   else
      {
      this->currentId = MIN_ID;
      this->idSearchRequired = true;
      }

   // Increase id counter;
   this->idCount ++;

   return id;
   };


void KernelObjectTable::closeId( KernelObjectId id )
   {
   std::map < KernelObjectId, KernelObject * >::iterator entryIterator =
      this->objects.find( id );

   if ( entryIterator == this->objects.end() ) return;

   // Set object;
   KernelObject * object = entryIterator->second;

   // Erase entry;
   this->objects.erase( entryIterator );

   // Release object;
   object->release();

   // Decrease id counter;
   this->idCount --;
   };


void KernelObjectTable::clear()
   {
   this->idSearchRequired = false;
   this->idCount = 0;
   this->currentId = MIN_ID;

   std::map < KernelObjectId, KernelObject * >::iterator entryIterator =
      this->objects.begin();

   // Release all the objects;
   while ( entryIterator != this->objects.end() )
      {
      entryIterator->second->release();
      entryIterator ++;
      }

   // Clear objects;
   this->objects.clear();
   };


KernelObject * KernelObjectTable::getObject( KernelObjectId id )
   {
   std::map < KernelObjectId, KernelObject * >::iterator entryIterator =
      this->objects.find( id );

   if ( entryIterator == this->objects.end() ) return NULL;

   return entryIterator->second;
   };


/***************************************************************************
 *   Kernel singleton class implementation                                 *
 ***************************************************************************/


KernelObjectId Kernel::insertObject( KernelObject * object )
   {
   // Try to create id;
   return this->objectTable.createId( object );
   };


void Kernel::deleteObject( KernelObjectId id )
   {
   // Try to close id;
   this->objectTable.closeId( id );
   };


KernelObject * Kernel::getObject( KernelObjectId id )
   {
   // Try to get object by it's id;
   return this->objectTable.getObject( id );
   };


void Kernel::doFile( const char * fileName )
   {
   if ( this->luaVM != NULL )
      {
      luaL_dofile( this->luaVM, fileName );
      }
   };


Kernel::Kernel()
   : Singleton < Kernel >::Singleton()
   {
   // Initialize random generator;
   srand( time( NULL ) );

   // Create lua virtual machine context;
   this->luaVM = luaL_newstate();

   // Initialize lua standard library functions;
   luaL_openlibs( this->luaVM );

   // Register API functions;
   registerApiFunctions( this->luaVM );
   };


Kernel::~Kernel()
   {
   // Close lua virtual machine context;
   lua_close( this->luaVM );
   };
