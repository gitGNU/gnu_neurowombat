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

-- Function for testing probabilistic network;
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

-- Function for estimating time to fail destribution of probabilistic network;
function estimateTimeToFailDestribution( times, lambda )
   local d = {};
   for i = 1, times do
      abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, net.weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      while stepOverEngine( engine ) do
         if not testNetwork() then break end
         end

      d[ i ] = getCurrentTime( engine );

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   return d;
   end

-- Function for estimating time to fail of probabilistic network;
function estimateTimeToFail( times, lambda )
   local t = 0.0;
   local tsqr = 0.0;
   for i = 1, times do
      abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, net.weights );
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
      end

   t = t / times;
   tsqr = tsqr / times;
   local d = calcMeanCI( t, tsqr, times, 0.95 );
   return t, t - d, t + d;
   end

-- Function for estimating survival function of probabilistic network;
function estimateSurvivalFunction( times, lambda, t )
   local p = 0.0;
   for i = 1, times do
      abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, net.weights );
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
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for estimating faults count destribution of probabilistic network;
function estimateFaultsCountDestribution( times, lambda )
   local d = {};
   for i = 0, weightsCount do d[ i ] = 0 end
   for i = 1, times do
      abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, net.weights );
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

   for i = 0, weightsCount do d[ i ] = d[ i ] / times end
   return d;
   end

-- Function for estimating weight importance of probabilistic network;
function estimateWeightImportance( times, lambda, weightIndex, t )
   local p = 0.0;
   local m = 0; local s = -1;
   for i = 1, times do
      abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
      local destr = createExponentialDestribution( lambda );
      local manager = createAbstractWeightsManager( destr, net.weights );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      if t >= getFutureTime( engine ) then
         while stepOverEngine( engine ) do
            if t < getFutureTime( engine ) then break end
            end         
         end

      if testNetwork() then
         setAbstractWeights( net.weights, weightIndex, { 0.0 } );
         if not testNetwork() then p = p + 1.0 end
         end

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

print( "API version: " .. apiVersion() );

inputs = 5;
spread = 0.7;
trainVectors = {};
--   { { 4 }, { 1.5 } },
--   { { 5 }, { 3.6 } },
--   { { 6 }, { 6.7 } }
--   { { 1 }, { 0 } },
--   { { 2 }, { 1 } },
--   { { 3 }, { 2 } },
--   { { 4 }, { 3 } },
--   { { 5 }, { 2 } },
--   { { 6 }, { 1 } },
--   { { 7 }, { 2 } },
--   { { 8 }, { 1 } }
--   };

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
io.write( "Assembling generalized regression network ( 7 neurons ) ... " ); io.flush();
net = abstract.GeneralizedRegressionNetwork.create( inputs, #trainVectors, 1 );
print( "[OK]" );

-- Train probabilistic network;
io.write( "Training ... " ); io.flush();
abstract.GeneralizedRegressionNetwork.train( net, trainVectors, spread );
print( "[OK]" );

-- Print weights;
print( "Radial-basis layer:" );
for i = 1, #net.neurons[ 1 ] do
   w = getAbstractWeights( net.weights, ( inputs + 1 ) * ( i - 1 ), inputs + 1 );
   for j = 1, inputs + 1 do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

print( "Linear layer:" );
for i = 1, #net.neurons[ 2 ] do
   w = getAbstractWeights( net.weights, ( inputs + 1 ) * #net.neurons[ 1 ] + #net.neurons[ 1 ] * ( i - 1 ), #net.neurons[ 1 ] );
   for j = 1, #net.neurons[ 1 ] do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

-- Test results;
-- vectors = {
--   { 4.5 },
--   { 4.6 }
--   { 1 },
--   { 4 },
--   { 6 },
--   { 7 },
--   { 8 },
--   { 10 }
--   };

--for i = 1, #vectors do
--   setSignals( connectors, 0, vectors[ i ] );
--   computeAbstractNeurons( neurons[ 1 ], 1 );
--   computeAbstractNeurons( neurons[ 2 ], 1 );
--   x = getSignals( connectors, inputs + layers[ 1 ], layers[ 2 ] );
--   print( "{ " .. vectors[ i ][ 1 ] .. " } => " .. x[ 1 ] );
--   end

visualizeResults( 0, 2 * math.pi, 10 );

-- Simulate Hopfield network;
io.write( "Simulating ... " ); io.flush();
print( "[OK]" );

length = 10; x = {};
-- start = 0.00001; stop = 0.0001; delta = ( stop - start ) / ( length - 1 );
-- start = 0.0; stop = 1000.0; delta = ( stop - start ) / ( length - 1 );
start = 0.0; stop = 1000.0; delta = ( stop - start ) / ( length - 1 );
for i = 0, length - 1 do
   x[ i ] = start + i * delta;
   -- y, dyl, dyh = estimateTimeToFail( 1000, x[ i ] );
   -- y, dyl, dyh = estimateSurvivalFunction( 2000, 0.0001, x[ i ] );
   y, dyl, dyh = estimateWeightImportance( 2000, 0.0001, 0, x[ i ] );
   y1, dyl1, dyh1 = estimateWeightImportance( 2000, 0.0001, 222, x[ i ] );
   -- print( x[ i ] * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
   -- print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
   print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 );
   end;

-- destr = estimateFaultsCountDestribution( 1000, 0.0001 );
-- for i = 0, weightsCount do
   -- print( i .. " " .. destr[ i ] );
   -- end

-- Close network;
abstract.GeneralizedRegressionNetwork.destroy( net );

