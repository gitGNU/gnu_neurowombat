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

require "abstract.GeneralizedRegressionNetwork";
require "reliability";

function visualizeResults( x1, x2, pointsCount )
   local value = x1;
   local delta = ( x2 - x1 ) / ( pointsCount - 1 );
   local x = {};
   print( "Real <=> predicted" );
   for i = 1, pointsCount do
      x[ 1 ] = 5.0 * math.sin( value - 5.0 * math.pi / 16.0 );
      x[ 2 ] = 5.0 * math.sin( value - 4.0 * math.pi / 16.0 );
      x[ 3 ] = 5.0 * math.sin( value - 3.0 * math.pi / 16.0 );
      x[ 4 ] = 5.0 * math.sin( value - 2.0 * math.pi / 16.0 );
      x[ 5 ] = 5.0 * math.sin( value - 1.0 * math.pi / 16.0 );
      local y = abstract.GeneralizedRegressionNetwork.compute( net, x );
      io.write( string.format( "%.3f <=> %.3f\n", 5.0 * math.sin( value ), y[ 1 ] ) );

      value = value + delta;
      end
   end

-- Function for testing generalized regression network;
function testNetwork()
   local errLimit = 1.0;
   local deltaTest = -5.0 * math.pi / 16.0;
   local x = {}; local y = {}; local target = {};
   for j = 1, 32 do
      x[ 1 ] = 5.0 * math.sin( deltaTest );
      x[ 2 ] = 5.0 * math.sin( deltaTest + math.pi / 16.0 );
      x[ 3 ] = 5.0 * math.sin( deltaTest + 2.0 * math.pi / 16.0 );
      x[ 4 ] = 5.0 * math.sin( deltaTest + 3.0 * math.pi / 16.0 );
      x[ 5 ] = 5.0 * math.sin( deltaTest + 4.0 * math.pi / 16.0 );
      target[ 1 ] = 5.0 * math.sin( deltaTest + 5.0 * math.pi / 16.0 );
      deltaTest = deltaTest + math.pi / 16.0;

      local y = abstract.GeneralizedRegressionNetwork.compute( net, x );
      if math.abs( target[ 1 ] - y[ 1 ] ) > errLimit then return false end
      end

   return true;
end

print( "API version: " .. apiVersion() );

inputs = 5;
spread = 0.7;
trainVectors = {};
deltaTest = -5.0 * math.pi / 16.0;
for i = 1, 32 do
   trainVectors[ i ] = { {}, {} };
   trainVectors[ i ][ 1 ][ 1 ] = 5.0 * math.sin( deltaTest );
   trainVectors[ i ][ 1 ][ 2 ] = 5.0 * math.sin( deltaTest + math.pi / 16.0 );
   trainVectors[ i ][ 1 ][ 3 ] = 5.0 * math.sin( deltaTest + 2.0 * math.pi / 16.0 );
   trainVectors[ i ][ 1 ][ 4 ] = 5.0 * math.sin( deltaTest + 3.0 * math.pi / 16.0 );
   trainVectors[ i ][ 1 ][ 5 ] = 5.0 * math.sin( deltaTest + 4.0 * math.pi / 16.0 );
   trainVectors[ i ][ 2 ][ 1 ] = 5.0 * math.sin( deltaTest + 5.0 * math.pi / 16.0 );
   deltaTest = deltaTest + math.pi / 16.0;
   end

-- Create neurons layer;
io.write( "Assembling generalized regression network ( 33 neurons ) ... " ); io.flush();
net = abstract.GeneralizedRegressionNetwork.create( inputs, #trainVectors, 1 );
print( "[OK]" );

-- Train generalized regression network;
io.write( "Training ... " ); io.flush();
abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
print( "[OK]" );

visualizeResults( 0, 2 * math.pi, 10 );

-- Simulate generalized regression network;
print( "Choose the option to do:\n1) Estimate time to fail\n2) Estimate survival function\n3) Estimate component importance\n4) Estimate time to fail destribution\n5) Estimate faults count destribution\n6) Exit" );
op = 0;
repeat
   local ops = io.read();
   if ops == "1" or ops == "2" or ops == "3" or ops == "4" or ops == "5" or ops == "6" then op = tonumber( ops ) end
   until op ~= 0

if op >= 1 and op <= 5 then
   print( "Start simulation..." );
   lengths = { 10, 10, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 1000.0 }, { 0.0, 1000.0 } };

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
            y, dyl, dyh = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager, 222 );
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
abstract.GeneralizedRegressionNetwork.destroy( net );

