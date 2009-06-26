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


#ifndef ABSTRACTADDERS_H
#define ABSTRACTADDERS_H


#include "kernel/KernelObject.h"


/***************************************************************************
 *   AbstractAdders class declaration                                      *
 ***************************************************************************/


class AbstractAdders : public KernelObject
   {
   public:
      AbstractAdders( unsigned int count = 0 );
      ~AbstractAdders();

      unsigned int count() const;

      void resetSignalSum( unsigned int index, double signal = 0.0 );
      void addToSignalSum( unsigned int index, double signal );
      double getSignalSum( unsigned int index ) const;

   private:
      unsigned int numAdders;
      double * analogSignalSums;
   };


#endif
