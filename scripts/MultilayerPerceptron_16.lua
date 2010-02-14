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


require "abstract.MultilayerPerceptron";
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
      local y = abstract.MultilayerPerceptron.compute( net, x );
      io.write( string.format( "%.3f <=> %.3f\n", 5.0 * math.sin( value ), y[ 1 ] ) );

      value = value + delta;
      end
   end

-- Function for testing multilayer perceptron;
function testNetwork()
   local errLimit = 1.0;
   for i = 1, #trainVectors do
      local y = abstract.MultilayerPerceptron.compute( net, trainVectors[ i ][ 1 ] );
      if math.abs( trainVectors[ i ][ 2 ][ 1 ] - y[ 1 ] ) > errLimit then return false end
      end

   return true;
   end


function retrainNetwork()
   abstract.MultilayerPerceptron.train( net, trainVectors, 0.5, 0.5, 0.01, 10000 );
   end

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

-- Create activation functions;
linActFunc = createActFunc( ACT_FUNC.LINEAR, 1.0, 0.0 );
sigActFunc = createActFunc( ACT_FUNC.SIGMOID, 1.0 );

-- Set neurons count in each layer;
inputs = 5;
layers = { 15, 1 };

-- Create neuron layers;
io.write( "Assembling multilayer perceptron ( 16 neurons ) ... " ); io.flush();
net = abstract.MultilayerPerceptron.create( inputs, layers, { sigActFunc, linActFunc } );
print( "[OK]" );

-- Train multilayer perceptron;
io.write( "Training ... " ); io.flush();
epochs = abstract.MultilayerPerceptron.train( net, trainVectors, 0.5, 0.5, 0.01, 10000 );
print( epochs .. " [OK]" );

-- Simulate multilayer perceptron network;
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
   lengths = { 10, 6, 10 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 1000.0 }, { 0.0, 1000.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         distr = createDistribution( DISTR.EXP, x );
         manager = createInterruptManager( net.weights, distr, retrainNetwork );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager );
         y, dyl, dyh = reliability.estimateTimeToFail( 121, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager );
         closeId( distr );
         end

   elseif op >= 2 and op <= 5 then
      distr = createDistribution( DISTR.EXP, 0.0001 );
      manager = createInterruptManager( net.weights, distr, retrainNetwork );
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
            y, dyl, dyh = reliability.estimateComponentImportance( x, 121, engine, testNetwork, manager, 91 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 200, engine, testNetwork, { manager } );
         for i = 0, net.weightsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager );
      closeId( distr );
      end
   end

-- Close objects;
abstract.MultilayerPerceptron.destroy( net );
closeId( sigActFunc );
closeId( linActFunc );

