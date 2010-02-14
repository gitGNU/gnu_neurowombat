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


#include "neurons/digital/DigitalNeuron.h"


#include <string.h>


/***************************************************************************
 *   DigitalNeuron class implementation                                    *
 ***************************************************************************/


DigitalNeuron::DigitalNeuron(
   unsigned int inputsCount,
   unsigned int * inputConnectors,
   DigitalConnectors * connectors,
   unsigned int connectorsBaseIndex,
   MemoryModule * memory,
   unsigned int memoryBaseIndex,
   ProcessingUnit * processingUnit,
   ActivationFunction * activationFunction
   )
   : KernelObject()
   {
   this->inputsCount = inputsCount;

   // Allocate memory for inputConnectors;
   if ( inputsCount > 0 && inputConnectors != NULL )
      {
      this->inputConnectors = new unsigned int[ inputsCount ];

      // Fill inputConnectors;
      if ( this->inputConnectors != NULL )
         {
         memcpy( this->inputConnectors, inputConnectors, inputsCount * sizeof( unsigned int ) );
         }
      }
   else
      {
      this->inputConnectors = NULL;
      }

   // Setup connectors;
   this->connectors = connectors;
   this->connectorsBaseIndex = connectorsBaseIndex;
   if ( connectors != NULL ) connectors->capture();

   // Setup memory;
   this->memory = memory;
   this->memoryBaseIndex = memoryBaseIndex;
   if ( memory != NULL ) memory->capture();

   // Setup built-in buffers;
   this->builtInBuffers = NULL;

   // Setup processingUnit;
   this->processingUnit = processingUnit;
   if ( processingUnit != NULL ) processingUnit->capture();

   // Setup activationFunction;
   this->activationFunction = activationFunction;
   if ( activationFunction != NULL ) activationFunction->capture();

   // Setup processingUnitOut;
   this->processingUnitOut = 0.0;

   // Setup delta;
   this->delta = 0.0;
   };


DigitalNeuron::~DigitalNeuron()
   {
   if ( this->inputConnectors != NULL ) delete[] this->inputConnectors;
   if ( this->builtInBuffers != NULL ) delete[] this->builtInBuffers;

   // Release captured objects;
   if ( connectors != NULL ) connectors->release();
   if ( memory != NULL ) memory->release();
   if ( activationFunction != NULL ) activationFunction->release();
   if ( processingUnit != NULL ) processingUnit->release();
   };


unsigned int DigitalNeuron::getInputsCount() const
   {
   return this->inputsCount;
   };


void DigitalNeuron::setWeight( unsigned int index, double weight )
   {
   this->memory->at( memoryBaseIndex + index ) = weight;
   };


double DigitalNeuron::getWeight( unsigned int index )
   {
   return this->memory->at( memoryBaseIndex + index );
   };


double DigitalNeuron::getOutput()
   {
   return connectors->at( connectorsBaseIndex );
   };


void DigitalNeuron::compute()
   {
   // Calculate processor out;
   processingUnitOut = processingUnit->process(
      inputsCount, inputConnectors, connectors,
      memory, memoryBaseIndex
      );

   connectors->at( connectorsBaseIndex ) = activationFunction->evaluateFunction( processingUnitOut );
   };


void DigitalNeuron::createDampingBuffers()
   {
   if ( builtInBuffers == NULL )
      {
      builtInBuffers = new double[ inputsCount ];
      for ( unsigned int i = 0; i < inputsCount; i ++ )
         {
         builtInBuffers[ i ] = 0.0;
         };
      };
   };


void DigitalNeuron::snapDelta( double err )
   {
   delta = err;
   delta *= activationFunction->evaluateDerivative( processingUnitOut );
   };


double DigitalNeuron::getDelta()
   {
   return delta;
   };


double DigitalNeuron::getWeightedDelta( unsigned int index )
   {
   return delta * memory->at( memoryBaseIndex + index );
   };


void DigitalNeuron::modifyWeights( double damping, double speed )
   {
   double d = delta * speed;
   double dw = 0;

   for ( unsigned int i = 0; i < inputsCount; i ++ )
      {
      dw = damping * builtInBuffers[ i ] +
         d * connectors->at( inputConnectors[ i ] );
      builtInBuffers[ i ] = dw;
      memory->at( memoryBaseIndex + i ) += dw;
      }
   };


DigitalNeuron::DigitalNeuron()
   : KernelObject()
   {
   // Do nothing;
   };


DigitalNeuron::DigitalNeuron( const DigitalNeuron & other )
   {
   // Do nothing;
   };
