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


function create( inputs, neuronsPerLayer )
   local network = {};
   network.inputs = inputs;
   network.neuronsCount = neuronsPerLayer * 2;
   network.neurons = { {}, {} };
   network.actFuncs = { createActFunc( ACT_FUNC.LINEAR, 1.0, 0.0 ), createActFunc( ACT_FUNC.LIM, 0.0, -1.0, 1.0 ) };
   network.connectorsCount = inputs + neuronsPerLayer;
   network.connectors = createDigitalConnectors( network.connectorsCount );
   network.memorySize = inputs * neuronsPerLayer + neuronsPerLayer * neuronsPerLayer;
   network.memory = createMemoryModule( network.memorySize );
   network.procUnit = createProcUnit( PROC_UNIT.WEIGHTED_SUM );

   local hmInputConnectors = {};
   for j = 0, inputs - 1 do
      hmInputConnectors[ j + 1 ] = j;
      end

   local hpInputConnectors = {};
   for j = 0, layer - 1 do
      hpInputConnectors[ j + 1 ] = inputs + j;
      end

   for i = 0, neuronsPerLayer - 1 do
      network.neurons[ 1 ][ i + 1 ] = createDigitalNeuron(
         inputs,
         hmInputConnectors,
         network.connectors, inputs + i,
         network.memory, inputs * i,
         network.procUnit,
         network.actFuncs[ 1 ]
         );

      network.neurons[ 2 ][ i + 1 ] = createDigitalNeuron(
         neuronsPerLayer,
         hpInputConnectors,
         network.connectors, inputs + i,
         network.memory, inputs * neuronsPerLayer + neuronsPerLayer * i,
         network.procUnit,
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

   closeId( network.procUnit );
   closeId( network.memory );
   closeId( network.connectors );
   closeId( network.actFuncs[ 2 ] );
   closeId( network.actFuncs[ 1 ] );
   end;


function train( network, vectors )
   local layer = network.neuronsCount / 2;
   local w = {};
   for i = 0, layer - 1 do
      for j = 1, network.inputs do
         w[ j ] = 0.5 * vectors[ i + 1 ][ j ];
         end

      setDigitalWeights( network.memory, network.inputs * i, w );
      end

   w = {};
   for i = 0, layer - 1 do
      for j = 0, layer - 1 do
         if i ~= j then w[ j + 1 ] = -1.0 / layer;
         else w[ j + 1 ] = 1.0; end
         end

      setDigitalWeights( network.memory, network.inputs * layer + layer * i, w );
      end
   end


function compute( network, x, times )
   setValues( network.connectors, 0, x );
   computeDigitalNeurons( network.neurons[ 1 ], 1 );
   computeDigitalNeurons( network.neurons[ 2 ], times );
   return getValues( network.connectors, network.inputs, #network.neurons[ 2 ] );
   end

