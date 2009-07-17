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


#include "components/abstract/AbstractBuffers.h"


#include <string.h>


AbstractBuffers::AbstractBuffers( unsigned int count )
   : KernelObject()
   {
   this->numBuffers = count;

   if ( count > 0 )
      {
      this->buffers = new double[ count ];
      }
   else
      {
      this->buffers = NULL;
      }
   };


AbstractBuffers::~AbstractBuffers()
   {
   if ( this->buffers != NULL ) delete[] this->buffers;
   };


unsigned int AbstractBuffers::count() const
   {
   return this->numBuffers;
   };


void AbstractBuffers::setBuffer( unsigned int index, double buffer )
   {
   this->buffers[ index ] = buffer;
   };


double AbstractBuffers::getBuffer( unsigned int index ) const
   {
   return this->buffers[ index ];
   };


/***************************************************************************
 *   AbstractBuffersManager class implementation                           *
 ***************************************************************************/


AbstractBuffersManager::AbstractBuffersManager()
   : InterruptManager()
   {
   // Do nothing;
   };


AbstractBuffersManager::AbstractBuffersManager(
   Destribution * destribution,
   AbstractBuffers * abstractBuffers
   )
   : InterruptManager(
      ( abstractBuffers != NULL ) ? abstractBuffers->count() : 0,
      false,
      destribution
      )
   {
   this->abstractBuffers = abstractBuffers;

   // Capture object;
   if ( abstractBuffers != NULL ) abstractBuffers->capture();
   };


AbstractBuffersManager::~AbstractBuffersManager()
   {
   // Release captured object;
   if ( abstractBuffers != NULL ) abstractBuffers->release();
   };


void AbstractBuffersManager::handleInterrupt()
   {
   // Break up buffer;
   int resistorIndex = this->getIntSource();
   if ( resistorIndex >= 0 && this->abstractBuffers != NULL )
      {
      this->abstractBuffers->setBuffer( resistorIndex, 0.0 );
      }

   // Pass control to base implementation;
   InterruptManager::handleInterrupt();
   };
