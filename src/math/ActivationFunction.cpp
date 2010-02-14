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


#include "ActivationFunction.h"


/***************************************************************************
 *   ActivationFunction abstract class implementation                      *
 ***************************************************************************/


ActivationFunction::ActivationFunction()
   : KernelObject()
   {
   // Do nothing;
   };


ActivationFunction::~ActivationFunction()
   {
   // Do nothing;
   };


/***************************************************************************
 *   CustomActivationFunction class implementation                         *
 ***************************************************************************/


CustomActivationFunction::CustomActivationFunction( CustomFunction * customFunction, CustomFunction * customDerivative )
   : ActivationFunction()
   {
   this->customFunction = customFunction;
   this->customDerivative = customDerivative;

   if ( customFunction != NULL ) customFunction->capture();
   if ( customDerivative != NULL ) customDerivative->capture();
   };


CustomActivationFunction::CustomActivationFunction( const CustomActivationFunction & other )
   : ActivationFunction( other )
   {
   customFunction = other.customFunction;
   customDerivative = other.customDerivative;

   if ( customFunction != NULL ) customFunction->capture();
   if ( customDerivative != NULL ) customDerivative->capture();
   };


CustomActivationFunction::~CustomActivationFunction()
   {
   if ( customFunction != NULL ) customFunction->release();
   if ( customDerivative != NULL ) customDerivative->release();
   };


double CustomActivationFunction::evaluateFunction( double x )
   {
   return customFunction->call( x );
   };


double CustomActivationFunction::evaluateDerivative( double x )
   {
   return customDerivative->call( x );
   };


/***************************************************************************
 *   GaussianActivationFunction class implementation                       *
 ***************************************************************************/


GaussianActivationFunction::GaussianActivationFunction( double beta )
   : ActivationFunction()
   {
   this->beta = beta;
   };


GaussianActivationFunction::~GaussianActivationFunction()
   {
   // Do nothing;
   };


double GaussianActivationFunction::evaluateFunction( double x )
   {
   return exp( - beta * x * x );
   };


double GaussianActivationFunction::evaluateDerivative( double x )
   {
   return - 2 * x * beta * exp( - beta * x * x );
   };


/***************************************************************************
 *   LimActivationFunction class implementation                            *
 ***************************************************************************/


LimActivationFunction::LimActivationFunction( double xLim, double yLow, double yHigh )
   : ActivationFunction()
   {
   this->xLim = xLim;
   this->yLow = yLow;
   this->yHigh = yHigh;
   };


LimActivationFunction::~LimActivationFunction()
   {
   // Do nothing;
   };


double LimActivationFunction::evaluateFunction( double x )
   {
   return ( x < xLim ) ? yLow : yHigh;
   };


double LimActivationFunction::evaluateDerivative( double x )
   {
   return 0.0;
   };


/***************************************************************************
 *   LinearActivationFunction class implementation                         *
 ***************************************************************************/


LinearActivationFunction::LinearActivationFunction( double a, double b )
   : ActivationFunction()
   {
   this->a = a;
   this->b = b;
   };


LinearActivationFunction::~LinearActivationFunction()
   {
   // Do nothing;
   };


double LinearActivationFunction::evaluateFunction( double x )
   {
   return this->a * x + this->b;
   };


double LinearActivationFunction::evaluateDerivative( double x )
   {
   return this->a;
   };


/***************************************************************************
 *   LimLinearActivationFunction class implementation                      *
 ***************************************************************************/


LimLinearActivationFunction::LimLinearActivationFunction( double a, double b, double xMin, double xMax )
   : ActivationFunction()
   {
   this->a = a;
   this->b = b;
   this->xMin = xMin;
   this->xMax = xMax;
   };


LimLinearActivationFunction::~LimLinearActivationFunction()
   {
   // Do nothing;
   };


double LimLinearActivationFunction::evaluateFunction( double x )
   {
   if ( x < xMin )
      {
      return a * xMin + b;
      }
   else if ( x > xMax )
      {
      return a * xMax + b;
      }

   return a * x + b;
   };


double LimLinearActivationFunction::evaluateDerivative( double x )
   {
   if ( x < xMin || x > xMax )
      {
      return 0.0;
      }

   return a;
   };


/***************************************************************************
 *   PosLinearActivationFunction class implementation                      *
 ***************************************************************************/


PosLinearActivationFunction::PosLinearActivationFunction( double a, double b )
   : ActivationFunction()
   {
   this->a = a;
   this->b = b;
   };


PosLinearActivationFunction::~PosLinearActivationFunction()
   {
   // Do nothing;
   };


double PosLinearActivationFunction::evaluateFunction( double x )
   {
   double f = a * x + b;
   return ( f >= 0.0 ) ? f : 0.0;
   };


double PosLinearActivationFunction::evaluateDerivative( double x )
   {
   return ( a * x + b >= 0.0 ) ?  a : 0.0;
   };


/***************************************************************************
 *   SigmoidActivationFunction class implementation                        *
 ***************************************************************************/


SigmoidActivationFunction::SigmoidActivationFunction( double lambda )
   : ActivationFunction()
   {
   this->lambda = lambda;
   };


SigmoidActivationFunction::~SigmoidActivationFunction()
   {
   // Do nothing;
   };


double SigmoidActivationFunction::evaluateFunction( double x )
   {
   return ( 1.0 / ( 1.0 + exp( - lambda * x ) ) );
   };


double SigmoidActivationFunction::evaluateDerivative( double x )
   {
   double f = 1.0 / ( 1.0 + exp( - lambda * x ) );
   return lambda * f * ( 1 - f );
   };


/***************************************************************************
 *   ThSigmoidActivationFunction class implementation                      *
 ***************************************************************************/


ThSigmoidActivationFunction::ThSigmoidActivationFunction()
   : ActivationFunction()
   {
   // Do nothing;
   };


ThSigmoidActivationFunction::~ThSigmoidActivationFunction()
   {
   // Do nothing;
   };


double ThSigmoidActivationFunction::evaluateFunction( double x )
   {
   return tanh( x );
   };


double ThSigmoidActivationFunction::evaluateDerivative( double x )
   {
   double f = cosh( x );
   return 1.0 / ( f * f );
   };
