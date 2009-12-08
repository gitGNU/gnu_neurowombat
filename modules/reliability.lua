--   Copyright (C) 2009 Andrew Timashov
--
--   This file is part of NeuroWombat.
--
--   NeuroWombat is free software: you can redistribute it and/or modify
--   it under the terms of the GNU General Public License as published by
--   the Free Software Foundation, either version 3 of the License, or
--   (at your option) any later version.
--
--   NeuroWombat is distributed in the hope that it will be useful,
--   but WITHOUT ANY WARRANTY; without even the implied warranty of
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--   GNU General Public License for more details.
--
--   You should have received a copy of the GNU General Public License
--   along with NeuroWombat.  If not, see <http://www.gnu.org/licenses/>.


module( ..., package.seeall )


function estimateTimeToFail( times, engine, testFunc )
   local t = 0.0;
   local tsqr = 0.0;
   for i = 1, times do
      repeat
         if not testFunc() then
            local x = getCurrentTime( engine );
            t = t + x;
            tsqr = tsqr + x * x;
            break;
            end

         until not stepOverEngine( engine )

      restartEngine( engine );
      end

   t = t / times;
   tsqr = tsqr / times;
   local d = calcMeanCI( t, tsqr, times, 0.95 );
   return t, t - d, t + d;
   end


function estimateSurvivalFunction( t, times, engine, testFunc )
   local p = 0.0;
   for i = 1, times do
      local tIsTooLarge = true;
      repeat
         if getCurrentTime( engine ) >= t then
            if testNetwork() then p = p + 1.0 end
            tIsTooLarge = false;
            break;
            end

         until not stepOverEngine( engine )

      if tIsTooLarge then
         if testFunc() then p = p + 1.0 end
         end;

      restartEngine( engine );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end


function estimateComponentImportance( t, times, engine, testFunc, manager, intSrc )
   local p = 0.0;
   for i = 1, times do
      if t >= getFutureTime( engine ) then
         while stepOverEngine( engine ) do
            if t < getFutureTime( engine ) then break end
            end         
         end

      if testFunc() then
         simulateInterrupt( manager, intSrc );
         if not testFunc() then p = p + 1.0 end
         end

      restartEngine( engine );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end


function estimateTimeToFailDestribution( times, engine, testFunc )
   local d = {};
   for i = 1, times do
      repeat
         if not testFunc() then break end
         until not stepOverEngine( engine )

      d[ i ] = getCurrentTime( engine );

      restartEngine( engine );
      end

   return d;
   end


function estimateFaultsCountDestribution( times, engine, testFunc, manager )
   local d = {};
   local componentsCount = getIntSourcesCount( manager );
   for i = 0, componentsCount do d[ i ] = 0 end
   for i = 1, times do
      repeat
         if not testFunc() then break end
         until not stepOverEngine( engine )

      local faultsCount = getInterruptsCount( manager );
      d[ faultsCount ] = d[ faultsCount ] + 1;
      restartEngine( engine );
      end

   for i = 0, componentsCount do d[ i ] = d[ i ] / times end
   return d;
   end

