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


module( ..., package.seeall )


-- Function for calculating distance between two points;
local function distance( p1, p2 )
   local d = 0;
   for i = 1, #p1 do
      d = d + ( p1[ i ] - p2[ i ] )^2;
      end

   return math.sqrt( d );
   end


function create( inputs, layers, actFuncs )
   local network = {};
   network.inputs = inputs;
   network.layers = {};
   network.neuronsCount = 0;
   network.neurons = {};
   network.connectorsCount = 1 + inputs;
   network.connectors = 0;
   network.memorySize = 0;
   network.memory = 0;
   network.procUnit = createProcUnit( PROC_UNIT.WEIGHTED_SUM );

   local j = inputs;
   for i = 1, #layers do
      network.layers[ i ] = layers[ i ];
      network.neuronsCount = network.neuronsCount + layers[ i ];
      network.connectorsCount = network.connectorsCount + layers[ i ];
      network.memorySize = network.memorySize + layers[ i ] * ( j + 1 );
      j = layers[ i ];
      end

   -- Create digital components for multilayer perceptron;
   network.connectors = createDigitalConnectors( network.connectorsCount );
   network.memory = createMemoryModule( network.memorySize );

   -- Set 1.0 value for the first connector;
   setValues( network.connectors, 0, { 1.0 } );

   -- Create neuron layers;
   local neuronsBaseIndex = 1;
   local connectorsBaseIndex = 1;
   local memoryBaseIndex = 0;
   local prevNeuronsCount = inputs;
   for i = 1, #layers do
      local inputConnectors = { 0 };
      for k = 2, prevNeuronsCount + 1 do
         inputConnectors[ k ] = connectorsBaseIndex;
         connectorsBaseIndex = connectorsBaseIndex + 1;
         end

      for j = 1, layers[ i ] do
         network.neurons[ neuronsBaseIndex ] = createDigitalNeuron(
            prevNeuronsCount + 1,
            inputConnectors,
            network.connectors, connectorsBaseIndex,
            network.memory, memoryBaseIndex,
            network.procUnit,
            actFuncs[ i ]
            );

         neuronsBaseIndex = neuronsBaseIndex + 1;
         connectorsBaseIndex = connectorsBaseIndex + 1;
         memoryBaseIndex = memoryBaseIndex + prevNeuronsCount + 1;
         end

      connectorsBaseIndex = connectorsBaseIndex - layers[ i ];
      prevNeuronsCount = layers[ i ];
      end

   return network;
   end


function destroy( network )
   for i = 1, #network.neurons do
      closeId( network.neurons[ i ] );
      end

   closeId( network.procUnit );
   closeId( network.memory );
   closeId( network.connectors );
   end;


function train( network, vectors, err, damping, speed, epochsLimit )
   local w = {}
   for i = 1, network.memorySize do
      w[ i ] = 1.0 - 2.0 * math.random();
      end

   setDigitalWeights( network.memory, 0, w );

   local lastLayer = network.layers[ #network.layers ];
   local epochs = 0;
   while epochs < epochsLimit do
      local vec = epochs % #vectors + 1;
      setValues( network.connectors, 1, vectors[ vec ][ 1 ] );
      trainBPDigitalNeurons( network.neurons, network.layers, vectors[ vec ][ 2 ], damping, speed );
      computeDigitalNeurons( network.neurons, 1 );
      local y = getValues( network.connectors, network.connectorsCount - lastLayer, lastLayer );

      if distance( vectors[ vec ][ 2 ], y ) <= err then
         -- Complete test;
         local maxE = 0.0;
         for i = 1, #vectors do
            y = compute( network, vectors[ i ][ 1 ] );
            local e = distance( vectors[ i ][ 2 ], y );
            if maxE < e then maxE = e end
            end

         if maxE <= err then break end
         end

      epochs = epochs + 1;
      end

   return epochs;
   end


function compute( network, x )
   setValues( network.connectors, 1, x );
   computeDigitalNeurons( network.neurons, 1 );
   local lastLayer = network.layers[ #network.layers ];
   return getValues( network.connectors, network.connectorsCount - lastLayer, lastLayer );
   end

