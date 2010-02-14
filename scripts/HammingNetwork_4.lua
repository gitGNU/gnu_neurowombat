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


require "analog.HammingNetwork";
require "reliability";

-- Function for testing Hamming network;
function testNetwork()
   local x = {};
   local hitsCount = 0;
   local totalCount = 10;

   -- Generate bottom "1" pattern for i <= 5 and top "1" pattern for i > 5;
   for i = 1, 10 do
      local bottomPatternCounter = 0;
      local topPatternCounter = 0;
      for j = 1, inputs do
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

      local y = analog.HammingNetwork.computeC( net, x, 5.0, 250 );

      -- Analize result;
      if topPatternCounter == bottomPatternCounter then totalCount = totalCount - 1; end

      if ( topPatternCounter > bottomPatternCounter and y[ 1 ] == 1.0 and y[ 2 ] == -1.0 or
         topPatternCounter < bottomPatternCounter and y[ 1 ] == -1.0 and y[ 2 ] == 1.0 
         ) then hitsCount = hitsCount + 1; end
      end

   return ( hitsCount / totalCount >= 0.75 );
   end

-- Create Hamming network;
io.write( "Assembling Hamming network ( 4 neurons ) ... " ); io.flush();
inputs = 16;
layer = 2;
net = analog.HammingNetwork.create( inputs, layer );
print( "[OK]" );

-- Train Hamming network;
io.write( "Training ... " ); io.flush();
trainVectors = {
   { 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1 },
   { -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1 }
   };

analog.HammingNetwork.train( net, trainVectors );
print( "[OK]" );

-- Simulate Hamming network;
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
      y = analog.HammingNetwork.computeC( net, x[ i ], 5.0, 250 );
      print( y[ 1 ] .. " " .. y[ 2 ] );
      end

elseif op >= 2 and op <= 6 then
   op = op - 1;
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 10000.0 }, { 0.0, 12000.0 } };

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
         y, dyl, dyh = reliability.estimateTimeToFail( 250, engine, testNetwork );
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
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 2000, engine, testNetwork );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 3 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager1, 0 );
            y1, dyl1, dyh1 = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager1, 32 );
            y2, dyl2, dyh2 = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager2, 0 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 .. " " .. dyl2 .. " " .. y2 .. " " .. dyh2 );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 200, engine, testNetwork, { manager1 } );
         for i = 0, net.resistorsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager2 );
      closeId( manager1 );
      closeId( distr );
      end
   end

-- Close network;
analog.HammingNetwork.destroy( net );

