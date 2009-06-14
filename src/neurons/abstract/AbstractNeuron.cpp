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


#include <string.h>


#include "neurons/abstract/AbstractNeuron.h"


/***************************************************************************
 *   AbstractNeuronExcp class implementation                               *
 ***************************************************************************/


AbstractNeuronExcp::AbstractNeuronExcp( NS_ABSTRACTNEURON::EC error )
   : Exception < NS_ABSTRACTNEURON::EC > ( error )
   {
   // Do nothing;
   };


/***************************************************************************
 *   AbstractNeuron class implementation                                   *
 ***************************************************************************/


AbstractNeuron::AbstractNeuron(
   unsigned int numInputs,
   unsigned int * inputConnectors,
   AbstractActivators * activators,
   unsigned int activatorsBaseIndex,
   AbstractAdders * adders,
   unsigned int addersBaseIndex,
   AbstractConnectors * connectors,
   unsigned int connectorsBaseIndex,
   AbstractWeights * weights,
   unsigned int weightsBaseIndex
   )
   : KernelObject()
   {
   this->numInputs = numInputs;

   // Allocate memory for inputConnectors;
   if ( numInputs > 0 && inputConnectors != NULL )
      {
      this->inputConnectors = new unsigned int[ numInputs ];

      // Fill inputConnectors;
      if ( this->inputConnectors != NULL )
         {
         memcpy( this->inputConnectors, inputConnectors, numInputs * sizeof( unsigned int ) );
         }
      }
   else
      {
      this->inputConnectors = NULL;
      }

   this->activators = activators;
   this->activatorsBaseIndex = activatorsBaseIndex;

   this->adders = adders;
   this->addersBaseIndex = addersBaseIndex;

   this->connectors = connectors;
   this->connectorsBaseIndex = connectorsBaseIndex;

   this->weights = weights;
   this->weightsBaseIndex = weightsBaseIndex;
   }


AbstractNeuron::AbstractNeuron( const AbstractNeuron & other )
   {
   this->numInputs = other.numInputs;

   if ( other.inputConnectors != NULL )
      {
      this->inputConnectors = new unsigned int[ this->numInputs ];

      if ( this->inputConnectors != NULL )
         {
         memcpy( this->inputConnectors, other.inputConnectors, this->numInputs * sizeof( unsigned int ) );
         }
      }
   else
      {
      this->inputConnectors = NULL;
      }

   this->activators = other.activators;
   this->activatorsBaseIndex = other.activatorsBaseIndex;

   this->adders = other.adders;
   this->addersBaseIndex = other.addersBaseIndex;

   this->connectors = other.connectors;
   this->connectorsBaseIndex = other.connectorsBaseIndex;

   this->weights = other.weights;
   this->weightsBaseIndex = other.weightsBaseIndex;
   };


AbstractNeuron::~AbstractNeuron()
   {
   if ( this->inputConnectors != NULL ) delete[] this->inputConnectors;
   };


AbstractNeuron * AbstractNeuron::clone()
   {
   return new AbstractNeuron( * this );
   };


unsigned int AbstractNeuron::getNumInputs() const
   {
   return this->numInputs;
   };


unsigned int AbstractNeuron::getWeightsBaseIndex() const
   {
   return this->weightsBaseIndex;
   };


void AbstractNeuron::compute()
   {
   /*// Transfer signal from input wires to memistors;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      this->memistors->setSignal(
         this->memistorsBaseIndex + i,
         this->wires->getSignal( this->inputWireIndexis[ i ] )
         );
      }

   // Transfer signal from src wire to T memistor;
   this->memistors->setSignal(
      this->memistorsBaseIndex + numInputs,
      this->wires->getSignal( this->srcWireIndex )
      );

   // Reset net with T memistor signal;
   this->adders->resetSignalSum(
      this->adderIndex,
      this->memistors->getSignal( this->memistorsBaseIndex + numInputs )
      );

   // Add signals from memistors to net;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      this->adders->addToSignalSum(
         this->adderIndex,
         this->memistors->getSignal( this->memistorsBaseIndex + i )
         );
      }

   // Transfer net to middle wire;
   this->wires->setSignal(
      this->wiresBaseIndex,
      this->adders->getSignalSum( this->adderIndex )
      );

   // Transfer middle ware signal to non linear element;
   this->nonLinearElements->setSignal(
      this->nonLinearElementIndex,
      this->wires->getSignal( this->wiresBaseIndex )
      );

   // Transfer signal to output wire;
   this->wires->setSignal(
      this->wiresBaseIndex + 1,
      this->nonLinearElements->getSignal( this->nonLinearElementIndex )
      );*/
   };


/*void PseudoAnalogNeuron::trainOnce(
   double * x,
   unsigned int length,
   double y,
   double * e,
   double speed
   )
   {
   * e = y - this->compute( x, length );

   // Net value will be loaded from buffer which stores it till the last forvard propagation;
   this->delta = ( * e );
   double d = speed * this->delta;
   if ( ! this->damping )
      {
      for ( unsigned int i = 0; i < this->numInputs; i ++ )
         {
         this->wbuffer[ i ] = this->w[ i ];
         this->w[ i ] += x[ i ] * d;
         }

      this->t += d;
      }
   };*/
