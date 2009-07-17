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


#ifndef ABSTRACTCONNECTORS_H
#define ABSTRACTCONNECTORS_H


#include "kernel/KernelObject.h"
//#include "InterruptManager.h"


/***************************************************************************
 *   AbstractConnectors class declaration                                  *
 ***************************************************************************/


class AbstractConnectors : public KernelObject
   {
   public:
      AbstractConnectors( unsigned int count = 0 );
      ~AbstractConnectors();

      unsigned int count() const;

      void copySignal( unsigned int destIndex, unsigned int srcIndex );
      void setSignal( unsigned int index, double signal );
      double getSignal( unsigned int index ) const;

      void repareAll();
      void breakWire( unsigned int index );
      void repareWire( unsigned int index );

   private:
      unsigned int numWires;
      double * analogSignals;
      bool * broken;
   };


/***************************************************************************
 *   AbstractConnectorsManager class declaration                           *
 ***************************************************************************/


/*class AnalogWiresManager : public InterruptManager
   {
   public:
      AnalogWiresManager();
      AnalogWiresManager(
         Destribution & destribution,
         AnalogWires * analogWires
         );

      virtual ~AnalogWiresManager();

      virtual void handleInterrupt();
   };*/


#endif
