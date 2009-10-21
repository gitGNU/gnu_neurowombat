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


#ifndef ACTIVATIONFUNCTION_H
#define ACTIVATIONFUNCTION_H


#include <lua.hpp>


#include "kernel/KernelObject.h"


/***************************************************************************
 *   ActivationFunction abstract class declaration                         *
 ***************************************************************************/


class ActivationFunction : public KernelObject
   {
   public:
      ActivationFunction();
      virtual ~ActivationFunction();
      virtual ActivationFunction * clone() = 0;

      virtual double evaluateFunction( double x ) = 0;
      virtual double evaluateDerivative( double x ) = 0;
   };


/***************************************************************************
 *   CustomActivationFunction class declaration                            *
 ***************************************************************************/


class CustomActivationFunction : public ActivationFunction
   {
   public:
      CustomActivationFunction( lua_State * L, int functionRef, int derivativeRef );
      virtual ~CustomActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      lua_State * L;
      int functionRef;
      int derivativeRef;
   };


/***************************************************************************
 *   GaussianActivationFunction class declaration                          *
 ***************************************************************************/


class GaussianActivationFunction : public ActivationFunction
   {
   public:
      GaussianActivationFunction( double beta );
      virtual ~GaussianActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      double beta;
   };


/***************************************************************************
 *   LimActivationFunction class declaration                               *
 ***************************************************************************/


class LimActivationFunction : public ActivationFunction
   {
   public:
      LimActivationFunction( double xLim, double yLow, double yHigh );
      virtual ~LimActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      double xLim;
      double yLow;
      double yHigh;
   };


/***************************************************************************
 *   LinearActivationFunction class declaration                            *
 ***************************************************************************/


class LinearActivationFunction : public ActivationFunction
   {
   public:
      LinearActivationFunction( double a, double b );
      virtual ~LinearActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      double a;
      double b;
   };


/***************************************************************************
 *   LimLinearActivationFunction class declaration                         *
 ***************************************************************************/


class LimLinearActivationFunction : public ActivationFunction
   {
   public:
      LimLinearActivationFunction( double a, double b, double xMin, double xMax );
      virtual ~LimLinearActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      double a;
      double b;
      double xMin;
      double xMax;
   };


/***************************************************************************
 *   PosLinearActivationFunction class declaration                         *
 ***************************************************************************/


class PosLinearActivationFunction : public ActivationFunction
   {
   public:
      PosLinearActivationFunction( double a, double b );
      virtual ~PosLinearActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );

   private:
      double a;
      double b;
   };


/***************************************************************************
 *   SigmoidActivationFunction class declaration                           *
 ***************************************************************************/


class SigmoidActivationFunction : public ActivationFunction
   {
   public:
      SigmoidActivationFunction();
      virtual ~SigmoidActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );
   };


/***************************************************************************
 *   ThSigmoidActivationFunction class declaration                         *
 ***************************************************************************/


class ThSigmoidActivationFunction : public ActivationFunction
   {
   public:
      ThSigmoidActivationFunction();
      virtual ~ThSigmoidActivationFunction();
      virtual ActivationFunction * clone();

      virtual double evaluateFunction( double x );
      virtual double evaluateDerivative( double x );
   };


#endif