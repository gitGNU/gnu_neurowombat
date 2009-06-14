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


#ifndef ABSTRACTNEURON_H
#define ABSTRACTNEURON_H


#include "exceptions.h"
#include "kernel/KernelObject.h"
#include "components/abstract/AbstractActivators.h"
#include "components/abstract/AbstractAdders.h"
#include "components/abstract/AbstractConnectors.h"
#include "components/abstract/AbstractWeights.h"


/***************************************************************************
 *   AbstractNeuronExcp class declaration                                  *
 ***************************************************************************/


namespace NS_ABSTRACTNEURON
   {
   enum EC
      {
      };
   };


class AbstractNeuronExcp : public Exception < NS_ABSTRACTNEURON::EC >
   {
   public:
      AbstractNeuronExcp( NS_ABSTRACTNEURON::EC error );
   };


/***************************************************************************
 *   AbstractNeuron class declaration                                      *
 ***************************************************************************/


class TestObject : public KernelObject
   {
   public:
      TestObject(
         AbstractActivators * activators = NULL,
         AbstractAdders * adders = NULL,
         AbstractConnectors * connectors = NULL,
         AbstractWeights * weights = NULL
         )
         {
         this->activators = activators;
         this->adders = adders;
         this->connectors = connectors;
         this->weights = weights;
         this->activators->capture();
         this->adders->capture();
         this->connectors->capture();
         this->weights->capture();
         };
      virtual ~TestObject()
         {
         this->activators->release();
         this->adders->release();
         this->connectors->release();
         this->weights->release();
         };

   private:
      AbstractActivators * activators;
      AbstractAdders * adders;
      AbstractConnectors * connectors;
      AbstractWeights * weights;
   };


class AbstractNeuron : public KernelObject
   {
   public:
      AbstractNeuron(
         unsigned int numInputs = 0,
         unsigned int * inputConnectors = NULL,
         AbstractActivators * activators = NULL,
         unsigned int activatorsBaseIndex = 0,
         AbstractAdders * adders = NULL,
         unsigned int addersBaseIndex = 0,
         AbstractConnectors * connectors = NULL,
         unsigned int connectorsBaseIndex = 0,
         AbstractWeights * weights = NULL,
         unsigned int weightsBaseIndex = 0
         );
      AbstractNeuron( const AbstractNeuron & other );
      virtual ~AbstractNeuron();
      virtual AbstractNeuron * clone();

      unsigned int getNumInputs() const;
      unsigned int getWeightsBaseIndex() const;

      void compute();
      /*void trainOnce(
         double * x,
         unsigned int length,
         double y,
         double * e,
         double speed
         );*/

   private:
      unsigned int numInputs;
      unsigned int * inputConnectors;

      AbstractActivators * activators;
      unsigned int activatorsBaseIndex;

      AbstractAdders * adders;
      unsigned int addersBaseIndex;

      AbstractConnectors * connectors;
      unsigned int connectorsBaseIndex;

      AbstractWeights * weights;
      unsigned int weightsBaseIndex;
   };


#endif
