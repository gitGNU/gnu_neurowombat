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
   local index = 0;
   local max = x[ 0 ];
   for i = 1, N - 1 do
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
   w[ 0 ] = N * math.exp( 1.0 );
   for i = 1, K do
      w[ i ] = getInitWeight( vectors, i );
      end

   for i = 0, N - 1 do
      setAbstractWeights( neurons[ i ], w, K + 1 );
      end

   -- Setup activity parameters;
   local c = {};
   for i = 0, N - 1 do c[ i ] = 1.0 / N; end

   local L = 0.001;
   local x = {};
   for i = 0, epochs - 1 do
      -- Compute;
      setSignals( connectors, 1, vectors[ i % #vectors + 1 ] );
      computeAbstractNeurons( neurons, N, 1 );
      x = getSignals( connectors, 1 + K, N );
      winner = getWinner( x );

      for j = 0, N - 1 do
         w = getAbstractWeights( neurons[ j ] );

         if j == winner then
            -- Modify weights;
            for k = 1, K do
               w[ k ] = w[ k ] + speed * ( vectors[ i % #vectors + 1 ][ k ] - w[ k ] );
               end

            c[ j ] = c[ j ] + L * ( 1.0 - c[ j ] );
         else
            c[ j ] = c[ j ] * ( 1.0 - L );
         end

         w[ 0 ] = math.exp( 1.0 - math.log( c[ j ] ) );
         setAbstractWeights( neurons[ j ], w, K + 1 );
         end
      end
end

print( "API version: " .. apiVersion() );

-- Create abstract components for Kohonen layer;
N = 2;
K = 2;
io.write( "Creating abstract components ... " );
actFunc = createLinearActFunc( 1.0, 0.0 );
connectors = createAbstractConnectors( 1 + K + N );

-- Set 1.0 signal for 0-connector;
setSignals( connectors, 0, { 1.0 } );
print( "[OK]" );

-- Create neurons layer;
io.write( "Assembling Kohonen layer ( 2 neurons ) ... " );
neurons = {};
for i = 0, N - 1 do
   inputConnectors = {};
   inputConnectors[ 0 ] = 0;
   for j = 1, K do
      inputConnectors[ j ] = j;
      end

   neurons[ i ] = createAbstractNeuron(
      K + 1,
      inputConnectors,
      connectors, 1 + K + i,
      0, 0,
      0, 0,
      actFunc
      );
   end

print( "[OK]" );

-- Train Kohonen layer;
io.write( "Training ... " );
vectors = { { 0.1, 0.2 }, { 0.8, 0.9 }, { 0.1, 0.1 }, { 0.9, 0.8 } };
trainNetwork( vectors, 40, 0.5 )
print( "[OK]" );

-- Print weights;
for i = 0, N - 1 do
   w = getAbstractWeights( neurons[ i ] );
   for j = 0, K do
      print( "w[ " .. i .. " ][ " .. j .. " ] = " .. w[ j ] );
      end
   end

-- Test results;
vectors = { { 0.1, 0.1 }, { 0.8, 0.8 }, { 0.0, 0.2 }, { 0.8, 0.9 } };

for i = 1, #vectors do
   setSignals( connectors, 1, vectors[ i ] );
   computeAbstractNeurons( neurons, N, 1 );
   x = getSignals( connectors, 1 + K, N );
   print( "{ " .. vectors[ i ][ 1 ] .. ", " .. vectors[ i ][ 2 ] .. " } => " .. getWinner( x ) );
   end

-- Close objects;
for i = 0, N - 1 do
   closeId( neurons[ i ] );
   end

closeId( connectors );
closeId( actFunc );

