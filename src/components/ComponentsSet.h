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


#ifndef COMPONENTSSET_H
#define COMPONENTSSET_H


#include "kernel/KernelObject.h"


/***************************************************************************
 *   ComponentsSet class declaration                                       *
 ***************************************************************************/


template < class T >
   class ComponentsSet : public KernelObject
      {
      public:
         ComponentsSet( unsigned int count = 0 );
         virtual ~ComponentsSet();

         T & operator []( unsigned int index );
         T & at( unsigned int index );

         unsigned int count() const;

      protected:
         unsigned int propertiesCount;
         T * properties;
      };


/***************************************************************************
 *   ComponentsSet class implementation                                    *
 ***************************************************************************/


template < class T >
ComponentsSet < T >::ComponentsSet( unsigned int count )
   : KernelObject()
   {
   propertiesCount = count;

   if ( count > 0 )
      {
      properties = new T[ count ];
      }
   else
      {
      properties = NULL;
      }
   };


template < class T >
   ComponentsSet < T >::~ComponentsSet()
      {
      if ( properties != NULL ) delete[] properties;
      };


template < class T >
   T & ComponentsSet < T >::operator []( unsigned int index )
      {
      return properties[ index ];
      };


template < class T >
   T & ComponentsSet < T >::at( unsigned int index )
      {
      return properties[ index ];
      };


template < class T >
   unsigned int ComponentsSet < T >::count() const
      {
      return propertiesCount;
      };


#endif
