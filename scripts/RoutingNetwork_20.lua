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


require "reliability";

function delta( i, j )
   if i == j then return 1 end
   return 0;
   end

function create( neurons )
   local network = {};
   network.inputs = neurons;
   network.neuronsCount = neurons;
   network.neurons = {};
   network.capacitorsCount = 2 * neurons;
   network.capacitors = createAnalogCapacitors( network.capacitorsCount );
   network.comparatorsCount = neurons;
   network.comparators = createAnalogComparators( network.comparatorsCount );
   network.resistorsCount = 2 * ( neurons + 1 ) * neurons;
   network.resistors = createAnalogResistors( network.resistorsCount );
   network.wiresCount = 2 + neurons + neurons;
   network.wires = createAnalogWires( network.wiresCount );

   -- Set potentials for src and gnd wires;
   setPotentials( network.wires, 0, { 0.0, 1.0 } );

   -- Set capacitances for network capacitors;
   local capacitances = {};
   for i = 1, network.capacitorsCount do capacitances[ i ] = 1.0 end
   setAnalogCapacitances( network.capacitors, 0, capacitances );

   local inputWires = {};
   for i = 2, neurons + 1 do inputWires[ i ] = neurons + i end

   for i = 0, neurons - 1 do
      inputWires[ 1 ] = 2 + i;

      network.neurons[ i + 1 ] = createAnalogNeuron(
         neurons + 1,
         inputWires,
         0, 1,
         network.capacitors, i * 2,
         network.comparators, i,
         network.resistors, ( neurons + 1 ) * i * 2,
         network.wires, 2 + neurons + i
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
   closeId( network.capacitors );
   end;

function trainNetwork( network )
   local routersCount = ( 1 + math.sqrt( 1 + 4 * #network.neurons ) ) / 2;
   local neuron = 0;
   local w = { 1.0 };
   for x = 1, routersCount do
      for i = 1, routersCount do
         if i ~= x then
            neuron = neuron + 1;
            local weight = 0;
            for y = 1, routersCount do
               for j = 1, routersCount do
                  if j ~= y then
                     weight = weight + 1;
                     w[ weight + 1 ] = mu4 * delta( x, y ) * delta( i, j ) - mu3 * ( delta( x, y ) + delta( i, j ) - delta( j, x ) - delta( i, y ) );
                     end
                  end
               end

            setupAnalogResistors( network.resistors, 2 * ( #network.neurons + 1 ) * ( neuron - 1 ), #network.neurons + 1, w, 2 );
            end
         end
      end
   end

function calculateNetwork( network, src, dest, c, ro )
   local routersCount = ( 1 + math.sqrt( 1 + 4 * #network.neurons ) ) / 2;
   local neuron = 0;
   local bais = {};
   local initials = {};
   for x = 1, routersCount do
      for i = 1, routersCount do
         if i ~= x then
            neuron = neuron + 1;
            if ( x == dest ) and ( i == src ) then
               bais[ neuron ] = 0.5 * ( mu5 - mu4 );
            else
               bais[ neuron ] = 0.5 * ( - mu1 * c[ x ][ i ] - mu2 * ro[ x ][ i ] - mu4 );
               end

            initials[ neuron ] = 0.5;
            end
         end
      end

   setPotentials( network.wires, 2, bais );
   setPotentials( network.wires, 2 + #network.neurons, initials );
   computeAnalogLimNeuronsC( network.neurons, 10.0, 1000 );
   return getPotentials( network.wires, 2 + #network.neurons, #network.neurons );
   end

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

function decodePath( networkOut, src, dest )
   -- Create matrix;
   local matrix = {};
   local signal = 0;
   for x = 1, routersCount do
      matrix[ x ] = {};
      for i = 1, routersCount do
         if i ~= x then
            signal = signal + 1;
            matrix[ x ][ i ] = networkOut[ signal ];
         else
            matrix[ x ][ i ] = -1.0;
            end
         end
      end

   -- Decode matrix;
   local path = {};
   local pathIndex = 1;
   local nextX = src;
   repeat
      local valid = false;
      for i = 1, routersCount do
         if matrix[ nextX ][ i ] == 1.0 then
            path[ pathIndex ] = nextX;
            pathIndex = pathIndex + 1;
            nextX = i;
            valid = true;
            break;
            end
         end

      if ( not valid ) or pathIndex > routersCount then return false, {} end
      until ( nextX == dest )

   path[ pathIndex ] = nextX;
   return true, path;
   end

function calcPathLength( path, c )
   pathLength = 0.0;
   for i = 1, #path - 1 do
      pathLength = pathLength + c[ path[ i ] ][ path[ i + 1 ] ];
      end

   return pathLength;
   end

function testNetwork()
   --minLength = 0.0;
   predictedLength = 0.0;
   for src = 1, routersCount - 1 do
      for dest = src + 1, routersCount do
         --minLength = minLength + minPathes[ src ][ dest ];
         local y = calculateNetwork( net, src, dest, c, ro );
         local valid = false;
         local path = {};
         valid, path = decodePath( y, src, dest );
         if valid then
            for i = 1, #path - 1 do predictedLength = predictedLength + c[ path[ i ] ][ path[ i + 1 ] ] end
         else
            predictedLength = predictedLength + penaltyLength;
            end
         end
      end

   return ( ( predictedLength / 2.5 ) < 1.3 );
   end

mu1 = 950;
mu2 = 2500;
mu3 = 1500;
mu4 = 475;
mu5 = 2500;

c = {
{ 0.0, 0.1, 0.8, 0.0, 0.0 },
{ 0.1, 0.0, 0.1, 0.2, 0.3 },
{ 0.8, 0.1, 0.0, 0.9, 0.2 },
{ 0.0, 0.2, 0.9, 0.0, 0.4 },
{ 0.0, 0.3, 0.2, 0.4, 0.0 }
};

ro = {
{ 1.0, 0.0, 0.0, 1.0, 1.0 },
{ 0.0, 1.0, 0.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0, 0.0, 0.0 },
{ 1.0, 0.0, 0.0, 1.0, 0.0 },
{ 1.0, 0.0, 0.0, 0.0, 1.0 }
};

minPathes = {
{ 0.0, 0.1, 0.2, 0.3, 0.4 },
{ 0.0, 0.0, 0.1, 0.2, 0.3 },
{ 0.0, 0.0, 0.0, 0.3, 0.2 },
{ 0.0, 0.0, 0.0, 0.0, 0.4 },
{ 0.0, 0.0, 0.0, 0.0, 0.0 }
}

penaltyLength = 0.5;

-- Create routing network;
io.write( "Assembling routing network ( 20 neurons ) ... " ); io.flush();
routersCount = 5;
neuronsCount = routersCount * ( routersCount - 1 );
net = create( neuronsCount );
print( "[OK]" );

-- Train routing network;
io.write( "Training ... " ); io.flush();
trainNetwork( net );
print( "[OK]" );

-- Simulate routing network;
print( "Choose the option to do:\n1) Visualize results\n2) Estimate time to fail\n3) Estimate survival function\n4) Estimate component importance\n5) Estimate time to fail distribution\n6) Estimate faults count distribution\n7) Exit" );
while true do
   op = tonumber( io.read() );
   if op ~= nil then if op >= 1 and op <= 7 then break end end
   end

if op == 1 then
   y = calculateNetwork( net, 1, 5, c, ro );
   print( "Route matrix:" );
   printMatrix( y );

elseif op >= 2 and op <= 6 then
   op = op - 1;
   print( "Start simulation..." );
   lengths = { 10, 10, 7 };
   intervals = { { 0.00001, 0.0001 }, { 0.0, 200.0 }, { 0.0, 500.0 } };

   if op == 1 then
      delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
      for i = 0, lengths[ op ] - 1 do
         x = intervals[ op ][ 1 ] + i * delta;
         distr = createDistribution( DISTR.EXP, x );
         manager1 = createInterruptManager( net.resistors, distr, nil );
         manager2 = createInterruptManager( net.capacitors, distr, nil );
         engine = createSimulationEngine();
         appendInterruptManager( engine, manager1 );
         appendInterruptManager( engine, manager2 );
         y, dyl, dyh = reliability.estimateTimeToFail( 121, engine, testNetwork );
         print( x * 10000 .. " " .. dyl / 1000 .. " " .. y / 1000 .. " " .. dyh / 1000 );
         closeId( engine );
         closeId( manager2 );
         closeId( manager1 );
         closeId( distr );
         end

   elseif op >= 2 and op <= 5 then
      distr = createDistribution( DISTR.EXP, 0.0001 );
      manager1 = createInterruptManager( net.resistors, distr, nil );
      manager2 = createInterruptManager( net.capacitors, distr, nil );
      engine = createSimulationEngine();
      appendInterruptManager( engine, manager1 );
      appendInterruptManager( engine, manager2 );
      if op == 2 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateSurvivalFunction( x, 121, engine, testNetwork );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 3 then
         delta = ( intervals[ op ][ 2 ] - intervals[ op ][ 1 ] ) / ( lengths[ op ] - 1 );
         for i = 0, lengths[ op ] - 1 do
            x = intervals[ op ][ 1 ] + i * delta;
            y, dyl, dyh = reliability.estimateComponentImportance( x, 200, engine, testNetwork, manager1, 0 );
            print( x / 1000 .. " " .. dyl .. " " .. y .. " " .. dyh );
            end

      elseif op == 4 then
         x = reliability.estimateTimeToFailDistribution( 200, engine, testNetwork );
         for i = 1, #x do print( x[ i ] / 1000 .. ", " ) end
      elseif op == 5 then
         x = reliability.estimateFaultsCountDistribution( 200, engine, testNetwork, { manager1 } );
         for i = 0, net.resistorsCount do print( x[ i ] .. ", " ) end
         end

      closeId( engine );
      closeId( manager2 );
      closeId( manager1 );
      closeId( distr );
      end
   end

-- Close network;
destroy( net );

