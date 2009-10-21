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

-- Function for testing probabilistic network;
function testNetwork()
   local clusters = { { 0.1, 0.1, 0.1 }, { 0.9, 0.9, 0.9 } };
   local r = 0.05;

   -- Determine winner for first class;
   local x1 = abstract.ProbabilisticNetwork.compute( net, clusters[ 1 ] );
   local x2 = 3 - x1;

   local hitsCount = 0;
   for i = 1, 100 do
      local realClass = math.random( 2 );
      local v = {};
      v[ 1 ] = clusters[ realClass ][ 1 ] - r + ( 2 * r * math.random() );
      v[ 2 ] = clusters[ realClass ][ 2 ] - r + ( 2 * r * math.random() );
      v[ 3 ] = clusters[ realClass ][ 3 ] - r + ( 2 * r * math.random() );
      local winner = abstract.ProbabilisticNetwork.compute( net, v );
      if ( winner == x1 ) and ( realClass == 1 ) or ( winner == x2 ) and ( realClass == 2 ) then
         hitsCount = hitsCount + 1;
         end
      end

   return ( hitsCount >= 90 );
end

-- Function for estimating time to fail destribution of probabilistic network;
function estimateTimeToFailDestribution( times, lambda )
   local d = {};
   for i = 1, times do
      abstract.ProbabilisticNetwork.train( net, trainVectors );
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
      abstract.ProbabilisticNetwork.train( net, trainVectors );
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
      abstract.ProbabilisticNetwork.train( net, trainVectors );
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
      abstract.ProbabilisticNetwork.train( net, trainVectors );
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
      abstract.ProbabilisticNetwork.train( net, trainVectors );
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

inputs = 3;
classes = 2;
trainVectors = {
   { { 0.1, 0.2, 0.15 }, {1, 0} },
   { { 0.1, 0.1, 0.1 }, {1, 0} },
   { { 0.19, 0.12, 0.09 }, {1, 0} },
   { { 0.1, 0.07, 0.14 }, {1, 0} },
   { { 0.87, 0.9, 0.75 }, {0, 1} },
   { { 0.91, 0.91, 0.92 }, {0, 1} },
   { { 0.8, 0.9, 0.85 }, {0, 1} },
   { { 0.9, 0.8, 0.79 }, {0, 1} }
   };

-- Create neurons layer;
io.write( "Assembling probabilistic network ( 7 neurons ) ... " ); io.flush();
net = abstract.ProbabilisticNetwork.create( inputs, #trainVectors, classes );
print( "[OK]" );

-- Train probabilistic network;
io.write( "Training ... " ); io.flush();
abstract.ProbabilisticNetwork.train( net, trainVectors );
print( "[OK]" );

-- Print weights;
print( "Radial-basis layer:" );
for i = 1, #net.neurons[ 1 ] do
   w = getAbstractWeights( net.weights, inputs * ( i - 1 ), inputs );
   for j = 1, inputs do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

print( "Linear layer:" );
for i = 1, #net.neurons[ 2 ] do
   w = getAbstractWeights( net.weights, inputs * #net.neurons[ 1 ] + #net.neurons[ 1 ] * ( i - 1 ), #net.neurons[ 1 ] );
   for j = 1, #net.neurons[ 1 ] do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

-- Test results;
vectors = {
   {0.1, 0.1, 0.1},
   {0.9, 0.9, 0.9}
   };

for i = 1, #vectors do
   winner = abstract.ProbabilisticNetwork.compute( net, vectors[ i ] );
   print( "{ " .. vectors[ i ][ 1 ] .. ", " .. vectors[ i ][ 2 ] .. ", " .. vectors[ i ][ 3 ] .. " } => " .. winner );
   end

-- Simulate Hopfield network;
io.write( "Simulating ... " ); io.flush();
print( "[OK]" );

length = 10; x = {};
-- start = 0.00001; stop = 0.0001; delta = ( stop - start ) / ( length - 1 );
-- start = 0.0; stop = 20000.0; delta = ( stop - start ) / ( length - 1 );
start = 0.0; stop = 30000.0; delta = ( stop - start ) / ( length - 1 );
for i = 0, length - 1 do
   x[ i ] = start + i * delta;
   -- y, dyl, dyh = estimateTimeToFail( 1000, x[ i ] );
   -- y, dyl, dyh = estimateSurvivalFunction( 2000, 0.0001, x[ i ] );
   y, dyl, dyh = estimateWeightImportance( 2000, 0.0001, 17, x[ i ] );
   y1, dyl1, dyh1 = estimateWeightImportance( 2000, 0.0001, 24, x[ i ] );
   -- print( x[ i ] * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
   -- print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
   print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 );
   end;

-- destr = estimateFaultsCountDestribution( 1000, 0.0001 );
-- for i = 0, weightsCount do
   -- print( i .. " " .. destr[ i ] );
   -- end

-- Close network;
abstract.ProbabilisticNetwork.destroy( net );

