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


function create( inputs, layer1, layer2 )
   local network = {};
   network.inputs = inputs;
   network.neuronsCount = layer1 + layer2;
   network.neurons = { {}, {} };
   network.activatorsCount = 0;
   network.activators = 0;
   network.actFuncs = { createGaussianActFunc( 1.0 ), createLinearActFunc( 1.0, 0.0 ) };
   network.buffersCount = 0;
   network.buffers = 0;
   network.connectorsCount = inputs + layer1 + layer2;
   network.connectors = createAbstractConnectors( network.connectorsCount );
   network.weightsCount = ( inputs + 1 ) * layer1 + layer1 * layer2;
   network.weights = createAbstractWeights( network.weightsCount );
   network.processors = { createAbstractRadialBasisProcessor( 1 ), createAbstractScalarProcessor() };

   -- Create radial basis layer;
   local inputConnectors = {};
   for i = 1, inputs do
      inputConnectors[ i ] = i - 1;
      end

   for i = 0, layer1 - 1 do
      network.neurons[ 1 ][ i + 1 ] = createAbstractNeuron(
         inputs,
         inputConnectors,
         network.connectors, inputs + i,
         network.weights, ( inputs + 1 ) * i,
         0, 0,
         network.processors[ 1 ],
         network.actFuncs[ 1 ]
         );
      end

   -- Create scalar layer;
   inputConnectors = {};
   for i = 1, layer1 do
      inputConnectors[ i ] = inputs + i - 1;
      end

   for i = 0, layer2 - 1 do
      network.neurons[ 2 ][ i + 1 ] = createAbstractNeuron(
         layer1,
         inputConnectors,
         network.connectors, inputs + layer1 + i,
         network.weights, ( inputs + 1 ) * layer1 + layer1 * i,
         0, 0,
         network.processors[ 2 ],
         network.actFuncs[ 2 ]
         );
      end

   return network;
   end


function destroy( network )
   for i = 1, #network.neurons do
      for j = 1, #network.neurons[ i ] do
         closeId( network.neurons[ i ][ j ] );
         end
      end

   closeId( network.processors[ 2 ] );
   closeId( network.processors[ 1 ] );
   closeId( network.weights );
   closeId( network.connectors );
   closeId( network.actFuncs[ 2 ] );
   closeId( network.actFuncs[ 1 ] );
   end;


function train( network, vectors, spread )
   -- Set radial basis layer weights;
   local w = {};
   w[ network.inputs + 1 ] = 0.8326 / spread;
   for i = 1, #network.neurons[ 1 ] do
      for j = 1, network.inputs do w[ j ] = vectors[ i ][ 1 ][ j ] end
      setAbstractWeights( network.weights, ( network.inputs + 1 ) * ( i - 1 ), w );
      end

   -- Set linear layer weights;
   w = {};
   for i = 1, #network.neurons[ 2 ] do
      for j = 1, #network.neurons[ 1 ] do w[ j ] = vectors[ j ][ 2 ][ i ] end
      setAbstractWeights( network.neurons[ 2 ][ i ], w );
      end
   end


function compute( network, x )
   setSignals( network.connectors, 0, x );
   computeAbstractNeurons( network.neurons[ 1 ], 1 );
   computeAbstractNeurons( network.neurons[ 2 ], 1 );
   return getSignals( network.connectors, network.inputs + #network.neurons[ 1 ], #network.neurons[ 2 ] );
   end

