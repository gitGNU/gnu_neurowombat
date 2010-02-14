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


#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H


// This module uses random generator;


#include "kernel/KernelObject.h"
#include "objects/CustomFunction.h"


/***************************************************************************
 *   T_DISTR enum declaration                                              *
 ***************************************************************************/

namespace DISTR
   {
   enum T_DISTR
      {
      CUSTOM,
      EXP,
      WEIBULL
      };
   };


/***************************************************************************
 *   Distribution abstract class declaration                               *
 ***************************************************************************/


class Distribution : public KernelObject
   {
   public:
      Distribution();
      virtual ~Distribution();

      virtual double generateTime() = 0;

   protected:
      // Use rand();
      inline double genUniformRandomValue();
   };


/***************************************************************************
 *   CustomDistribution class declaration                                  *
 ***************************************************************************/


class CustomDistribution : public Distribution
   {
   public:
      CustomDistribution( CustomFunction * customInverseFunction );
      CustomDistribution( CustomDistribution & other );
      virtual ~CustomDistribution();

      virtual double generateTime();

   private:
      CustomDistribution( const CustomDistribution & other );

      CustomFunction * customInverseFunction;
   };


/***************************************************************************
 *   ExponentialDistribution class declaration                             *
 ***************************************************************************/


class ExponentialDistribution : public Distribution
   {
   public:
      ExponentialDistribution( double lambda );
      virtual ~ExponentialDistribution();

      virtual double generateTime();

   private:
      double lambda;
   };


/***************************************************************************
 *   WeibullDistribution class declaration                                 *
 ***************************************************************************/


class WeibullDistribution : public Distribution
   {
   public:
      WeibullDistribution( double theta, double beta );
      virtual ~WeibullDistribution();

      virtual double generateTime();

   private:
      double theta;
      double beta;
   };


#endif
