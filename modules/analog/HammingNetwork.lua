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


function create( inputs, neuronsPerLayer )
   local network = {};
   network.inputs = inputs;
   network.neuronsCount = neuronsPerLayer * 2;
   network.neurons = { {}, {} };
   network.comparatorsCount = neuronsPerLayer;
   network.comparators = createAnalogComparators( network.comparatorsCount );
   network.resistorsCount = inputs * neuronsPerLayer + 2 * neuronsPerLayer * neuronsPerLayer;
   network.resistors = createAnalogResistors( network.resistorsCount );
   network.wiresCount = 2 + inputs + neuronsPerLayer;
   network.wires = createAnalogWires( network.wiresCount );

   -- Set potentials for src and gnd wires;
   setPotentials( network.wires, 0, { 0.0, 1.0 } );

   local hmInputWires = {};
   for j = 0, inputs - 1 do
      hmInputWires[ j + 1 ] = 2 + j;
      end

   local hpInputWires = {};
   for j = 0, layer - 1 do
      hpInputWires[ j + 1 ] = 2 + inputs + j;
      end

   for i = 0, neuronsPerLayer - 1 do
      network.neurons[ 1 ][ i + 1 ] = createAnalogNeuron(
         inputs,
         hmInputWires,
         0, 1,
         0, 0,
         network.resistors, inputs * i,
         network.wires, 2 + inputs + i
         );

      network.neurons[ 2 ][ i + 1 ] = createAnalogNeuron(
         neuronsPerLayer,
         hpInputWires,
         0, 1,
         network.comparators, i,
         network.resistors, inputs * neuronsPerLayer + neuronsPerLayer * i * 2,
         network.wires, 2 + inputs + i
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

   closeId( network.wires );
   closeId( network.resistors );
   closeId( network.comparators );
   end;


function train( network, vectors )
   local layer = network.neuronsCount / 2;
   local w = {};
   for i = 0, layer - 1 do
      for j = 1, network.inputs do
         w[ j ] = 0.5 * vectors[ i + 1 ][ j ];
         end

      setupAnalogResistors( network.resistors, network.inputs * i, network.inputs, w, 1 );
      end

   w = {};
   for i = 0, layer - 1 do
      for j = 0, layer - 1 do
         if i ~= j then w[ j + 1 ] = -1.0 / layer;
         else w[ j + 1 ] = 1.0; end
         end

      setupAnalogResistors( network.resistors, network.inputs * layer + 2 * layer * i, layer, w, 2 );
      end
   end


function compute( network, x, times )
   setPotentials( network.wires, 2, x );
   computeAnalogNeurons( network.neurons[ 1 ], 1 );
   computeAnalogNeurons( network.neurons[ 2 ], times );
   return getPotentials( network.wires, 2 + network.inputs, #network.neurons[ 2 ] );
   end

