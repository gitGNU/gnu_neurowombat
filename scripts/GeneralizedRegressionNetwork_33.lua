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

-- Simulate generalized regression network;
print( "Choose the option to do:\n1) Visualize results\n2) Estimate time to fail\n3) Estimate survival function\n4) Estimate component importance\n5) Estimate time to fail distribution\n6) Estimate faults count distribution\n7) Exit" );
while true do
   op = tonumber( io.read() );
   if op ~= nil then if op >= 1 and op <= 7 then break end end
   end

if op == 1 then
   visualizeResults( 0, 2 * math.pi, 10 );

elseif op >= 2 and op <= 6 then
   op = op - 1;
   print( "Start simulation..." );
   lengths = { 10, 7, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 1000.0 }, { 0.0, 1000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         distr = createDistribution( DISTR.EXP, x );
         manager = createInterruptManager( net.weights, distr, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager );
         y, dyl, dyh = reliability.estimateTimeToFail( 1000, engine, testNetwork );
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
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 1000, engine, testNetwork );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 3 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateComponentImportance( x, 2000, engine, testNetwork, manager, 200 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 500, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 500, engine, testNetwork, { manager } );
         for i = 0, net.weightsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager );
      closeId( distr );
      end
   end

-- Close network;
abstract.GeneralizedRegressionNetwork.destroy( net );

