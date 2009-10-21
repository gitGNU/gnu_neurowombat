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


#include <math.h>
#include <string.h>


#include "neurons/analog/AnalogNeuron.h"


/***************************************************************************
 *   AnalogNeuron class implementation                                     *
 ***************************************************************************/


AnalogNeuron::AnalogNeuron(
   unsigned int numInputs,
   unsigned int * inputWires,
   unsigned int gndWireIndex,
   unsigned int srcWireIndex,
   AnalogComparators * comparators,
   unsigned int comparatorsBaseIndex,
   AnalogResistors * resistors,
   unsigned int resistorsBaseIndex,
   AnalogWires * wires,
   unsigned int wiresBaseIndex
   )
   : KernelObject()
   {
   this->numInputs = numInputs;

   // Allocate memory for inputWireIndexis;
   if ( numInputs > 0 )
      {
      this->inputWires = new unsigned int[ numInputs ];

      // Fill inputWireIndexis;
      if ( inputWires != NULL )
         {
         memcpy( this->inputWires, inputWires, numInputs * sizeof( unsigned int ) );
         }
      else
         {
         for ( unsigned int i = 0; i < numInputs; i ++ )
            {
            this->inputWires[ i ] = gndWireIndex;
            }
         }
      }
   else
      {
      this->inputWires = NULL;
      }

   this->gndWireIndex = gndWireIndex;
   this->srcWireIndex = srcWireIndex;

   this->comparators = comparators;
   this->comparatorsBaseIndex = comparatorsBaseIndex;
   if ( comparators != NULL ) comparators->capture();

   this->resistors = resistors;
   this->resistorsBaseIndex = resistorsBaseIndex;
   if ( resistors != NULL ) resistors->capture();

   this->wires = wires;
   this->wiresBaseIndex = wiresBaseIndex;
   if ( wires != NULL ) wires->capture();
   }


AnalogNeuron::~AnalogNeuron()
   {
   if ( this->inputWires != NULL ) delete[] this->inputWires;

   // Release captured objects;
   if ( comparators != NULL ) comparators->release();
   if ( resistors != NULL ) resistors->release();
   if ( wires != NULL ) wires->release();
   };


AnalogNeuron * AnalogNeuron::clone()
   {
   return new AnalogNeuron( * this );
   };


unsigned int AnalogNeuron::getNumInputs() const
   {
   return this->numInputs;
   };


unsigned int AnalogNeuron::getResistorsBaseIndex() const
   {
   return this->resistorsBaseIndex;
   };


void AnalogNeuron::compute()
   {
   if ( this->comparators != NULL )
      {
      /*double net = 0.0;

      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double x = this->wires->getSignal( this->inputWireIndexis[ i ] );
         double w = this->resistors->getResistance( this->resistorsBaseIndex + i );
         net += x * w;
         }

      if ( net > 0 )
         {
         this->wires->setSignal( this->wiresBaseIndex, 1.0 );
         }
      else
         {
         this->wires->setSignal( this->wiresBaseIndex, -1.0 );
         }*/


      // Calculate positive and negative branch resistance products and sums;
      double posProduct = 1.0;
      double negProduct = 1.0;
      double posSum = 0.0;
      double negSum = 0.0;
      unsigned int numNonZeroPosResistances = 0;
      unsigned int numNonZeroNegResistances = 0;
      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double resistance = fabs( this->resistors->getResistance( this->resistorsBaseIndex + i ) );
         posSum += resistance;
         if ( resistance != 0.0 )
            {
            numNonZeroPosResistances ++;
            posProduct *= resistance;
            }

         resistance = fabs( this->resistors->getResistance( this->resistorsBaseIndex + numInputs + i ) );
         negSum += resistance;
         if ( resistance != 0.0 )
            {
            numNonZeroNegResistances ++;
            negProduct *= resistance;
            }
         }

      // Divide products over sums;
      posProduct /= posSum;
      negProduct /= negSum;

      // Calculate positive and negative potentials;
      double posPotential = 0.0;
      double negPotential = 0.0;

      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double inputVoltage = this->wires->getPotential( this->inputWires[ i ] );

         double resistance = this->resistors->getResistance( this->resistorsBaseIndex + i );
         if ( resistance != 0.0 )
            {
            if ( numNonZeroPosResistances > 1 )
               {
               posPotential += ( ( posProduct / resistance ) * inputVoltage );
               }
            else
               {
               posPotential = ( resistance > 0 ) ? inputVoltage : - inputVoltage;
               }
            }

         resistance = this->resistors->getResistance( this->resistorsBaseIndex + numInputs + i );
         if ( resistance != 0.0 )
            {
            if ( numNonZeroNegResistances > 1 )
               {
               negPotential -= ( ( negProduct / resistance ) * inputVoltage );
               }
            else
               {
               negPotential = ( resistance > 0 ) ? - inputVoltage : inputVoltage;
               }
            }
         }

      // Transfer positive and negative potentials through the comparator to obtain output voltage;
      this->wires->setPotential(
         this->wiresBaseIndex,
         this->comparators->compare( this->comparatorsBaseIndex, negPotential, posPotential )
         );
      }
   else
      {
      /*double net = 0.0;

      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double x = this->wires->getSignal( this->inputWireIndexis[ i ] );
         double w = this->resistors->getResistance( this->resistorsBaseIndex + i );
         net += x * w;
         }

      this->wires->setSignal( this->wiresBaseIndex, net );*/


      // Calculate resistance products and sums;
      double product = 1.0;
      double sum = 0.0;
      unsigned int numNonZeroResistances = 0;
      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double resistance = fabs( this->resistors->getResistance( this->resistorsBaseIndex + i ) );
         sum += resistance;
         if ( resistance != 0.0 )
            {
            numNonZeroResistances ++;
            product *= resistance;
            }
         }

      // Divide product over sum;
      product /= sum;

      // Calculate potential;
      double potential = 0.0;

      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double inputVoltage = this->wires->getPotential( this->inputWires[ i ] );

         double resistance = this->resistors->getResistance( this->resistorsBaseIndex + i );
         if ( resistance != 0.0 )
            {
            if ( numNonZeroResistances > 1 )
               {
               potential += ( ( product / resistance ) * inputVoltage );
               }
            else
               {
               potential = ( resistance > 0 ) ? inputVoltage : - inputVoltage;
               }
            }
         }

      // Transfer potential;
      this->wires->setPotential( this->wiresBaseIndex, potential );
      }
   };
