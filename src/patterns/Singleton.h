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


#ifndef SINGLETON_H
#define SINGLETON_H


#ifndef NULL
#define NULL 0
#endif


/***************************************************************************
 *   Singleton class declaration                                           *
 ***************************************************************************/


template < class T >
   class Singleton
      {
      public:
         static T * instance();
         void freeInstance();
      protected:
         Singleton();
         virtual ~Singleton();
      private:
         enum CONSTANTS
            {
            MAX_REFERENCE_COUNT = 0xFFFFFFFF
            };

         static T * self;
         static unsigned int referenceCount;
      };


/***************************************************************************
 *   Singleton class implementation                                        *
 ***************************************************************************/

template < class T >
   T * Singleton < T >::instance()
      {
      if ( referenceCount >= Singleton < T >::MAX_REFERENCE_COUNT ) return NULL;

      if ( self == NULL )
         {
         self = new T();
         }

      referenceCount ++;
      return self;
      };


template < class T >
   void Singleton < T >::freeInstance()
      {
      referenceCount --;

      if ( referenceCount == 0 )
         {
         delete this;
         }
      };


template < class T >
   Singleton < T >::Singleton()
      {
      // Do nothing;
      };


template < class T >
   Singleton < T >::~Singleton()
      {
      self = NULL;
      };


template < class T >
   T * Singleton < T >::self = NULL;


template < class T >
   unsigned int Singleton < T >::referenceCount = 0;


#endif
