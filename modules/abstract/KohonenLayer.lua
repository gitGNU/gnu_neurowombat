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


module( ..., package.seeall )


-- Function for neurons competition;
local function getWinner( x )
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
local function getInitWeight( vectors, i )
   local min = vectors[ 1 ][ i ];
   local max = min;
   for j = 2, #vectors do
      if vectors[ j ][ i ] > max then max = vectors[ j ][ i ]; end
      if vectors[ j ][ i ] < min then min = vectors[ j ][ i ]; end
      end

   return ( max + min ) / 2.0;
end


function create( inputs, neurons )
   local network = {};
   network.inputs = inputs;
   network.neuronsCount = neurons;
   network.neurons = {};
   network.activatorsCount = 0;
   network.activators = 0;
   network.actFunc = createLinearActFunc( 1.0, 0.0 );
   network.buffersCount = 0;
   network.buffers = 0;
   network.connectorsCount = 1 + inputs + neurons;
   network.connectors = createAbstractConnectors( network.connectorsCount );
   network.weightsCount = ( inputs + 1 ) * neurons;
   network.weights = createAbstractWeights( network.weightsCount );
   network.processor = createAbstractWeightedSumProcessor();

   -- Set 1.0 signal for 0-connector;
   setSignals( network.connectors, 0, { 1.0 } );

   -- Create neurons layer;
   local inputConnectors = { 0 };
   for i = 1, inputs do
      inputConnectors[ i + 1 ] = i;
      end

   for i = 0, neurons - 1 do
      network.neurons[ i + 1 ] = createAbstractNeuron(
         inputs + 1,
         inputConnectors,
         network.connectors, 1 + inputs + i,
         network.weights, ( inputs + 1 ) * i,
         0, 0,
         network.processor,
         network.actFunc
         );
      end

   return network;
   end


function destroy( network )
   for i = 1, #network.neurons do
      closeId( network.neurons[ i ] );
      end

   closeId( network.processor );
   closeId( network.weights );
   closeId( network.connectors );
   closeId( network.actFunc );
   end;


function train( network, vectors, epochs, speed )
   -- Initial setup;
   local w = {};
   w[ 1 ] = network.neuronsCount * math.exp( 1.0 );
   for i = 2, network.inputs + 1 do
      w[ i ] = getInitWeight( vectors, i - 1 );
      end

   for i = 1, network.neuronsCount do
      setAbstractWeights( network.neurons[ i ], w );
      end

   -- Setup activity parameters;
   local c = {};
   for i = 1, network.neuronsCount do c[ i ] = 1.0 / network.neuronsCount; end

   local L = 0.001;
   local x = {};
   for i = 0, epochs - 1 do
      -- Compute;
      winner = compute( network, vectors[ i % #vectors + 1 ] );

      for j = 1, network.neuronsCount do
         w = getAbstractWeights( network.weights, ( network.inputs + 1 ) * ( j - 1 ), network.inputs + 1 );

         if j == winner then
            -- Modify weights;
            for k = 2, network.inputs + 1 do
               w[ k ] = w[ k ] + speed * ( vectors[ i % #vectors + 1 ][ k - 1 ] - w[ k ] );
               end

            c[ j ] = c[ j ] + L * ( 1.0 - c[ j ] );
         else
            c[ j ] = c[ j ] * ( 1.0 - L );
         end

         w[ 1 ] = math.exp( 1.0 - math.log( c[ j ] ) );
         setAbstractWeights( network.neurons[ j ], w );
         end
      end
   end


function compute( network, x )
   setSignals( network.connectors, 1, x );
   computeAbstractNeurons( network.neurons, 1 );
   return getWinner( getSignals( network.connectors, 1 + network.inputs, network.neuronsCount ) );
   end

