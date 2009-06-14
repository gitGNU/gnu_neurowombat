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


#include "engine/SimulationEngine.h"


/***************************************************************************
 *   SimulationEngine class implementation                                 *
 ***************************************************************************/


SimulationEngine::SimulationEngine()
   {
   this->currentTime = 0.0;
   };


SimulationEngine::~SimulationEngine()
   {
   // Do nothing;
   };


void SimulationEngine::appendManager( InterruptManager * manager )
   {
   if ( manager == NULL )
      {
      // throw SimulationEngineExcp( NS_SIMULATIONENGINE::NULL_PARAMETER );
      }

   this->managers.push_back( manager );
   };


void SimulationEngine::insertManagerBefore( unsigned int index, InterruptManager * manager )
   {
   if ( index >= this->managers.size() )
      {
      // throw SimulationEngineExcp( NS_SIMULATIONENGINE::INDEX_OUT_OF_RANGE );
      }

   if ( manager == NULL )
      {
      // throw SimulationEngineExcp( NS_SIMULATIONENGINE::NULL_PARAMETER );
      }

   this->managers.insert( this->managers.begin() + index, manager );
   };


void SimulationEngine::deleteManager( unsigned int index )
   {
   if ( index >= this->managers.size() )
      {
      // throw SimulationEngineExcp( NS_SIMULATIONENGINE::INDEX_OUT_OF_RANGE );
      }

   this->managers.erase( this->managers.begin() + index );
   };


void SimulationEngine::clear()
   {
   this->managers.clear();
   };


bool SimulationEngine::stepOver()
   {
   double min = -1.0;
   int winner = -1;

   int i = this->managers.size() - 1;

   // Initialize minimum;
   while ( i >= 0 )
      {
      if ( this->managers[ i ]->getInterrupt() >= 0.0 )
         {
         min = this->managers[ i ]->getInterrupt();
         winner = i;
         break;
         }

      i --;
      }

   // Search for minimum;
   double currentInt = -1.0;
   while ( i >= 0 )
      {
      currentInt = this->managers[ i ]->getInterrupt();
      if ( currentInt >= 0.0 && currentInt < min )
         {
         min = currentInt;
         winner = i;
         }

      i --;
      }

   // Call winner's interrupt handler;
   if ( winner >= 0 )
      {
      this->currentTime = this->managers[ winner ]->getInterrupt();
      this->managers[ winner ]->handleInterrupt();
      return true;
      }

   return false;
   };


double SimulationEngine::getCurrentTime()
   {
   return this->currentTime;
   };
