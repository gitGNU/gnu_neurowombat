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


-- Hopfield neural network script ( 16 neurons ). Network task is to 
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

-- Function for testing Hopfield network;
function testNetwork()
   local x = {};
   local hitsCount = 0;
   local totalCount = 100;

   -- Generate bottom "1" pattern for i <= 50 and top "1" pattern for i > 50;
   for i = 1, 100 do
      local bottomPatternCounter = 0;
      local topPatternCounter = 0;
      for j = 1, N do
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
      computeAnalogNeurons( neurons, N, 4 );
      local y = getPotentials( wires, 2, N );

      -- Analize result;
      local isTopPatern = true;
      local isBottomPatern = true;
      for j = 1, N do
         if j <= 8 then
            if y[ j ] ~= 1.0 then isTopPatern = false; end
            if y[ j ] ~= -1.0 then isBottomPatern = false; end
         else
            if y[ j ] ~= -1.0 then isTopPatern = false; end
            if y[ j ] ~= 1.0 then isBottomPatern = false; end
            end
         end

      if topPatternCounter == bottomPatternCounter then totalCount = totalCount - 1; end

      if ( topPatternCounter > bottomPatternCounter and isTopPatern or
         topPatternCounter < bottomPatternCounter and isBottomPatern
         ) then hitsCount = hitsCount + 1; end
      end

   return ( hitsCount / totalCount > 0.90 );
end

print( "API version: " .. apiVersion() );

-- Create analog components for Hopfield network;
N = 16;
io.write( "Creating analog components ... " ); io.flush();
comparators = createAnalogComparators( N );
resistors = createAnalogResistors( N * ( N - 1 ) * 2 );
wires = createAnalogWires( 2 + N );
print( "[OK]" );

-- Create neurons layer;
io.write( "Assembling Hopfield network ( 16 neurons ) ... " ); io.flush();
neurons = {};
for i = 0, N - 1 do
   inputWires = {};
   for j = 0, N - 2 do
      if j >= i then inputWires[ j + 1 ] = j + 3;
      else inputWires[ j + 1 ] = j + 2; end
      end

   neurons[ i + 1 ] = createAnalogNeuron(
      N - 1,
      inputWires,
      0, 1,
      comparators, i,
      resistors, ( N - 1 ) * i * 2,
      wires, 2 + i
      );
   end

print( "[OK]" );

-- Train Hopfield network;
io.write( "Training ... " ); io.flush();
M = 2;
images = {
   { 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1 },
   { -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1 }
   };

for i = 0, N - 1 do
   w = {};
   for j = 0, N - 2 do
      w[ j + 1 ] = 0.0;
      if j >= i then
         for k = 1, M do
            w[ j + 1 ] = w[ j + 1 ] + images[ k ][ i + 1 ] * images[ k ][ j + 2 ];
            end
      else
         for k = 1, M do
            w[ j + 1 ] = w[ j + 1 ] + images[ k ][ i + 1 ] * images[ k ][ j + 1 ];
            end
         end

      w[ j + 1 ] = w[ j + 1 ] / N;
      end

      setupAnalogResistors( resistors, 2 * ( N - 1 ) * i, N - 1, w, 2 );
   end

print( "[OK]" );

-- Simulate Hopfield network;
destr = createExponentialDestribution( 0.00001 );
manager = createAnalogResistorsManager( destr, resistors );
engine = createSimulationEngine();
appendInterruptManager( engine, manager );

io.write( "Simulating ... " ); io.flush();
resistorFaults = 0;
timeToFail = 0.0;
while stepOverEngine( engine ) do
   if not testNetwork() then
      timeToFail = getCurrentTime( engine );
      break;
      end
   resistorFaults = resistorFaults + 1;
   end;

print( "[OK]" );

print( "resistorFaults = " .. resistorFaults .. "; timeToFail = " .. timeToFail );

-- Close objects;
closeId( engine );
closeId( manager );
closeId( destr );
for i = 1, N do
   closeId( neurons[ i ] );
   end

closeId( comparators );
closeId( resistors );
closeId( wires );

