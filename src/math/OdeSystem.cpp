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


#include "math/OdeSystem.h"


/***************************************************************************
 *   OdeSystem abstract class implementation                               *
 ***************************************************************************/


OdeSystem::OdeSystem( unsigned int equationsCount )
   {
   this->equationsCount = equationsCount;

   if ( equationsCount > 0 )
      {
      depVars = new double[ equationsCount ];
      }
   else
      {
      depVars = NULL;
      }
   };


OdeSystem::~OdeSystem()
   {
   if ( depVars != NULL ) delete[] depVars;
   };


unsigned int OdeSystem::getEquationsCount()
   {
   return equationsCount;
   };


void OdeSystem::setIndependentVariable( double x )
   {
   indepVar = x;
   };


double OdeSystem::getIndependentVariable()
   {
   return indepVar;
   };


void OdeSystem::incIndependentVariable( double x )
   {
   indepVar += x;
   };


void OdeSystem::setDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] = x;
   };


double OdeSystem::getDependentVariable( unsigned int index )
   {
   return depVars[ index ];
   };


void OdeSystem::incDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] += x;
   };


void OdeSystem::incDependentVariables( double * x1, double * x2, double k1, double k2 )
   {
   for ( unsigned int i = 0; i < equationsCount; i ++ )
      {
      depVars[ i ] += k1 * x1[ i ] + k2 * x2[ i ];
      }
   };


OdeSystem::OdeSystem()
   {
   // Do nothing;
   };


OdeSystem::OdeSystem( OdeSystem & other )
   {
   // Do nothing;
   };


/***************************************************************************
 *   AbstractNeuronsOdeSystem class implementation                         *
 ***************************************************************************/


AbstractNeuronsOdeSystem::AbstractNeuronsOdeSystem(
   std::vector < AbstractNeuron * > * neurons,
   double timeConstant
   )
   : OdeSystem( ( neurons != NULL ) ? neurons->size() : 0 )
   {
   this->neurons = neurons;
   this->timeConstant = timeConstant;

   // Initialize independent variable;
   indepVar = 0.0;

   // Initialize dependent variables;
   for ( unsigned int i = 0; i < equationsCount; i ++ )
      {
      depVars[ i ] = neurons->at( i )->leftCompute();
      }
   };


AbstractNeuronsOdeSystem::~AbstractNeuronsOdeSystem()
   {
   // Do nothing;
   };


double AbstractNeuronsOdeSystem::evaluateOdeFunction( unsigned int index )
   {
   return ( - depVars[ index ] / timeConstant ) + neurons->at( index )->leftCompute();
   };


void AbstractNeuronsOdeSystem::setDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] = x;
   neurons->at( index )->rightCompute( depVars[ index ] );
   };


void AbstractNeuronsOdeSystem::incDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] += x;
   neurons->at( index )->rightCompute( depVars[ index ] );
   };


void AbstractNeuronsOdeSystem::incDependentVariables( double * x1, double * x2, double k1, double k2 )
   {
   for ( unsigned int i = 0; i < equationsCount; i ++ )
      {
      depVars[ i ] += k1 * x1[ i ] + k2 * x2[ i ];
      neurons->at( i )->rightCompute( depVars[ i ] );
      }
   };


AbstractNeuronsOdeSystem::AbstractNeuronsOdeSystem()
   : OdeSystem()
   {
   // Do nothing;
   };


AbstractNeuronsOdeSystem::AbstractNeuronsOdeSystem( AbstractNeuronsOdeSystem & other )
   : OdeSystem( other )
   {
   // Do nothing;
   };


/***************************************************************************
 *   AnalogLimNeuronsOdeSystem class implementation                        *
 ***************************************************************************/


AnalogLimNeuronsOdeSystem::AnalogLimNeuronsOdeSystem(
   std::vector < AnalogNeuron * > * neurons
   )
   : OdeSystem( ( neurons != NULL ) ? 2 * neurons->size() : 0 )
   {
   this->neurons = neurons;

   // Initialize independent variable;
   indepVar = 0.0;

   // Initialize dependent variables;
   for ( unsigned int i = 0; i < neurons->size(); i ++ )
      {
      depVars[ i ] = neurons->at( i )->leftComputePos();
      depVars[ i + neurons->size() ] = neurons->at( i )->leftComputeNeg();
      }
   };


AnalogLimNeuronsOdeSystem::~AnalogLimNeuronsOdeSystem()
   {
   // Do nothing;
   };


double AnalogLimNeuronsOdeSystem::evaluateOdeFunction( unsigned int index )
   {
   double result = 0.0;
   if ( index < neurons->size() )
      {
      result = neurons->at( index )->getPosConstant();
      if ( result != 0.0 ) result = ( - depVars[ index ] + neurons->at( index )->leftComputePos() ) / result;
      }
   else
      {
      result = neurons->at( index - neurons->size() )->getNegConstant();
      if ( result != 0.0 ) result = ( - depVars[ index ] + neurons->at( index - neurons->size() )->leftComputeNeg() ) / result;
      }

   return result;
   };


void AnalogLimNeuronsOdeSystem::setDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] = x;

   if ( index < neurons->size() )
      {
      neurons->at( index )->rightCompute( depVars[ index + neurons->size() ], depVars[ index ] );
      }
   else
      {
      neurons->at( index - neurons->size() )->rightCompute( depVars[ index ], depVars[ index - neurons->size() ] );
      }
   };


void AnalogLimNeuronsOdeSystem::incDependentVariable( unsigned int index, double x )
   {
   depVars[ index ] += x;

   if ( index < neurons->size() )
      {
      neurons->at( index )->rightCompute( depVars[ index + neurons->size() ], depVars[ index ] );
      }
   else
      {
      neurons->at( index - neurons->size() )->rightCompute( depVars[ index ], depVars[ index - neurons->size() ] );
      }
   };


void AnalogLimNeuronsOdeSystem::incDependentVariables( double * x1, double * x2, double k1, double k2 )
   {
   for ( unsigned int i = 0; i < equationsCount; i ++ )
      {
      depVars[ i ] += k1 * x1[ i ] + k2 * x2[ i ];
      if ( i < neurons->size() )
         {
         neurons->at( i )->rightCompute( depVars[ i + neurons->size() ], depVars[ i ] );
         }
      else
         {
         neurons->at( i - neurons->size() )->rightCompute( depVars[ i ], depVars[ i - neurons->size() ] );
         }
      }
   };


AnalogLimNeuronsOdeSystem::AnalogLimNeuronsOdeSystem()
   : OdeSystem()
   {
   // Do nothing;
   };


AnalogLimNeuronsOdeSystem::AnalogLimNeuronsOdeSystem( AnalogLimNeuronsOdeSystem & other )
   : OdeSystem( other )
   {
   // Do nothing;
   };
