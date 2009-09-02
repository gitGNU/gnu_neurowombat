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


-- Kohonen layer script ( 2 neurons ). The goal of training is to 
-- clusterize 4 base vectors. Then the task is to classify 4 test 
-- vectors.

-- Function for neurons competition;
function getWinner( x )
   local index = 1;
   local max = x[ 1 ];
   for i = 2, #x do
      if x[ i ] > max then
         max = x[ i ];
         index = i;
         end
      end

   return index;
end

-- Function for calculating initial weights;
function getInitWeight( vectors, i )
   local min = vectors[ 1 ][ i ];
   local max = min;
   for j = 2, #vectors do
      if vectors[ j ][ i ] > max then max = vectors[ j ][ i ]; end
      if vectors[ j ][ i ] < min then min = vectors[ j ][ i ]; end
      end

   return ( max + min ) / 2.0;
end

-- Function for training Kohonen layer;
function trainNetwork( vectors, epochs, speed )
   -- Initial setup;
   local w = {};
   w[ 1 ] = N * math.exp( 1.0 );
   for i = 2, K + 1 do
      w[ i ] = getInitWeight( vectors, i - 1 );
      end

   for i = 1, N do
      setAbstractWeights( neurons[ i ], w );
      end

   -- Setup activity parameters;
   local c = {};
   for i = 1, N do c[ i ] = 1.0 / N; end

   local L = 0.001;
   local x = {};
   for i = 0, epochs - 1 do
      -- Compute;
      setSignals( connectors, 1, vectors[ i % #vectors + 1 ] );
      computeAbstractNeurons( neurons, 1 );
      x = getSignals( connectors, 1 + K, N );
      winner = getWinner( x );

      for j = 1, N do
         w = getAbstractWeights( weights, ( K + 1 ) * ( j - 1 ), K + 1 );

         if j == winner then
            -- Modify weights;
            for k = 2, K + 1 do
               w[ k ] = w[ k ] + speed * ( vectors[ i % #vectors + 1 ][ k - 1 ] - w[ k ] );
               end

            c[ j ] = c[ j ] + L * ( 1.0 - c[ j ] );
         else
            c[ j ] = c[ j ] * ( 1.0 - L );
         end

         w[ 1 ] = math.exp( 1.0 - math.log( c[ j ] ) );
         setAbstractWeights( neurons[ j ], w );
         end
      end
   end

-- Function for testing Kohonen layer;
function testNetwork()
   local clusters = { { 0.1, 0.1, 0.1 }, { 0.9, 0.9, 0.9 } };
   local r = 0.05;

   -- Determine winner for first class;
   setSignals( connectors, 1, clusters[ 1 ] );
   computeAbstractNeurons( neurons, 1 );
   local x1 = getWinner( getSignals( connectors, 1 + K, N ) );
   local x2 = 3 - x1;

   local hitsCount = 0;
   for i = 1, 100 do
      local realClass = math.random( 2 );
      local v = {};
      v[ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ 3 ] = clusters[ realClass ][ 3 ] - r + ( 2 * r * math.random() );
      setSignals( connectors, 1, v );
      computeAbstractNeurons( neurons, 1 );
      local winner = getWinner( getSignals( connectors, 1 + K, N ) );
      if ( winner == x1 ) and ( realClass == 1 ) or ( winner == x2 ) and ( realClass == 2 ) then
         hitsCount = hitsCount + 1;
         end
      end

   return ( hitsCount >= 90 );
end

-- Function for estimating time to fail of Kohonen layer;
function estimateTimeToFail( times, lambda )
   local t = 0.0;
   local tsqr = 0.0;
   for i = 1, times do
      trainNetwork( trainVectors, 100, 0.5 );
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
      end

   t = t / times;
   tsqr = tsqr / times;
   local d = calcMeanCI( t, tsqr, times, 0.95 );
   return t, t - d, t + d;
   end

-- Function for estimating survival function of Kohonen layer;
function estimateSurvivalFunction( times, lambda, t )
   local p = 0.0;
   for i = 1, times do
      trainNetwork( trainVectors, 100, 0.5 );
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
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for estimating faults count destribution of Kohonen layer;
function estimateFaultsCountDestribution( times, lambda )
   local d = {};
   for i = 0, ( K + 1 ) * N do d[ i ] = 0 end
   for i = 1, times do
      trainNetwork( trainVectors, 100, 0.5 );
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

   for i = 0, ( K + 1 ) * N do d[ i ] = d[ i ] / times end
   return d;
   end

-- Function for estimating weight importance of Kohonen layer;
function estimateWeightImportance( times, lambda, weightIndex, t )
   local p = 0.0;
   local m = 0; local s = -1;
   for i = 1, times do
      trainNetwork( trainVectors, 100, 0.5 );
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

print( "API version: " .. apiVersion() );

-- Create abstract components for Kohonen layer;
N = 2;
K = 3;
io.write( "Creating abstract components ... " ); io.flush();
actFunc = createLinearActFunc( 1.0, 0.0 );
connectors = createAbstractConnectors( 1 + K + N );
weights = createAbstractWeights( ( K + 1 ) * N );

-- Set 1.0 signal for 0-connector;
setSignals( connectors, 0, { 1.0 } );
print( "[OK]" );

-- Create neurons layer;
io.write( "Assembling Kohonen layer ( 2 neurons ) ... " ); io.flush();
inputConnectors = { 0 };
for i = 1, K do
   inputConnectors[ i + 1 ] = i;
   end

neurons = {};
for i = 0, N - 1 do
   neurons[ i + 1 ] = createAbstractNeuron(
      K + 1,
      inputConnectors,
      connectors, 1 + K + i,
      weights, ( K + 1 ) * i,
      0, 0,
      0, 0,
      actFunc
      );
   end

print( "[OK]" );

-- Train Kohonen layer;
io.write( "Training ... " ); io.flush();
trainVectors = {
   { 0.1, 0.2, 0.15 }, { 0.8, 0.9, 0.85 },
   { 0.1, 0.1, 0.1 }, { 0.9, 0.8, 0.79 },
   { 0.19, 0.12, 0.09 }, { 0.87, 0.9, 0.75 },
   { 0.91, 0.91, 0.92 }, { 0.1, 0.07, 0.14 }
   };
trainNetwork( trainVectors, 100, 0.5 );
print( "[OK]" );

-- Print weights;
for i = 1, N do
   --w = getAbstractWeights( weights, ( K + 1 ) * ( i - 1 ), K + 1 );
   w = getAbstractWeights( neurons[ i ] );
   for j = 1, K + 1 do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

-- Test results;
vectors = { { 0.1, 0.1, 0.1 }, { 0.8, 0.8, 0.8 }, { 0.0, 0.2, 0.1 }, { 0.8, 0.9, 0.75 } };

for i = 1, #vectors do
   setSignals( connectors, 1, vectors[ i ] );
   computeAbstractNeurons( neurons, 1 );
   x = getSignals( connectors, 1 + K, N );
   print( "{ " .. vectors[ i ][ 1 ] .. ", " .. vectors[ i ][ 2 ] .. ", " .. vectors[ i ][ 3 ] .. " } => " .. getWinner( x ) );
   end

-- Simulate Hopfield network;
io.write( "Simulating ... " ); io.flush();
-- timeToFail, dtl, dth = estimateTimeToFail( 200, 0.00001 );
-- p, dpl, dph = estimateSurvivalFunction( 200, 0.00001, 20000 );
-- print( "[OK]" );
-- print( "timeToFail = " .. timeToFail .. "; CI = [" .. dtl .. ", " .. dth .. "]" );
-- print( "p = " .. p .. "; CI = [" .. dpl .. ", " .. dph .. "]" );
print( "[OK]" );

length = 10; x = {};
-- start = 0.00001; stop = 0.0001; delta = ( stop - start ) / ( length - 1 );
-- start = 0.0; stop = 5000.0; delta = ( stop - start ) / ( length - 1 );
start = 0.0; stop = 30000.0; delta = ( stop - start ) / ( length - 1 );
for i = 0, length - 1 do
   x[ i ] = start + i * delta;
   -- y, dyl, dyh = estimateTimeToFail( 500, x[ i ] );
   -- y, dyl, dyh = estimateSurvivalFunction( 2000, 0.0001, x[ i ] );
   y, dyl, dyh = estimateWeightImportance( 2000, 0.0001, 0, x[ i ] );
   y1, dyl1, dyh1 = estimateWeightImportance( 2000, 0.0001, 4, x[ i ] );
   -- print( x[ i ] * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
   -- print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
   print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 );
   end;

-- destr = estimateFaultsCountDestribution( 1000, 0.0001 );
-- for i = 0, ( K + 1 ) * N do
   -- print( i .. " " .. destr[ i ] );
   -- end

-- Close objects;
for i = 1, N do
   closeId( neurons[ i ] );
   end

closeId( weights );
closeId( connectors );
closeId( actFunc );

