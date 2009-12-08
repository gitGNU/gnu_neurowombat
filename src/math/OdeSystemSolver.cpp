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


#include "math/OdeSystemSolver.h"


/***************************************************************************
 *   OdeSystemSolver class implementation                                  *
 ***************************************************************************/


void OdeSystemSolver::solve(
   OdeSystem & odeSystem,
   double indepVarValue0,
   double indepVarValue1,
   unsigned int stepsCount
   )
   {
   unsigned int equationsCount = odeSystem.getEquationsCount();

   double * terms[ 4 ];
   terms[ 0 ] = new double[ equationsCount ];
   terms[ 1 ] = new double[ equationsCount ];
   terms[ 2 ] = new double[ equationsCount ];
   terms[ 3 ] = new double[ equationsCount ];

   odeSystem.setIndependentVariable( indepVarValue0 );

   double delta = ( indepVarValue1 - indepVarValue0 ) / stepsCount;
   for ( unsigned int step = 0; step < stepsCount; step ++ )
      {
      // Calculate terms[ 0 ];
      for ( unsigned int i = 0; i < equationsCount; i ++ )
         {
         terms[ 0 ][ i ] = odeSystem.evaluateOdeFunction( i );
         }

      odeSystem.incIndependentVariable( 0.5 * delta );
      odeSystem.incDependentVariables( terms[ 0 ], terms[ 0 ], 0.25 * delta, 0.25 * delta );

      // Calculate terms[ 1 ];
      for ( unsigned int i = 0; i < equationsCount; i ++ )
         {
         terms[ 1 ][ i ] = odeSystem.evaluateOdeFunction( i );
         }

      odeSystem.incDependentVariables( terms[ 1 ], terms[ 0 ], 0.5 * delta, -0.5 * delta );

      // Calculate terms[ 2 ];
      for ( unsigned int i = 0; i < equationsCount; i ++ )
         {
         terms[ 2 ][ i ] = odeSystem.evaluateOdeFunction( i );
         }

      odeSystem.incIndependentVariable( 0.5 * delta );
      odeSystem.incDependentVariables( terms[ 2 ], terms[ 1 ],  delta, -0.5 * delta );

      // Calculate terms[ 3 ];
      for ( unsigned int i = 0; i < equationsCount; i ++ )
         {
         terms[ 3 ][ i ] = odeSystem.evaluateOdeFunction( i );
         }

      // Increase dependent variables;
      for ( unsigned int i = 0; i < equationsCount; i ++ )
         {
         odeSystem.incDependentVariable(
            i,
            - delta * terms[ 2 ][ i ] + delta * ( terms[ 0 ][ i ] + 2.0 * terms[ 1 ][ i ] + 2.0 * terms[ 2 ][ i ] + terms[ 3 ][ i ] ) / 6.0
            );
         }
      }

   delete[] terms[ 3 ];
   delete[] terms[ 2 ];
   delete[] terms[ 1 ];
   delete[] terms[ 0 ];
   };


OdeSystemSolver::OdeSystemSolver()
   {
   // Do nothing;
   };


OdeSystemSolver::OdeSystemSolver( OdeSystemSolver & other )
   {
   // Do nothing;
   };


OdeSystemSolver::~OdeSystemSolver()
   {
   // Do nothing;
   };
