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

require "abstract.ProbabilisticNetwork";
require "reliability";

function genTrainVectors()
   local r = 5;
   local v = {};
   for i = 1, 15 do
      local realClass = math.random( 3 );
      v[ i ] = { {}, {} };
      v[ i ][ 1 ][ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ i ][ 1 ][ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ i ][ 1 ][ 3 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ i ][ 2 ][ 1 ] = 0;
      v[ i ][ 2 ][ 2 ] = 0;
      v[ i ][ 2 ][ 3 ] = 0;
      v[ i ][ 2 ][ realClass ] = 1;
      end

   return v;
end

-- Function for testing probabilistic network;
function testNetwork()
   local r = 5;
   local hitsCount = 0;
   for i = 1, 100 do
      local realClass = math.random( 3 );
      local v = {};
      v[ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ 3 ] = clusters[ realClass ][ 3 ] - r + ( 2 * r * math.random() );
      local winner = abstract.ProbabilisticNetwork.compute( net, v );
      if winner == realClass then
         hitsCount = hitsCount + 1;
         end
      end

   return ( hitsCount >= 90 );
end

print( "API version: " .. apiVersion() );

inputs = 3;
classes = 3;
clusters = { { 20, 20, 20 }, { 50, 50, 50 }, { 90, 90, 90 } };
trainVectors = genTrainVectors();

-- Create neurons layer;
io.write( "Assembling probabilistic network ( 18 neurons ) ... " ); io.flush();
net = abstract.ProbabilisticNetwork.create( inputs, #trainVectors, classes );
print( "[OK]" );

-- Train probabilistic network;
io.write( "Training ... " ); io.flush();
abstract.ProbabilisticNetwork.train( net, trainVectors );
print( "[OK]" );

-- Test results;
vectors = { { 10, 10, 10 }, { 55, 45, 56 }, { 90, 87, 92 }, { 15, 20, 25 }, { 43, 54, 58 }, { 88, 95, 79 } };
for i = 1, #vectors do
   winner = abstract.ProbabilisticNetwork.compute( net, vectors[ i ] );
   print( "{ " .. vectors[ i ][ 1 ] .. ", " .. vectors[ i ][ 2 ] .. ", " .. vectors[ i ][ 3 ] .. " } => " .. winner );
   end

-- Simulate probabilistic network;
print( "Choose the option to do:\n1) Estimate time to fail\n2) Estimate survival function\n3) Estimate component importance\n4) Estimate time to fail destribution\n5) Estimate faults count destribution\n6) Exit" );
op = 0;
repeat
   local ops = io.read();
   if ops == "1" or ops == "2" or ops == "3" or ops == "4" or ops == "5" or ops == "6" then op = tonumber( ops ) end
   until op ~= 0

if op >= 1 and op <= 5 then
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 8000.0 }, { 0.0, 8000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         destr = createExponentialDestribution( x );
         manager = createAbstractWeightsManager( destr, net.weights, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager );
         y, dyl, dyh = reliability.estimateTimeToFail( 1000, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager );
         closeId( destr );
         end

   elseif op >= 2 and op <= 5 then
      destr = createExponentialDestribution( 0.0001 );
      manager = createAbstractWeightsManager( destr, net.weights, nil );
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
            y, dyl, dyh = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager, 89 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDestribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDestribution( 200, engine, testNetwork, manager );
         for i = 0, net.weightsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end
   end

-- Close network;
abstract.ProbabilisticNetwork.destroy( net );

