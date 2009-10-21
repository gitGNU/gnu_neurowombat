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


#ifndef DESTRIBUTION_H
#define DESTRIBUTION_H


// This module use random generator;


#include "kernel/KernelObject.h"


/***************************************************************************
 *   Destribution abstract class declaration                               *
 ***************************************************************************/


class Destribution : public KernelObject
   {
   public:
      Destribution();
      virtual ~Destribution();
      virtual Destribution * clone() = 0;

      virtual double generateTime() = 0;

   protected:
      // Use QT's qrand();
      inline double genUniformRandomValue();
   };


/***************************************************************************
 *   ExponentialDestribution class declaration                             *
 ***************************************************************************/


class ExponentialDestribution : public Destribution
   {
   public:
      ExponentialDestribution( double lambda );
      virtual ~ExponentialDestribution();
      virtual Destribution * clone();

      virtual double generateTime();

   private:
      double lambda;
   };


/***************************************************************************
 *   WeibullDestribution class declaration                                 *
 ***************************************************************************/


class WeibullDestribution : public Destribution
   {
   public:
      WeibullDestribution( double theta, double beta );
      virtual ~WeibullDestribution();
      virtual Destribution * clone();

      virtual double generateTime();

   private:
      double theta;
      double beta;
   };


#endif
