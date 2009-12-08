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


function create( neurons )
   local network = {};
   network.inputs = neurons;
   network.neuronsCount = neurons;
   network.neurons = {};
   network.comparatorsCount = neurons;
   network.comparators = createAnalogComparators( network.comparatorsCount );
   network.resistorsCount = neurons * ( neurons - 1 ) * 2;
   network.resistors = createAnalogResistors( network.resistorsCount );
   network.wiresCount = 2 + neurons;
   network.wires = createAnalogWires( network.wiresCount );

   -- Set potentials for src and gnd wires;
   setPotentials( network.wires, 0, { 0.0, 1.0 } );

   local inputWires = {};
   for i = 0, neurons - 1 do
      for j = 0, neurons - 2 do
         if j >= i then inputWires[ j + 1 ] = j + 3;
         else inputWires[ j + 1 ] = j + 2; end
         end

      network.neurons[ i + 1 ] = createAnalogNeuron(
         neurons - 1,
         inputWires,
         0, 1,
         network.comparators, i,
         network.resistors, ( neurons - 1 ) * i * 2,
         network.wires, 2 + i
         );
      end

   return network;
   end


function destroy( network )
   for i = 1, #network.neurons do
      closeId( network.neurons[ i ] );
      end

   closeId( network.wires );
   closeId( network.resistors );
   closeId( network.comparators );
   end;


function train( network, vectors )
   for i = 0, network.neuronsCount - 1 do
      local w = {};
      for j = 0, network.neuronsCount - 2 do
         w[ j + 1 ] = 0.0;
         if j >= i then
            for k = 1, #vectors do
               w[ j + 1 ] = w[ j + 1 ] + vectors[ k ][ i + 1 ] * vectors[ k ][ j + 2 ];
               end
         else
            for k = 1, #vectors do
               w[ j + 1 ] = w[ j + 1 ] + vectors[ k ][ i + 1 ] * vectors[ k ][ j + 1 ];
               end
            end

         w[ j + 1 ] = w[ j + 1 ] / network.neuronsCount;
         end

      setupAnalogResistors( network.resistors, 2 * ( network.neuronsCount - 1 ) * i, network.neuronsCount - 1, w, 2 );
      end
   end


function compute( network, x, times )
   setPotentials( network.wires, 2, x );
   computeAnalogNeurons( network.neurons, times );
   return getPotentials( network.wires, 2, network.neuronsCount );
   end

