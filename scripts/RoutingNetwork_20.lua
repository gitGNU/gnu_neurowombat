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


-- Routing neural network script ( 20 neurons ). Network task is to 
-- find shortest path from source to destination host.

function create( neurons )
   local network = {};
   network.inputs = neurons;
   network.neuronsCount = neurons;
   network.neurons = {};
   network.activatorsCount = 0;
   network.activators = 0;
   network.actFunc = createActFunc( ACT_FUNC.SIGMOID, 1.0 );
   network.buffersCount = 0;
   network.buffers = 0;
   network.connectorsCount = neurons + neurons;
   network.connectors = createAbstractConnectors( network.connectorsCount );
   network.weightsCount = ( neurons + 1 ) * neurons;
   network.weights = createAbstractWeights( network.weightsCount );
   network.processor = createAbstractProcessor( ABSTRACT_PROCESSOR.WEIGHTED_SUM );

   local inputConnectors = {};
   for i = 2, neurons + 1 do inputConnectors[ i ] = neurons + i - 2 end

   for i = 0, neurons - 1 do
      inputConnectors[ 1 ] = i;

      network.neurons[ i + 1 ] = createAbstractNeuron(
         neurons + 1,
         inputConnectors,
         network.connectors, neurons + i,
         network.weights, ( neurons + 1 ) * i,
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

function printMatrix( y )
   local signal = 0;
   for x = 1, routersCount do
      for i = 1, routersCount do
         if i ~= x then
            signal = signal + 1;
            io.write( string.format( "%.1f", y[ signal ] ) .. ", " );
         else
            io.write( "#.#, " );
            end
         end

      print( "" );
      end
   end

function delta( i, j )
   if i == j then return 1 end
   return 0;
   end

function trainNetwork( net, routersCount, src, dest, c, ro )
   local neuron = 0;
   local w = {};
   local bais = {};
   local initials = {};
   for x = 1, routersCount do
      for i = 1, routersCount do
         if i ~= x then
            neuron = neuron + 1;
            -- Calculate weights;
            local weight = 0;
            for y = 1, routersCount do
               for j = 1, routersCount do
                  if j ~= y then
                     weight = weight + 1;
                     w[ weight + 1 ] = mu4 * delta( x, y ) * delta( i, j ) - mu3 * ( delta( x, y ) + delta( i, j ) - delta( j, x ) - delta( i, y ) );
                     end
                  end
               end

            w[ 1 ] = 1.0;
            setAbstractWeights( net.neurons[ neuron ], w );

            -- Calculate baises;
            if ( x == dest ) and ( i == src ) then
               bais[ neuron ] = 0.5 * ( mu5 - mu4 );
            else
               bais[ neuron ] = 0.5 * ( - mu1 * c[ x ][ i ] - mu2 * ro[ x ][ i ] - mu4 );
               end

            initials[ neuron ] = 0.5;
            end
         end
      end

   setSignals( net.connectors, 0, bais );
   setSignals( net.connectors, #net.neurons, initials );
   end

function calculateNetwork( net )
   local y = getSignals( net.connectors, #net.neurons, #net.neurons );
   computeAbstractNeuronsC( net.neurons, 1, 10.0, 1000 );
   return getSignals( net.connectors, #net.neurons, #net.neurons );
   end

print( "API version: " .. apiVersion() );

mu1 = 950;
mu2 = 2500;
mu3 = 1500;
mu4 = 475;
mu5 = 2500;

c = {
{ 0.0, 0.1, 0.4, 0.0, 0.0 },
{ 0.0, 0.0, 0.8, 0.2, 0.3 },
{ 0.0, 0.0, 0.0, 0.5, 0.2 },
{ 0.0, 0.0, 0.0, 0.0, 0.4 },
{ 0.0, 0.0, 0.0, 0.0, 0.0 }
};

ro = {
{ 1.0, 0.0, 0.0, 1.0, 1.0 },
{ 1.0, 1.0, 0.0, 0.0, 0.0 },
{ 1.0, 1.0, 1.0, 0.0, 0.0 },
{ 1.0, 1.0, 1.0, 1.0, 0.0 },
{ 1.0, 1.0, 1.0, 1.0, 1.0 },
};

-- Create routing network;
io.write( "Assembling routing network ( 20 neurons ) ... " ); io.flush();
routersCount = 5;
neuronsCount = routersCount * ( routersCount - 1 );
net = create( neuronsCount );
print( "[OK]" );

-- Train routing network;
io.write( "Training ... " ); io.flush();
trainNetwork( net, routersCount, 1, 5, c, ro );
print( "[OK]" );

-- Test network;
y = calculateNetwork( net );
print( "Route matrix:" );
printMatrix( y );

-- Close network;
destroy( net );

