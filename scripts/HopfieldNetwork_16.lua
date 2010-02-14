--   Copyright (C) 2009, 2010 Andrew Timashov
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


require "analog.HopfieldNetwork";
require "reliability";

-- Function for testing Hopfield network;
function testNetwork()
   local x = {};
   local hitsCount = 0;
   local totalCount = 10;

   -- Generate bottom "1" pattern for i <= 5 and top "1" pattern for i > 5;
   for i = 1, 10 do
      local bottomPatternCounter = 0;
      local topPatternCounter = 0;
      for j = 1, net.neuronsCount do
         if ( ( i <= 5 and j <= 8 ) or ( i > 5 and j > 8 ) ) then
            if math.random ( 4 ) == 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
         else
            if math.random ( 4 ) > 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
            end

         if ( j <= 8 and x[ j ] == 1.0 ) then topPatternCounter = topPatternCounter + 1; end
         if ( j > 8 and x[ j ] == 1.0 ) then bottomPatternCounter = bottomPatternCounter + 1; end
         end

      local y = analog.HopfieldNetwork.computeC( net, x, 5.0, 250 );

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

   return ( hitsCount / totalCount >= 0.75 );
   end

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
print( "Choose the option to do:\n1) Visualize results\n2) Estimate time to fail\n3) Estimate survival function\n4) Estimate component importance\n5) Estimate time to fail distribution\n6) Estimate faults count distribution\n7) Exit" );
while true do
   op = tonumber( io.read() );
   if op ~= nil then if op >= 1 and op <= 7 then break end end
   end

if op == 1 then
   x = {
      { 1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, 1, -1 },
      { -1, -1, 1, 1, -1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, 1 }
      };

   for i = 1, 2 do
      y = analog.HopfieldNetwork.computeC( net, x[ i ], 5.0, 250 );
      for j = 0, 3 do
         for k = 1, 4 do
            io.write( y[ j * 4 + k ] .. " " );
            end

         print( "" );
         end

      print( "" );
      end

elseif op >= 2 and op <= 6 then
   op = op - 1;
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 10000.0 }, { 0.0, 8000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         distr = createDistribution( DISTR.EXP, x );
         manager1 = createInterruptManager( net.resistors, distr, nil );
         manager2 = createInterruptManager( net.capacitors, distr, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager1 );
         appendInterruptManager( engine, manager2 );
         y, dyl, dyh = reliability.estimateTimeToFail( 121, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager2 );
         closeId( manager1 );
         closeId( distr );
         end

   elseif op >= 2 and op <= 5 then
      distr = createDistribution( DISTR.EXP, 0.0001 );
      manager1 = createInterruptManager( net.resistors, distr, nil );
      manager2 = createInterruptManager( net.capacitors, distr, nil );
      engine = createSimulationEngine();
      appendInterruptManager( engine, manager1 );
      appendInterruptManager( engine, manager2 );
      if op == 2 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 200, engine, testNetwork );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 3 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateComponentImportance( x, 200, engine, testNetwork, manager2, 0 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 200, engine, testNetwork, { manager1, manager2 } );
         for i = 0, net.resistorsCount + net.capacitorsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager2 );
      closeId( manager1 );
      closeId( distr );
      end
   end

-- Close network;
analog.HopfieldNetwork.destroy( net );

