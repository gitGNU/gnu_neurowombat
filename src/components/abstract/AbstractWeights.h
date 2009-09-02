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


#ifndef ABSTRACTWEIGHTS_H
#define ABSTRACTWEIGHTS_H


#include "kernel/KernelObject.h"
#include "engine/InterruptManager.h"


/***************************************************************************
 *   AbstractWeights class declaration                                     *
 ***************************************************************************/


class AbstractWeights : public KernelObject
   {
   public:
      AbstractWeights( unsigned int count = 0 );
      virtual ~AbstractWeights();

      unsigned int count() const;

      void setWeight( unsigned int index, double weight );
      void incWeight( unsigned int index, double weight );
      double getWeight( unsigned int index ) const;

   private:
      unsigned int numWeights;
      double * weights;
   };


/***************************************************************************
 *   AbstractWeightsManager class declaration                              *
 ***************************************************************************/


class AbstractWeightsManager : public InterruptManager
   {
   public:
      AbstractWeightsManager();
      AbstractWeightsManager(
         Destribution * destribution,
         AbstractWeights * abstractWeights
         );

      virtual ~AbstractWeightsManager();

      virtual void handleInterrupt();

   private:
      AbstractWeights * abstractWeights;
   };


#endif
