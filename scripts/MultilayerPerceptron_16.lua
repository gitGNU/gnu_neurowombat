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


-- Multilayer perceptron script ( 21 neurons ). The goal of training 
-- is to predict periodic function values.

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
      setSignals( net.connectors, 1, x );
      computeAbstractNeurons( net.neurons, 1 );
      local y = getSignals( net.connectors, net.connectorsCount - 1, 1 );
      io.write( string.format( "%.3f <=> %.3f\n", 5.0 * math.sin( value ), y[ 1 ] ) );

      value = value + delta;
      end
   end

-- Function for testing multilayer perceptron;
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

      setSignals( net.connectors, 1, x );
      computeAbstractNeurons( net.neurons, 1 );
      local y = getSignals( net.connectors, net.connectorsCount - 1, 1 );
      if math.abs( target[ 1 ] - y[ 1 ] ) > errLimit then return false end
      end

   return true;
   end

-- Function for training multilayer perceptron;
function trainNetwork( err, damping, speed, epochsLimit )
   local w = {}
   for i = 1, net.weightsCount do
      w[ i ] = 1.0 - 2.0 * math.random();
      end

   setAbstractWeights( net.weights, 0, w );

   local x = {};
   local target = {};
   local delta = -5.0 * math.pi / 16.0;
   local i = 0;
   local epochs = 0;
   while epochs < epochsLimit do
      x[ 1 ] = 5.0 * math.sin( delta );
      x[ 2 ] = 5.0 * math.sin( delta + math.pi / 16.0 );
      x[ 3 ] = 5.0 * math.sin( delta + 2.0 * math.pi / 16.0 );
      x[ 4 ] = 5.0 * math.sin( delta + 3.0 * math.pi / 16.0 );
      x[ 5 ] = 5.0 * math.sin( delta + 4.0 * math.pi / 16.0 );
      target[ 1 ] = 5.0 * math.sin( delta + 5.0 * math.pi / 16.0 );
      delta = delta + math.pi / 16.0;

      setSignals( net.connectors, 1, x );
      trainBPAbstractNeurons( net.neurons, layers, target, damping, speed );
      computeAbstractNeurons( net.neurons, 1 );
      local y = getSignals( net.connectors, net.connectorsCount - 1, 1 );

      if math.abs( target[ 1 ] - y[ 1 ] ) <= err then
         -- Complete test;
         local maxE = 0.0;
         local deltaTest = -5.0 * math.pi / 16.0;
         for j = 1, 32 do
            x[ 1 ] = 5.0 * math.sin( deltaTest );
            x[ 2 ] = 5.0 * math.sin( deltaTest + math.pi / 16.0 );
            x[ 3 ] = 5.0 * math.sin( deltaTest + 2.0 * math.pi / 16.0 );
            x[ 4 ] = 5.0 * math.sin( deltaTest + 3.0 * math.pi / 16.0 );
            x[ 5 ] = 5.0 * math.sin( deltaTest + 4.0 * math.pi / 16.0 );
            target[ 1 ] = 5.0 * math.sin( deltaTest + 5.0 * math.pi / 16.0 );
            deltaTest = deltaTest + math.pi / 16.0;

            setSignals( net.connectors, 1, x );
            computeAbstractNeurons( net.neurons, 1 );
            y = getSignals( net.connectors, net.connectorsCount - 1, 1 );
            if maxE < math.abs( target[ 1 ] - y[ 1 ] ) then
               maxE = math.abs( target[ 1 ] - y[ 1 ] );
               end
            end

         if maxE <= err then break; end
         end

      if i < 32 then
         i = i + 1;
      else
         i = 0;
         delta = -5.0 * math.pi / 16.0;
         end

      epochs = epochs + 1;
      end

   return epochs;
   end


function retrainNetwork()
   trainNetwork( 0.5, 0.5, 0.01, 10000 );
   end

print( "API version: " .. apiVersion() );

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
epochs = trainNetwork( 0.5, 0.5, 0.01, 10000 );
print( epochs .. " [OK]" );

visualizeResults( 0, 2 * math.pi, 10 );

-- Simulate multilayer perceptron network;
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
         manager = createAbstractWeightsManager( destr, net.weights, retrainNetwork );
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
      manager = createAbstractWeightsManager( destr, net.weights, retrainNetwork );
      engine = createSimulationEngine();
      appendInterruptManager( engine, manager );
      if op == 2 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 121, engine, testNetwork );
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

-- Close objects;
abstract.MultilayerPerceptron.destroy( net );
closeId( sigActFunc );
closeId( linActFunc );

