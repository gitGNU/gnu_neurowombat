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


#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H


#include <vector>


#include "kernel/KernelObject.h"
#include "engine/InterruptManager.h"


/***************************************************************************
 *   SimulationEngine class declaration                                    *
 ***************************************************************************/


class SimulationEngine : public KernelObject
   {
   public:
      SimulationEngine();
      virtual ~SimulationEngine();

      void appendManager( InterruptManager * manager );
      void insertManagerBefore( unsigned int index, InterruptManager * manager );
      void deleteManager( unsigned int index );
      void clear();

      bool stepOver();

      double getCurrentTime();
      double getFutureTime();
      InterruptManager * getCurrentIntSource();
      InterruptManager * getFutureIntSource();

   private:
      // Most time-consuming operation;
      InterruptManager * findOutIntSource();

      std::vector< InterruptManager * > managers;
      double currentTime;
      InterruptManager * currentIntSource;
      InterruptManager * futureIntSource;
   };


#endif
