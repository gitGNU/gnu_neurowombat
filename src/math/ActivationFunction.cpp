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


ActivationFunction * LinearActivationFunction::clone()
   {
   return new LinearActivationFunction( * this );
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
 *   SigmoidActivationFunction class implementation                        *
 ***************************************************************************/


SigmoidActivationFunction::SigmoidActivationFunction()
   : ActivationFunction()
   {
   // Do nothing;
   };


SigmoidActivationFunction::~SigmoidActivationFunction()
   {
   // Do nothing;
   };


ActivationFunction * SigmoidActivationFunction::clone()
   {
   return new SigmoidActivationFunction( * this );
   };


double SigmoidActivationFunction::evaluateFunction( double x )
   {
   return ( 1.0 / ( 1.0 + exp( - x ) ) );
   };


double SigmoidActivationFunction::evaluateDerivative( double x )
   {
   double f = 1.0 / ( 1.0 + exp( - x ) );
   return f * ( 1 - f );
   };
