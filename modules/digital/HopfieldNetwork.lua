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


function create( neurons )
   local network = {};
   network.inputs = neurons;
   network.neuronsCount = neurons;
   network.neurons = {};
   network.actFunc = createActFunc( ACT_FUNC.LIM, 0.0, -1.0, 1.0 );
   network.connectorsCount = 2 + neurons;
   network.connectors = createDigitalConnectors( network.connectorsCount );
   network.memorySize = neurons * ( neurons - 1 );
   network.memory = createMemoryModule( network.memorySize );
   network.procUnit = createProcUnit( PROC_UNIT.WEIGHTED_SUM );

   local inputConnectors = {};
   for i = 0, neurons - 1 do
      for j = 0, neurons - 2 do
         if j >= i then inputConnectors[ j + 1 ] = j + 1
         else inputConnectors[ j + 1 ] = j end
         end

      network.neurons[ i + 1 ] = createDigitalNeuron(
         neurons - 1,
         inputConnectors,
         network.connectors, i,
         network.memory, ( neurons - 1 ) * i,
         network.procUnit,
         network.actFunc
         );
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
   closeId( network.actFunc );
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

      setDigitalWeights( network.memory, ( network.neuronsCount - 1 ) * i, w );
      end
   end


function compute( network, x, times )
   setValues( network.connectors, 0, x );
   computeDigitalNeurons( network.neurons, times );
   return getValues( network.connectors, 0, network.neuronsCount );
   end

