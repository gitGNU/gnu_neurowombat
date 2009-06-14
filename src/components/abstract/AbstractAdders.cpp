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


#include "components/abstract/AbstractAdders.h"


/***************************************************************************
 *   AbstractAdders class implementation                                   *
 ***************************************************************************/


AbstractAdders::AbstractAdders( unsigned int count )
   : KernelObject()
   {
   this->numAdders = count;

   if ( count > 0 )
      {
      this->analogSignalSums = new double[ count ];
      }
   else
      {
      this->analogSignalSums = NULL;
      }
   };


AbstractAdders::~AbstractAdders()
   {
   if ( this->analogSignalSums != NULL ) delete[] this->analogSignalSums;
   };


unsigned int AbstractAdders::count() const
   {
   return this->numAdders;
   };


void AbstractAdders::resetSignalSum( unsigned int index, double signal )
   {
   this->analogSignalSums[ index ] = signal;
   };


void AbstractAdders::addToSignalSum( unsigned int index, double signal )
   {
   this->analogSignalSums[ index ] += signal;
   };


double AbstractAdders::getSignalSum( unsigned int index ) const
   {
   return this->analogSignalSums[ index ];
   };
