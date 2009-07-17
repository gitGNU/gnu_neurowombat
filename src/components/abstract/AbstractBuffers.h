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


#ifndef ABSTRACTBUFFERS_H
#define ABSTRACTBUFFERS_H


#include "kernel/KernelObject.h"
#include "engine/InterruptManager.h"


/***************************************************************************
 *   AbstractBuffers class declaration                                     *
 ***************************************************************************/


class AbstractBuffers : public KernelObject
   {
   public:
      AbstractBuffers( unsigned int count = 0 );
      virtual ~AbstractBuffers();

      unsigned int count() const;

      void setBuffer( unsigned int index, double buffer );
      double getBuffer( unsigned int index ) const;

   private:
      unsigned int numBuffers;
      double * buffers;
   };


/***************************************************************************
 *   AbstractBuffersManager class declaration                              *
 ***************************************************************************/


class AbstractBuffersManager : public InterruptManager
   {
   public:
      AbstractBuffersManager();
      AbstractBuffersManager(
         Destribution * destribution,
         AbstractBuffers * abstractBuffers
         );

      virtual ~AbstractBuffersManager();

      virtual void handleInterrupt();

   private:
      AbstractBuffers * abstractBuffers;
   };


#endif
