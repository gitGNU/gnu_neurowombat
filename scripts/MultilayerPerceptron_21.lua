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


-- Multilayer perceptron script ( 21 neurons ). The goal of training 
-- is to predict periodic function values.

function visualizeResults( x1, x2, pointsCount )
   local value = x1;
   local delta = ( x2 - x1 ) / ( pointsCount - 1 );
   local x = {};
   print( "Real <=> predicted" );
   for i = 1, pointsCount do
      x[ 1 ] = 5.0 * math.sin( value - 5.0 * math.pi / 16.0 );
      x[ 2 ] = 5.0 * math.sin( value - 4.0 * math.pi / 16.0 );
      x[ 3 ] = 5.0 * math.sin( value - 3.0 * math.pi / 16.0 );
      x[ 4 ] = 5.0 * math.sin( value - 2.0 * math.pi / 16.0 );
      x[ 5 ] = 5.0 * math.sin( value - 1.0 * math.pi / 16.0 );
      setSignals( connectors, 1, x );
      computeAbstractNeurons( neurons, neuronsCount, 1 );
      y = getSignals( connectors, connectorsCount - 1, 1 );
      io.write( string.format( "%.3f <=> %.3f\n", 5.0 * math.sin( value ), y[ 1 ] ) );

      value = value + delta;
      end
   end


-- Function for training multilayer perceptron;
function trainNetwork( err, damping, speed, epochsLimit )
   local w = {}
   for i = 1, weightsCount do
      w[ i ] = 1.0 - 2.0 * math.random();
      end

   setAbstractWeights( weights, w, weightsCount, 0 );

   local x = {};
   local target = {};
   local delta = -5.0 * math.pi / 16.0;
   local i = 0;
   local epochs = 0;
   while epochs < epochsLimit do
      x[ 1 ] = 5.0 * math.sin( delta );
      x[ 2 ] = 5.0 * math.sin( delta + math.pi / 16.0 );
      x[ 3 ] = 5.0 * math.sin( delta + 2.0 * math.pi / 16.0 );
      x[ 4 ] = 5.0 * math.sin( delta + 3.0 * math.pi / 16.0 );
      x[ 5 ] = 5.0 * math.sin( delta + 4.0 * math.pi / 16.0 );
      target[ 1 ] = 5.0 * math.sin( delta + 5.0 * math.pi / 16.0 );
      delta = delta + math.pi / 16.0;

      setSignals( connectors, 1, x );
      trainBPAbstractNeurons( neurons, layers, target, damping, speed );
      computeAbstractNeurons( neurons, neuronsCount, 1 );
      local y = getSignals( connectors, connectorsCount - 1, 1 );

      if math.abs( target[ 1 ] - y[ 1 ] ) <= err then
         -- Complete test;
         local maxE = 0.0;
         local deltaTest = -5.0 * math.pi / 16.0;
         for j = 1, 32 do
            x[ 1 ] = 5.0 * math.sin( deltaTest );
            x[ 2 ] = 5.0 * math.sin( deltaTest + math.pi / 16.0 );
            x[ 3 ] = 5.0 * math.sin( deltaTest + 2.0 * math.pi / 16.0 );
            x[ 4 ] = 5.0 * math.sin( deltaTest + 3.0 * math.pi / 16.0 );
            x[ 5 ] = 5.0 * math.sin( deltaTest + 4.0 * math.pi / 16.0 );
            target[ 1 ] = 5.0 * math.sin( deltaTest + 5.0 * math.pi / 16.0 );
            deltaTest = deltaTest + math.pi / 16.0;

            setSignals( connectors, 1, x );
            computeAbstractNeurons( neurons, neuronsCount, 1 );
            y = getSignals( connectors, connectorsCount - 1, 1 );
            if math.abs( maxE ) < math.abs( target[ 1 ] - y[ 1 ] ) then
               maxE = target[ 1 ] - y[ 1 ];
               end
            end

         if math.abs( maxE ) <= err then break; end
         end

      if i < 32 then
         i = i + 1;
      else
         i = 0;
         delta = -5.0 * math.pi / 16.0;
         end

      epochs = epochs + 1;
      end

   return epochs;
   end


print( "API version: " .. apiVersion() );

-- Create activation functions;
linActFunc = createLinearActFunc( 1.0, 0.0 );
sigActFunc = createSigmoidActFunc();

-- Set neurons count in each layer;
layers = { 5, 15, 1 };
actFuncs = { linActFunc, sigActFunc, linActFunc };

-- Calculate total neurons, connectors and weights count;
neuronsCount = 0;
connectorsCount = 1 + layers[ 1 ]; -- The first connector is reserved for 1.0 signal;
weightsCount = 0;

j = 1;
for i = 1, #layers do
   neuronsCount = neuronsCount + layers[ i ];
   connectorsCount = connectorsCount + layers[ i ];
   weightsCount = weightsCount + layers[ i ] * ( j + 1 );
   j = layers[ i ];
   end

-- Create abstract components for multilayer perceptron;
io.write( "Creating abstract components ... " ); io.flush();
connectors = createAbstractConnectors( connectorsCount );
weights = createAbstractWeights( weightsCount );

-- Set 1.0 signal for the first connector;
setSignals( connectors, 0, { 1.0 } );
print( "[OK]" );

-- Create neuron layers;
io.write( "Assembling multilayer perceptron ... " ); io.flush();
neurons = {};
neuronsBaseIndex = 1;
weightsBaseIndex = 0;
connectorsBaseIndex = 1 + layers[ 1 ];
prevNeuronsCount = 1;
for i = 1, #layers do
   inputConnectors = {};
   inputConnectors[ 1 ] = 0;
   if i > 1 then
      for k = 2, prevNeuronsCount + 1 do
         inputConnectors[ k ] = connectorsBaseIndex;
         connectorsBaseIndex = connectorsBaseIndex + 1;
         end
      end

   for j = 1, layers[ i ] do
      if i == 1 then
         inputConnectors[ 2 ] = j;
         end

      neurons[ neuronsBaseIndex ] = createAbstractNeuron(
         prevNeuronsCount + 1,
         inputConnectors,
         connectors, connectorsBaseIndex,
         weights, weightsBaseIndex,
         0, 0,
         0, 0,
         actFuncs[ i ]
         );

      neuronsBaseIndex = neuronsBaseIndex + 1;
      weightsBaseIndex = weightsBaseIndex + prevNeuronsCount + 1;
      connectorsBaseIndex = connectorsBaseIndex + 1;
      end

   connectorsBaseIndex = connectorsBaseIndex - layers[ i ];
   prevNeuronsCount = layers[ i ];
   end

print( "[OK]" );

-- Train multilayer perceptron;
io.write( "Training ... " ); io.flush();
epochs = trainNetwork( 0.5, 0.5, 0.01, 10000 );
print( epochs .. " [OK]" );

visualizeResults( 0, 2 * math.pi, 10 );

-- Close objects;
for i = 1, neuronsCount do
   closeId( neurons[ i ] );
   end

closeId( weights );
closeId( connectors );
closeId( sigActFunc );
closeId( linActFunc );

