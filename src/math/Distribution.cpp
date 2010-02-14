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
#include <stdlib.h>


#include "math/Distribution.h"


/***************************************************************************
 *   Distribution abstract class implementation                            *
 ***************************************************************************/


Distribution::Distribution()
   {
   // Do nothing;
   };


Distribution::~Distribution()
   {
   // Do nothing;
   };


inline double Distribution::genUniformRandomValue()
   {
   return (
      ( ( double ) rand() )
      /
      ( ( double ) RAND_MAX )
      );
   };


/***************************************************************************
 *   CustomDistribution class implementation                               *
 ***************************************************************************/


CustomDistribution::CustomDistribution( CustomFunction * customInverseFunction )
   : Distribution()
   {
   this->customInverseFunction = customInverseFunction;

   if ( customInverseFunction != NULL ) customInverseFunction->capture();
   };


CustomDistribution::CustomDistribution( const CustomDistribution & other )
   : Distribution( other )
   {
   customInverseFunction = other.customInverseFunction;

   if ( customInverseFunction != NULL ) customInverseFunction->capture();
   };


CustomDistribution::~CustomDistribution()
   {
   if ( customInverseFunction != NULL ) customInverseFunction->release();
   };


double CustomDistribution::generateTime()
   {
   return customInverseFunction->call( genUniformRandomValue() );
   };


/***************************************************************************
 *   ExponentialDistribution class implementation                          *
 ***************************************************************************/


ExponentialDistribution::ExponentialDistribution( double lambda )
   : Distribution()
   {
   this->lambda = lambda;
   };


ExponentialDistribution::~ExponentialDistribution()
   {
   // Do nothing;
   };


double ExponentialDistribution::generateTime()
   {
   return ( - log( 1.0 - genUniformRandomValue() ) ) / lambda;
   };


/***************************************************************************
 *   WeibullDistribution class implementation                              *
 ***************************************************************************/


WeibullDistribution::WeibullDistribution( double theta, double beta )
   : Distribution()
   {
   this->theta = theta;
   this->beta = beta;
   };


WeibullDistribution::~WeibullDistribution()
   {
   // Do nothing;
   };


double WeibullDistribution::generateTime()
   {
   return pow( - log( 1.0 - genUniformRandomValue() ) / theta, 1.0 / beta );
   };
