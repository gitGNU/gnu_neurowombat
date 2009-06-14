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


#include "components/analog/AnalogComparators.h"


/***************************************************************************
 *   AnalogComparators class implementation                                *
 ***************************************************************************/


AnalogComparators::AnalogComparators( unsigned int count )
   : KernelObject()
   {
   this->numComparators = count;

   if ( count > 0 )
      {
      this->analogSignals = new double[ count ];
      }
   else
      {
      this->analogSignals = NULL;
      }
   };


AnalogComparators::~AnalogComparators()
   {
   if ( this->analogSignals != NULL ) delete[] this->analogSignals;
   };


unsigned int AnalogComparators::count() const
   {
   return this->numComparators;
   };


double AnalogComparators::compare( unsigned int index, double neg, double pos )
   {
   if ( pos - neg > 0 )
      {
      return 1.0;
      }
   else
      {
      return -1.0;
      }
   };
