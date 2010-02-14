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


#ifndef ABSTRACTWEIGHTS_H
#define ABSTRACTWEIGHTS_H


#include "components/ComponentsSet.h"
#include "engine/InterruptManager.h"
#include "objects/CustomFunction.h"


/***************************************************************************
 *   AbstractWeights class declaration                                     *
 ***************************************************************************/


class AbstractWeights : public ComponentsSet < double >
   {
   public:
      AbstractWeights( unsigned int count = 0 );
      virtual ~AbstractWeights();
   };


/***************************************************************************
 *   AbstractWeightsManager class declaration                              *
 ***************************************************************************/


class AbstractWeightsManager : public InterruptManager
   {
   public:
      AbstractWeightsManager();
      AbstractWeightsManager(
         Distribution * distribution,
         AbstractWeights * abstractWeights,
         CustomFunction * fixFunction
         );

      virtual ~AbstractWeightsManager();

      virtual void simulateInterrupt( unsigned int intSource );
      virtual void handleInterrupt();
      virtual void reinit();

   private:
      AbstractWeights * abstractWeights;
      CustomFunction * fixFunction;
      double * backup;
   };


#endif
