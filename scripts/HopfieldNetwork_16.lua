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


-- Hopfield neural network script ( 16 neurons ). Network task is to 
-- classify 4x4 images. It works as associative memory and returns one 
-- of the base images for every input image. There are 2 base images:
-- ####     ....
-- #### and ....
-- ....     ####
-- ....     ####
-- Network has to perform at least 90% correct classification for 100
-- random images like
-- ###.     ..##
-- #### or  ....
-- ....     #.##
-- #..#     ###.
-- Otherwise it is considered to be in a fault state.

require "analog.HopfieldNetwork";
require "reliability";

-- Function for testing Hopfield network;
function testNetwork()
   local x = {};
   local hitsCount = 0;
   local totalCount = 100;

   -- Generate bottom "1" pattern for i <= 50 and top "1" pattern for i > 50;
   for i = 1, 100 do
      local bottomPatternCounter = 0;
      local topPatternCounter = 0;
      for j = 1, net.neuronsCount do
         if ( ( i <= 50 and j <= 8 ) or ( i > 50 and j > 8 ) ) then
            if math.random ( 4 ) == 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
         else
            if math.random ( 4 ) > 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
            end

         if ( j <= 8 and x[ j ] == 1.0 ) then topPatternCounter = topPatternCounter + 1; end
         if ( j > 8 and x[ j ] == 1.0 ) then bottomPatternCounter = bottomPatternCounter + 1; end
         end

      local y = analog.HopfieldNetwork.compute( net, x, 4 );

      -- Analize result;
      local isTopPatern = true;
      local isBottomPatern = true;
      for j = 1, net.neuronsCount do
         if j <= 8 then
            if y[ j ] ~= 1.0 then isTopPatern = false; end
            if y[ j ] ~= -1.0 then isBottomPatern = false; end
         else
            if y[ j ] ~= -1.0 then isTopPatern = false; end
            if y[ j ] ~= 1.0 then isBottomPatern = false; end
            end
         end

      if topPatternCounter == bottomPatternCounter then totalCount = totalCount - 1; end

      if ( topPatternCounter > bottomPatternCounter and isTopPatern or
         topPatternCounter < bottomPatternCounter and isBottomPatern
         ) then hitsCount = hitsCount + 1; end
      end

   return ( hitsCount / totalCount >= 0.90 );
   end

print( "API version: " .. apiVersion() );

-- Create neurons layer;
io.write( "Assembling Hopfield network ( 16 neurons ) ... " ); io.flush();
neurons = 16;
net = analog.HopfieldNetwork.create( neurons );
print( "[OK]" );

-- Train Hopfield network;
io.write( "Training ... " ); io.flush();
trainVectors = {
   { 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1 },
   { -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1 }
   };

analog.HopfieldNetwork.train( net, trainVectors );
print( "[OK]" );

-- Simulate Hopfield network;
print( "Choose the option to do:\n1) Estimate time to fail\n2) Estimate survival function\n3) Estimate component importance\n4) Estimate time to fail destribution\n5) Estimate faults count destribution\n6) Exit" );
op = 0;
repeat
   local ops = io.read();
   if ops == "1" or ops == "2" or ops == "3" or ops == "4" or ops == "5" or ops == "6" then op = tonumber( ops ) end
   until op ~= 0

if op >= 1 and op <= 5 then
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 30000.0 }, { 9000.0, 25000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         destr = createExponentialDestribution( x );
         manager = createAnalogResistorsManager( destr, net.resistors, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager );
         y, dyl, dyh = reliability.estimateTimeToFail( 121, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager );
         closeId( destr );
         end

   elseif op >= 2 and op <= 5 then
      destr = createExponentialDestribution( 0.0001 );
      manager = createAnalogResistorsManager( destr, net.resistors, nil );
      engine = createSimulationEngine();
      appendInterruptManager( engine, manager );
      if op == 2 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 500, engine, testNetwork );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 3 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateComponentImportance( x, 1000, engine, testNetwork, manager, 0 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDestribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDestribution( 200, engine, testNetwork, manager );
         for i = 0, net.resistorsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end
   end

-- Close network;
analog.HopfieldNetwork.destroy( net );

