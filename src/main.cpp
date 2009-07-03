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


#define NEUROWOMBAT_VERSION "0.1"


#define NEUROWOMBAT_OPTIONS "\
  --help                   Display this information\n\
  --version                Display neurovombat version"


#define COPYING "\
Copyright (C) 2009 Andrew Timashov\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law."



#include <stdio.h>
#include <cstdlib>
#include <string.h>
//#include <time.h>


#include "kernel/Kernel.h"


// Kernel decleared as public in order to increase
// performance of API functions. Anyway object remains
// singletone;
Kernel * kernel;


int main( int argc, char ** argv )
   {
   // Parse command line arguments;
   if ( argc < 2  )
      {
      printf( "neurowombat: no input file\n" );
      exit( EXIT_FAILURE );
      }
   else
      {
      if ( strcmp( argv[ 1 ], "--help" ) == 0 )
         {
         printf( "Usage: neurowombat [options] file\nOptions:\n%s\n", NEUROWOMBAT_OPTIONS );
         exit( EXIT_SUCCESS );
         }
      if ( strcmp( argv[ 1 ], "--version" ) == 0 )
         {
         printf( "neurowombat (NeuroWombat) %s\n%s\n", NEUROWOMBAT_VERSION, COPYING );
         exit( EXIT_SUCCESS );
         }
      }

   kernel = Kernel::instance();

   /*struct timespec tp;
   clock_gettime( CLOCK_REALTIME, & tp );
   time_t seconds = tp.tv_sec;
   long nanoseconds = tp.tv_nsec;*/
   kernel->doFile( argv[ 1 ] );
   /*clock_gettime( CLOCK_REALTIME, & tp );
   seconds = tp.tv_sec - seconds;
   nanoseconds = tp.tv_nsec - nanoseconds;
   printf( "seconds = %i\nmicroseconds = %li\n", seconds, nanoseconds / 1000 );*/

   kernel->freeInstance();
   };
