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


#include "TransferFunction.h"


/***************************************************************************
 *   TransferFunction abstract class implementation                        *
 ***************************************************************************/


TransferFunction::TransferFunction()
   : KernelObject()
   {
   // Do nothing;
   };


TransferFunction::~TransferFunction()
   {
   // Do nothing;
   };


/***************************************************************************
 *   LinearTransferFunction class implementation                           *
 ***************************************************************************/


LinearTransferFunction::LinearTransferFunction( double a, double b )
   : TransferFunction()
   {
   this->a = a;
   this->b = b;
   };


LinearTransferFunction::~LinearTransferFunction()
   {
   // Do nothing;
   };


TransferFunction * LinearTransferFunction::clone()
   {
   return new LinearTransferFunction( * this );
   };


double LinearTransferFunction::evaluateFunction( double x )
   {
   return this->a * x + this->b;
   };


double LinearTransferFunction::evaluateDerivative( double x )
   {
   return this->a;
   };


/***************************************************************************
 *   SigmoidTransferFunction class implementation                          *
 ***************************************************************************/


SigmoidTransferFunction::SigmoidTransferFunction()
   : TransferFunction()
   {
   // Do nothing;
   };


SigmoidTransferFunction::~SigmoidTransferFunction()
   {
   // Do nothing;
   };


TransferFunction * SigmoidTransferFunction::clone()
   {
   return new SigmoidTransferFunction( * this );
   };


double SigmoidTransferFunction::evaluateFunction( double x )
   {
   return ( 1.0 / ( 1.0 + exp( - x ) ) );
   };


double SigmoidTransferFunction::evaluateDerivative( double x )
   {
   double f = 1.0 / ( 1.0 + exp( - x ) );
   return f * ( 1 - f );
   };
