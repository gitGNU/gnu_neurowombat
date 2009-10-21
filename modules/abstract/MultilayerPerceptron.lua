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


function create( inputs, layers, actFuncs )
   local network = {};
   network.neuronsCount = 0;
   network.neurons = {};
   network.activatorsCount = 0;
   network.activators = 0;
   network.buffersCount = 0;
   network.buffers = 0;
   network.connectorsCount = 1 + inputs;
   network.connectors = 0;
   network.weightsCount = 0;
   network.weights = 0;
   network.processor = createAbstractWeightedSumProcessor();

   local j = inputs;
   for i = 1, #layers do
      network.neuronsCount = network.neuronsCount + layers[ i ];
      network.connectorsCount = network.connectorsCount + layers[ i ];
      network.weightsCount = network.weightsCount + layers[ i ] * ( j + 1 );
      j = layers[ i ];
      end

   -- Create abstract components for multilayer perceptron;
   network.connectors = createAbstractConnectors( network.connectorsCount );
   network.weights = createAbstractWeights( network.weightsCount );

   -- Set 1.0 signal for the first connector;
   setSignals( network.connectors, 0, { 1.0 } );

   -- Create neuron layers;
   local neuronsBaseIndex = 1;
   local connectorsBaseIndex = 1;
   local weightsBaseIndex = 0;
   local prevNeuronsCount = inputs;
   for i = 1, #layers do
      local inputConnectors = { 0 };
      for k = 2, prevNeuronsCount + 1 do
         inputConnectors[ k ] = connectorsBaseIndex;
         connectorsBaseIndex = connectorsBaseIndex + 1;
         end

      for j = 1, layers[ i ] do
         network.neurons[ neuronsBaseIndex ] = createAbstractNeuron(
            prevNeuronsCount + 1,
            inputConnectors,
            network.connectors, connectorsBaseIndex,
            network.weights, weightsBaseIndex,
            0, 0,
            network.processor,
            actFuncs[ i ]
            );

         neuronsBaseIndex = neuronsBaseIndex + 1;
         connectorsBaseIndex = connectorsBaseIndex + 1;
         weightsBaseIndex = weightsBaseIndex + prevNeuronsCount + 1;
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

   closeId( network.processor );
   closeId( network.weights );
   closeId( network.connectors );
   end;

