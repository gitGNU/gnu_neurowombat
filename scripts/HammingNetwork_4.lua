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


-- Hamming neural network script ( 4 neurons ). Network task is to 
-- classify 4x4 images. It works as associative memory and returns one 
-- of the base images for every input image. There are 2 base images:
-- ####     ....
-- #### and ....
-- ....     ####
-- ....     ####
-- Network has to perform at least 90% correct classification for 100
-- random images like
-- ###.     ..##
-- #### or  ....
-- ....     #.##
-- #..#     ###.
-- Otherwise it is considered to be in a fault state.

-- Function for testing Hamming network;
function testNetwork()
   local x = {};
   local hitsCount = 0;
   local totalCount = 100;

   -- Generate bottom "1" pattern for i <= 50 and top "1" pattern for i > 50;
   for i = 1, 100 do
      local bottomPatternCounter = 0;
      local topPatternCounter = 0;
      for j = 1, inputsCount do
         if ( ( i <= 50 and j <= 8 ) or ( i > 50 and j > 8 ) ) then
            if math.random ( 4 ) == 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
         else
            if math.random ( 4 ) > 1 then x[ j ] = 1.0;
            else x[ j ] = -1.0; end
            end

         if ( j <= 8 and x[ j ] == 1.0 ) then topPatternCounter = topPatternCounter + 1; end
         if ( j > 8 and x[ j ] == 1.0 ) then bottomPatternCounter = bottomPatternCounter + 1; end
         end

      setPotentials( wires, 2, x );
      computeAnalogNeurons( neuronsHm, 1 );
      computeAnalogNeurons( neuronsHp, 4 );
      local y = getPotentials( wires, 2 + inputsCount, layer );

      -- Analize result;
      if topPatternCounter == bottomPatternCounter then totalCount = totalCount - 1; end

      if ( topPatternCounter > bottomPatternCounter and y[ 1 ] == 1.0 and y[ 2 ] == -1.0 or
         topPatternCounter < bottomPatternCounter and y[ 1 ] == -1.0 and y[ 2 ] == 1.0 
         ) then hitsCount = hitsCount + 1; end
      end

   return ( hitsCount / totalCount >= 0.90 );
   end

-- Function for estimating time to fail of Hamming network;
function estimateTimeToFail( times, lambda )
   local t = 0.0;
   local tsqr = 0.0;
   for i = 1, times do
      trainNetwork( trainVectors );
      local destr = createExponentialDestribution( lambda );
      local manager = createAnalogResistorsManager( destr, resistors );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      repeat
         if not testNetwork() then
            local x = getCurrentTime( engine );
            t = t + x;
            tsqr = tsqr + x * x;
            break;
            end

         until not stepOverEngine( engine )

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   t = t / times;
   tsqr = tsqr / times;
   local d = calcMeanCI( t, tsqr, times, 0.95 );
   return t, t - d, t + d;
   end

-- Function for estimating survival function of Hamming network;
function estimateSurvivalFunction( times, lambda, t )
   local p = 0.0;
   for i = 1, times do
      trainNetwork( trainVectors );
      local destr = createExponentialDestribution( lambda );
      local manager = createAnalogResistorsManager( destr, resistors );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      local tIsTooLarge = true;
      repeat
         if getCurrentTime( engine ) >= t then
            if testNetwork() then p = p + 1.0 end
            tIsTooLarge = false;
            break;
            end

         until not stepOverEngine( engine )

      if tIsTooLarge then
         if testNetwork() then p = p + 1.0 end
         end;

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for estimating faults count destribution of Hamming network;
function estimateFaultsCountDestribution( times, lambda )
   local d = {};
   for i = 0, resistorsCount do d[ i ] = 0 end
   for i = 1, times do
      trainNetwork( trainVectors );
      local destr = createExponentialDestribution( lambda );
      local manager = createAnalogResistorsManager( destr, resistors );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      failesCount = 0;
      while stepOverEngine( engine ) do
         failesCount = failesCount + 1;
         if not testNetwork() then break end

         end

      d[ failesCount ] = d[ failesCount ] + 1;

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   for i = 0, resistorsCount do d[ i ] = d[ i ] / times end
   return d;
   end

-- Function for estimating resistor importance of Hamming network;
function estimateWeightImportance( times, lambda, resistorIndex, t )
   local p = 0.0;
   local m = 0; local s = -1;
   for i = 1, times do
      trainNetwork( trainVectors );
      local destr = createExponentialDestribution( lambda );
      local manager = createAnalogResistorsManager( destr, resistors );
      local engine = createSimulationEngine();
      appendInterruptManager( engine, manager );

      if t >= getFutureTime( engine ) then
         while stepOverEngine( engine ) do
            if t < getFutureTime( engine ) then break end
            end         
         end

      if testNetwork() then
         setAnalogResistances( resistors, resistorIndex, { 0.0 } );
         if not testNetwork() then p = p + 1.0 end
         end

      closeId( engine );
      closeId( manager );
      closeId( destr );
      end

   p = p / times;
   return p, calcACProbabilityCI( p, times, 0.05 );
   end

-- Function for training Hamming network;
function trainNetwork( vectors )
   w = {};
   for i = 0, layer - 1 do
      for j = 1, inputsCount do
         w[ j ] = 0.5 * vectors[ i + 1 ][ j ];
         end

      setupAnalogResistors( resistors, inputsCount * i, inputsCount, w, 1 );
      end

   w = {};
   for i = 0, layer - 1 do
      for j = 0, layer - 1 do
         if i ~= j then w[ j + 1 ] = -1.0 / layer;
         else w[ j + 1 ] = 1.0; end
         end

      setupAnalogResistors( resistors, inputsCount * layer + 2 * layer * i, layer, w, 2 );
      end
   end

print( "API version: " .. apiVersion() );

-- Set inputs count in each neuron of the first layer;
inputsCount = 16;

-- Set neurons count in Hamming and Hopfield layers;
layer = 2;

-- Calculate total comparators, resistors and wires count;
comparatorsCount = layer;
resistorsCount = inputsCount * layer + 2 * layer * layer;
wiresCount = 2 + inputsCount + layer;

-- Create analog components for Hamming network;
io.write( "Creating analog components ... " ); io.flush();
comparators = createAnalogComparators( comparatorsCount );
resistors = createAnalogResistors( resistorsCount );
wires = createAnalogWires( wiresCount );
print( "[OK]" );

-- Create Hamming and Hopfield layers;
io.write( "Assembling Hamming network ( 4 neurons ) ... " ); io.flush();
neuronsHm = {};
neuronsHp = {};
hmInputWires = {};
for j = 0, inputsCount - 1 do
   hmInputWires[ j + 1 ] = 2 + j;
   end

hpInputWires = {};
for j = 0, layer - 1 do
   hpInputWires[ j + 1 ] = 2 + inputsCount + j;
   end

for i = 0, layer - 1 do
   neuronsHm[ i + 1 ] = createAnalogNeuron(
      inputsCount,
      hmInputWires,
      0, 1,
      0, 0,
      resistors, inputsCount * i,
      wires, 2 + inputsCount + i
      );

   neuronsHp[ i + 1 ] = createAnalogNeuron(
      layer,
      hpInputWires,
      0, 1,
      comparators, i,
      resistors, inputsCount * layer + layer * i * 2,
      wires, 2 + inputsCount + i
      );
   end

print( "[OK]" );

-- Train Hamming network;
io.write( "Training ... " ); io.flush();
trainVectors = {
   { 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1 },
   { -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1 }
   };

trainNetwork( trainVectors );
print( "[OK]" );

-- Simulate Hopfield network;
io.write( "Simulating ... " ); io.flush();
print( "[OK]" );

length = 10; x = {};
-- start = 0.00001; stop = 0.0001; delta = ( stop - start ) / ( length - 1 );
-- start = 0.0; stop = 5000.0; delta = ( stop - start ) / ( length - 1 );
start = 0.0; stop = 23000.0; delta = ( stop - start ) / ( length - 1 );
for i = 0, length - 1 do
   x[ i ] = start + i * delta;
   -- y, dyl, dyh = estimateTimeToFail( 250, x[ i ] );
   -- y, dyl, dyh = estimateSurvivalFunction( 2000, 0.0001, x[ i ] );
   y, dyl, dyh = estimateWeightImportance( 2000, 0.0001, 0, x[ i ] );
   y1, dyl1, dyh1 = estimateWeightImportance( 2000, 0.0001, 32, x[ i ] );
   -- print( x[ i ] * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
   -- print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
   print( x[ i ] / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh .. " " .. dyl1 .. " " .. y1 .. " " .. dyh1 );
   end;

-- destr = estimateFaultsCountDestribution( 1000, 0.0001 );
-- for i = 0, resistorsCount do
   -- print( i .. " " .. destr[ i ] );
   -- end

-- Close objects;
for i = 1, layer do
   closeId( neuronsHm[ i ] );
   closeId( neuronsHp[ i ] );
   end

closeId( comparators );
closeId( resistors );
closeId( wires );

