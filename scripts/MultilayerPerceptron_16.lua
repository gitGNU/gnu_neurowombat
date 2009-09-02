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
      setSignals( connectors, 1, x );
      computeAbstractNeurons( neurons, 1 );
      y = getSignals( connectors, connectorsCount - 1, 1 );
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

      setSignals( connectors, 1, x );
      computeAbstractNeurons( neurons, 1 );
      y = getSignals( connectors, connectorsCount - 1, 1 );
      if math.abs( target[ 1 ] - y[ 1 ] ) > errLimit then return false end
      end

   return true;
   end

-- Function for estimating time to fail of multilayer perceptron;
function estimateTimeToFail( times, lambda )
   local t = 0.0;
   local tsqr = 0.0;
   for i = 1, times do
      trainNetwork( 0.5, 0.5, 0.01, 10000 );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      repeat
         if not testNetwork() then
            local x = getCurrentTime( engine );
            t = t + x;
            tsqr = tsqr + x * x;
            break;
            end

         until not stepOverEngine( engine )

      closeId( engine );
      closeId( manager );
      closeId( destr );
      io.write( i .. "" ); io.flush();
      end

   print( "end" );
   t = t / times;
   tsqr = tsqr / times;
   local d = calcMeanCI( t, tsqr, times, 0.95 );
   return t, t - d, t + d;
   end

-- Function for estimating survival function of multilayer perceptron;
function estimateSurvivalFunction( times, lambda, t )
   local p = 0.0;
   for i = 1, times do
      trainNetwork( 0.5, 0.5, 0.01, 10000 );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      local tIsTooLarge = true;
      repeat
         if getCurrentTime( engine ) >= t then
            if testNetwork() then p = p + 1.0 end
            tIsTooLarge = false;
            break;
            end

         until not stepOverEngine( engine )

      if tIsTooLarge then
         if testNetwork() then p = p + 1.0 end
         end;

      closeId( engine );
      closeId( manager );
      closeId( destr );
      io.write( i .. "" ); io.flush();
      end

   print( "end" );
   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for estimating faults count destribution of multilayer perceptron;
function estimateFaultsCountDestribution( times, lambda )
   local d = {};
   for i = 0, weightsCount do d[ i ] = 0 end
   for i = 1, times do
      trainNetwork( 0.5, 0.5, 0.01, 10000 );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      failesCount = 0;
      while stepOverEngine( engine ) do
         failesCount = failesCount + 1;
         if not testNetwork() then break end

         end

      d[ failesCount ] = d[ failesCount ] + 1;

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   -- for i = 0, weightsCount do d[ i ] = d[ i ] / times end
   return d;
   end

-- Function for estimating weight importance of multilayer perceptron;
function estimateWeightImportance( times, lambda, weightIndex, t )
   local p = 0.0;
   local m = 0; local s = -1;
   for i = 1, times do
      trainNetwork( 0.5, 0.5, 0.01, 10000 );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      if t >= getFutureTime( engine ) then
         while stepOverEngine( engine ) do
            if t < getFutureTime( engine ) then break end
            end         
         end

      if testNetwork() then
         setAbstractWeights( weights, weightIndex, { 0.0 } );
         if not testNetwork() then p = p + 1.0 end
         end

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for training multilayer perceptron;
function trainNetwork( err, damping, speed, epochsLimit )
   local w = {}
   for i = 1, weightsCount do
      w[ i ] = 1.0 - 2.0 * math.random();
      end

   setAbstractWeights( weights, 0, w );

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

      setSignals( connectors, 1, x );
      trainBPAbstractNeurons( neurons, layers, target, damping, speed );
      computeAbstractNeurons( neurons, 1 );
      local y = getSignals( connectors, connectorsCount - 1, 1 );

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

            setSignals( connectors, 1, x );
            computeAbstractNeurons( neurons, 1 );
            y = getSignals( connectors, connectorsCount - 1, 1 );
            if math.abs( maxE ) < math.abs( target[ 1 ] - y[ 1 ] ) then
               maxE = target[ 1 ] - y[ 1 ];
               end
            end

         if math.abs( maxE ) <= err then break; end
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

print( "API version: " .. apiVersion() );

-- Create activation functions;
linActFunc = createLinearActFunc( 1.0, 0.0 );
sigActFunc = createSigmoidActFunc();

-- Set neurons count in each layer;
inputs = 5;
layers = { 15, 1 };
actFuncs = { sigActFunc, linActFunc };

-- Calculate total neurons, connectors and weights count;
neuronsCount = 0;
connectorsCount = 1 + inputs; -- The first connector is reserved for 1.0 signal;
weightsCount = 0;

j = inputs;
for i = 1, #layers do
   neuronsCount = neuronsCount + layers[ i ];
   connectorsCount = connectorsCount + layers[ i ];
   weightsCount = weightsCount + layers[ i ] * ( j + 1 );
   j = layers[ i ];
   end

-- Create abstract components for multilayer perceptron;
io.write( "Creating abstract components ... " ); io.flush();
connectors = createAbstractConnectors( connectorsCount );
weights = createAbstractWeights( weightsCount );

-- Set 1.0 signal for the first connector;
setSignals( connectors, 0, { 1.0 } );
print( "[OK]" );

-- Create neuron layers;
io.write( "Assembling multilayer perceptron ... " ); io.flush();
neurons = {};
neuronsBaseIndex = 1;
weightsBaseIndex = 0;
connectorsBaseIndex = 1;
prevNeuronsCount = inputs;
for i = 1, #layers do
   inputConnectors = {};
   inputConnectors[ 1 ] = 0;
   for k = 2, prevNeuronsCount + 1 do
      inputConnectors[ k ] = connectorsBaseIndex;
      connectorsBaseIndex = connectorsBaseIndex + 1;
      end

   for j = 1, layers[ i ] do
      neurons[ neuronsBaseIndex ] = createAbstractNeuron(
         prevNeuronsCount + 1,
         inputConnectors,
         connectors, connectorsBaseIndex,
         weights, weightsBaseIndex,
         0, 0,
         0, 0,
         actFuncs[ i ]
         );

      neuronsBaseIndex = neuronsBaseIndex + 1;
      weightsBaseIndex = weightsBaseIndex + prevNeuronsCount + 1;
      connectorsBaseIndex = connectorsBaseIndex + 1;
      end

   connectorsBaseIndex = connectorsBaseIndex - layers[ i ];
   prevNeuronsCount = layers[ i ];
   end

print( "[OK]" );

-- Train multilayer perceptron;
io.write( "Training ... " ); io.flush();
epochs = trainNetwork( 0.5, 0.5, 0.01, 10000 );
print( epochs .. " [OK]" );

visualizeResults( 0, 2 * math.pi, 10 );

length = 7; x = {};
-- start = 0.00001; stop = 0.0001; delta = ( stop - start ) / ( length - 1 );
-- start = 0.0; stop = 1000.0; delta = ( stop - start ) / ( length - 1 );
start = 0.0; stop = 1000.0; delta = ( stop - start ) / ( length - 1 );
for i = 0, length - 1 do
   x[ i ] = start + i * delta;
   -- y, dyl, dyh = estimateTimeToFail( 121, x[ i ] );
   -- y, dyl, dyh = estimateSurvivalFunction( 121, 0.0001, x[ i ] );
   y, dyl, dyh = estimateWeightImportance( 121, 0.0001, 1, x[ i ] );
   y1, dyl1, dyh1 = estimateWeightImportance( 121, 0.0001, 91, x[ i ] );
   -- print( x[ i ] * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
   -- print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
   print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 );
   end;

-- destr = estimateFaultsCountDestribution( 500, 0.0001 );
-- for i = 0, weightsCount do
   -- print( i .. " " .. destr[ i ] );
   -- end

-- Close objects;
for i = 1, neuronsCount do
   closeId( neurons[ i ] );
   end

closeId( weights );
closeId( connectors );
closeId( sigActFunc );
closeId( linActFunc );

