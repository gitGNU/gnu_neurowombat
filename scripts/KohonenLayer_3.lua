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


require "abstract.KohonenLayer";
require "reliability";

function genTrainVectors()
   local r = 5;
   local v = {};
   for i = 1, 15 do
      local realClass = math.random( 3 );
      v[ i ] = {};
      v[ i ][ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ i ][ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ i ][ 3 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      end

   return v;
end

-- Function for testing Kohonen layer;
function testNetwork()
   local r = 5;
   local hitsCount = 0;
   for i = 1, 100 do
      local realClass = math.random( 3 );
      local v = {};
      v[ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ 3 ] = clusters[ realClass ][ 3 ] - r + ( 2 * r * math.random() );
      local winner = abstract.KohonenLayer.compute( net, v );
      if ( winner == tx1 ) and ( realClass == 1 ) or ( winner == tx2 ) and ( realClass == 2 ) or ( winner == tx3 ) and ( realClass == 3 ) then
         hitsCount = hitsCount + 1;
         end
      end

   return ( hitsCount >= 90 );
end

-- Create neurons layer;
io.write( "Assembling Kohonen layer ( 3 neurons ) ... " ); io.flush();
inputs = 3;
neurons = 3;
net = abstract.KohonenLayer.create( inputs, neurons );
print( "[OK]" );

-- Train Kohonen layer;
io.write( "Training ... " ); io.flush();
clusters = { { 20, 20, 20 }, { 50, 50, 50 }, { 90, 90, 90 } };
trainVectors = genTrainVectors();
abstract.KohonenLayer.train( net, trainVectors, 100, 0.5, 0.001 );
-- Determine winners for all the classes;
tx1 = abstract.KohonenLayer.compute( net, clusters[ 1 ] );
tx2 = abstract.KohonenLayer.compute( net, clusters[ 2 ] );
tx3 = abstract.KohonenLayer.compute( net, clusters[ 3 ] );
print( "[OK]" );

-- Simulate Kohonen network;
print( "Choose the option to do:\n1) Visualize results\n2) Estimate time to fail\n3) Estimate survival function\n4) Estimate component importance\n5) Estimate time to fail distribution\n6) Estimate faults count distribution\n7) Exit" );
while true do
   op = tonumber( io.read() );
   if op ~= nil then if op >= 1 and op <= 7 then break end end
   end

if op == 1 then
   vectors = { { 10, 10, 10 }, { 55, 45, 56 }, { 90, 87, 92 }, { 15, 20, 25 }, { 43, 54, 58 }, { 88, 95, 79 } };
   for i = 1, #vectors do
      winner = abstract.KohonenLayer.compute( net, vectors[ i ] );
      print( "{ " .. vectors[ i ][ 1 ] .. ", " .. vectors[ i ][ 2 ] .. ", " .. vectors[ i ][ 3 ] .. " } => " .. winner );
      end

elseif op >= 2 and op <= 6 then
   op = op - 1;
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 5000.0 }, { 0.0, 15000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         distr = createDistribution( DISTR.EXP, x );
         manager = createInterruptManager( net.weights, distr, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager );
         y, dyl, dyh = reliability.estimateTimeToFail( 500, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager );
         closeId( distr );
         end

   elseif op >= 2 and op <= 5 then
      distr = createDistribution( DISTR.EXP, 0.0001 );
      manager = createInterruptManager( net.weights, distr, nil );
      engine = createSimulationEngine();
      appendInterruptManager( engine, manager );
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
            y, dyl, dyh = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager, 0 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 200, engine, testNetwork, { manager } );
         for i = 0, net.weightsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager );
      closeId( distr );
      end
   end

-- Close network;
abstract.KohonenLayer.destroy( net );

