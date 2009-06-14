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


#include "kernel/KernelObject.h"


#include <stdio.h>
#include <typeinfo>


/***************************************************************************
 *   KernelObject abstract class implementation                            *
 ***************************************************************************/


KernelObject::KernelObject()
   {
   this->referenceCount = 0;
   };


KernelObject::~KernelObject()
   {
   // Do nothing;
   };


void KernelObject::capture()
   {
   this->referenceCount ++;
   };


void KernelObject::release()
   {
   this->referenceCount --;
   if ( this->referenceCount == 0 )
      {
      //char str[ 256 ];
      //sprintf( str, "Object %s has been deleted", typeid( * this ).name() );
      delete this;
      //printf( "%s\n", str );
      }
   };
