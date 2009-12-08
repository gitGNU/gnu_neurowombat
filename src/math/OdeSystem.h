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


#ifndef ODESYSTEM_H
#define ODESYSTEM_H


#include <vector>


#include "neurons/abstract/AbstractNeuron.h"


/***************************************************************************
 *   OdeSystem abstract class declaration                                  *
 ***************************************************************************/


class OdeSystem
   {
   public:
      OdeSystem( unsigned int equationsCount );
      virtual ~OdeSystem();

      unsigned int getEquationsCount();

      virtual double evaluateOdeFunction( unsigned int index ) = 0;

      void setIndependentVariable( double x );
      double getIndependentVariable();
      void incIndependentVariable( double x );

      virtual void setDependentVariable( unsigned int index, double x );
      double getDependentVariable( unsigned int index );
      virtual void incDependentVariable( unsigned int index, double x );
      virtual void incDependentVariables( double * x1, double * x2, double k1, double k2 );

   protected:
      OdeSystem();
      OdeSystem( OdeSystem & other );

      unsigned int equationsCount;
      double indepVar;
      double * depVars;
   };


/***************************************************************************
 *   AbstractNeuronsOdeSystem class declaration                            *
 ***************************************************************************/


class AbstractNeuronsOdeSystem : public OdeSystem
   {
   public:
      AbstractNeuronsOdeSystem(
         std::vector < AbstractNeuron * > * neurons,
         double timeConstant
         );

      virtual ~AbstractNeuronsOdeSystem();

      virtual double evaluateOdeFunction( unsigned int index );

      virtual void setDependentVariable( unsigned int index, double x );
      virtual void incDependentVariable( unsigned int index, double x );
      virtual void incDependentVariables( double * x1, double * x2, double k1, double k2 );

   protected:
      AbstractNeuronsOdeSystem();
      AbstractNeuronsOdeSystem( AbstractNeuronsOdeSystem & other );

   private:
      std::vector < AbstractNeuron * > * neurons;
      double timeConstant;
   };


#endif
