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
#include <stdlib.h>


#include "math/Destribution.h"


/***************************************************************************
 *   Destribution abstract class implementation                            *
 ***************************************************************************/


Destribution::Destribution()
   {
   // Do nothing;
   };


Destribution::~Destribution()
   {
   // Do nothing;
   };


inline double Destribution::genUniformRandomValue()
   {
   return (
      ( ( double ) rand() )
      /
      ( ( double ) RAND_MAX )
      );
   };


/***************************************************************************
 *   ExponentialDestribution class implementation                          *
 ***************************************************************************/


ExponentialDestribution::ExponentialDestribution( double lambda )
   : Destribution()
   {
   this->lambda = lambda;
   };


ExponentialDestribution::~ExponentialDestribution()
   {
   // Do nothing;
   };


Destribution * ExponentialDestribution::clone()
   {
   return new ExponentialDestribution( * this );
   };


double ExponentialDestribution::generateTime()
   {
   return ( - log( 1.0 - this->genUniformRandomValue() ) ) / this->lambda;
   };


/***************************************************************************
 *   WeibullDestribution class implementation                              *
 ***************************************************************************/


WeibullDestribution::WeibullDestribution( double teta, double beta )
   : Destribution()
   {
   this->teta = teta;
   this->beta = beta;
   };


WeibullDestribution::~WeibullDestribution()
   {
   // Do nothing;
   };


Destribution * WeibullDestribution::clone()
   {
   return new WeibullDestribution( * this );
   };


double WeibullDestribution::generateTime()
   {
   return pow( - log( 1.0 - this->genUniformRandomValue() ) / this->teta, 1.0 / this->beta );
   };
