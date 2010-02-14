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


function create( inputs, layer1, layer2 )
   local network = {};
   network.inputs = inputs;
   network.neuronsCount = layer1 + layer2;
   network.neurons = { {}, {} };
   network.actFuncs = { createActFunc( ACT_FUNC.GAUSSIAN, 1.0 ), createActFunc( ACT_FUNC.LINEAR, 1.0, 0.0 ) };
   network.connectorsCount = inputs + 1 + layer1 + layer2;
   network.connectors = createDigitalConnectors( network.connectorsCount );
   network.memorySize = ( inputs + 1 ) * layer1 + layer1 * layer2;
   network.memory = createMemoryModule( network.memorySize );
   network.procUnits = {
      createProcUnit( PROC_UNIT.RADIAL_BASIS, COEFF_USAGE.MUL_BY ),
      createProcUnit( PROC_UNIT.SCALAR )
      };

   -- Set 1.0 value for input'th connector;
   setValues( network.connectors, inputs, { 1.0 } );

   -- Create radial basis layer;
   local inputConnectors = {};
   for i = 1, inputs do
      inputConnectors[ i ] = i - 1;
      end

   inputConnectors[ inputs + 1 ] = inputs;

   for i = 0, layer1 - 1 do
      network.neurons[ 1 ][ i + 1 ] = createDigitalNeuron(
         inputs + 1,
         inputConnectors,
         network.connectors, inputs + 1 + i,
         network.memory, ( inputs + 1 ) * i,
         network.procUnits[ 1 ],
         network.actFuncs[ 1 ]
         );
      end

   -- Create scalar layer;
   inputConnectors = {};
   for i = 1, layer1 do
      inputConnectors[ i ] = inputs + i;
      end

   for i = 0, layer2 - 1 do
      network.neurons[ 2 ][ i + 1 ] = createDigitalNeuron(
         layer1,
         inputConnectors,
         network.connectors, inputs + 1 + layer1 + i,
         network.memory, ( inputs + 1 ) * layer1 + layer1 * i,
         network.procUnits[ 2 ],
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

   closeId( network.procUnits[ 2 ] );
   closeId( network.procUnits[ 1 ] );
   closeId( network.memory );
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
      setDigitalWeights( network.neurons[ 1 ][ i ], w );
      end

   -- Set linear layer weights;
   w = {};
   for i = 1, #network.neurons[ 2 ] do
      for j = 1, #network.neurons[ 1 ] do w[ j ] = vectors[ j ][ 2 ][ i ] end
      setDigitalWeights( network.neurons[ 2 ][ i ], w );
      end
   end


function compute( network, x )
   setValues( network.connectors, 0, x );
   computeDigitalNeurons( network.neurons[ 1 ], 1 );
   computeDigitalNeurons( network.neurons[ 2 ], 1 );
   return getValues( network.connectors, network.inputs + 1 + #network.neurons[ 1 ], #network.neurons[ 2 ] );
   end

