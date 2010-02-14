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
   AnalogCapacitors * capacitors,
   unsigned int capacitorsBaseIndex,
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

   this->capacitors = capacitors;
   this->capacitorsBaseIndex = capacitorsBaseIndex;
   if ( capacitors != NULL ) capacitors->capture();

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
   if ( capacitors != NULL ) capacitors->release();
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


double AnalogNeuron::getPosConstant()
   {
   // Calculate positive branch resistances products;
   double posProduct = 1.0;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      double resistance = this->resistors->at( this->resistorsBaseIndex + i );
      if ( resistance != 0.0 ) posProduct *= resistance;
      }

   posProduct = fabs( posProduct );

   // Calculate positive constant;
   double posConstant = 0.0;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      double resistance = this->resistors->at( this->resistorsBaseIndex + i );
      if ( resistance != 0.0 ) posConstant += posProduct / fabs( resistance );
      }

   if ( posConstant != 0.0 ) posConstant =
      posProduct * capacitors->at( capacitorsBaseIndex ) / posConstant;

   return posConstant;
   };


double AnalogNeuron::getNegConstant()
   {
   // Calculate negative branch resistances products;
   double negProduct = 1.0;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      double resistance = this->resistors->at( this->resistorsBaseIndex + numInputs + i );
      if ( resistance != 0.0 ) negProduct *= resistance;
      }

   negProduct = fabs( negProduct );

   // Calculate negative constant;
   double negConstant = 0.0;
   for ( unsigned int i = 0; i < numInputs; i ++ )
      {
      double resistance = this->resistors->at( this->resistorsBaseIndex + numInputs + i );
      if ( resistance != 0.0 ) negConstant += negProduct / fabs( resistance );
      }

   if ( negConstant != 0.0 ) negConstant =
      negProduct * capacitors->at( capacitorsBaseIndex + 1 ) / negConstant;

   return negConstant;
   };


double AnalogNeuron::leftComputePos()
   {
   return calcPotencial( capacitorsBaseIndex, resistorsBaseIndex );
   };


double AnalogNeuron::leftComputeNeg()
   {
   return - calcPotencial( capacitorsBaseIndex + 1, resistorsBaseIndex + numInputs );
   };


void AnalogNeuron::rightCompute( double negPotential, double posPotential )
   {
   wires->at( wiresBaseIndex ) = comparators->compare( comparatorsBaseIndex, negPotential, posPotential );
   };


void AnalogNeuron::compute()
   {
   if ( comparators != NULL )
      {
      // Calculate potentials;
      double posPotential = calcPotencial( capacitorsBaseIndex, resistorsBaseIndex );
      double negPotential = - calcPotencial( capacitorsBaseIndex + 1, resistorsBaseIndex + numInputs );

      // Transfer positive and negative potentials through the comparator to obtain output voltage;
      wires->at( wiresBaseIndex ) = comparators->compare( comparatorsBaseIndex, negPotential, posPotential );
      }
   else
      {
      // Calculate potential;
      double potential = calcPotencial( capacitorsBaseIndex, resistorsBaseIndex );

      // Transfer potential;
      wires->at( wiresBaseIndex ) = potential;
      }
   };


double AnalogNeuron::calcPotencial( unsigned int capacitorsBaseIndex, unsigned int resistorsBaseIndex )
   {
   double potential = wires->at( gndWireIndex );
   if ( capacitors->at( capacitorsBaseIndex ) != 0.0 )
      {
      // Calculate resistances product;
      double product = 1.0;
      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double resistance = resistors->at( resistorsBaseIndex + i );
         if ( resistance != 0.0 ) product *= resistance;
         }

      product = fabs( product );

      // Calculate potential;
      double sum = 0.0;
      for ( unsigned int i = 0; i < numInputs; i ++ )
         {
         double resistance = resistors->at( resistorsBaseIndex + i );
         if ( resistance != 0.0 )
            {
            double multiplyer = product / resistance;
            potential += multiplyer * wires->at( inputWires[ i ] );
            sum += fabs( multiplyer );
            }
         }

      // Divide potential over sum;
      if ( sum != 0.0 ) potential /= sum;
      }

   return potential;
   };
