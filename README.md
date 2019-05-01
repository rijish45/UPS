# UPS

For this project, you will either be doing “mini-Amazon” (an online store) or “mini-UPS” (a
shipping website). If you are doing Amazon, you will have to make your system work with the
UPS systems in your interoperability group (IG)—2 groups doing Amazon and 2 groups doing UPS.

The “World”
----------
Since you won’t have access to real warehouses and trucks, your code will interact with a
simulated world provided for you. You will connect to the simulation server (port 12345 for UPS,
port 23456 for Amazon), and send commands and receive notifications.
The messages you can send and receive are in the .proto files (amazon.proto and ups.proto)
that will be provided. Notice that all messages either start with A or U to indicate which part they
belong to.
The server supports different worlds (identified by a 64-bit number). You may create as many
worlds as you want. There is presently no authentication on the worlds, so please only use your
own. To create a new world for a pair of Amazon and UPS, UPS should send a Uconnect request
without specifying a worldid number, so that the World Simulator would create one and return
its ID in UConnected response. Note only UPS can create a new world and only when you want
to create a new world do you leave the worldid blank.
Each world is comprised of a Cartesian coordinate grid where “addresses” are integer coordinates (so you will deliver a package to e.g., (2, 4)). The world contains trucks (controlled by
UPS) and warehouses (controlled by Amazon). These have to work together to deliver packages.
The basic flow is that you send an A/UConnect message with the worldid that you want and
receive an A/UConnected response. Note only ONE Amazon and ONE UPS is allowed to connect
to a world at the same. Upon successful connection, the result string in A/UConnected will
be “connected!”, otherwise it will be an error message starting with “error:”. Make sure your
result string is “connected!” before proceeding to any further actions. Once you have
received this response, you may send A/UCommands and receive A/Responses. You should not
send any other message, nor expect to receive any – all of the details are embedded in the
A/UCommands/Responses.
A/UCommands include two common options: simspeed and disconnect. You can adjust the
simulation speed (higher numbers make things happen more quickly in the world). Simulation
speed has a default value of 100 and it’s consistent once you specify it until you change it into
another value. Note that the simulation speed only affects future events. If you set
disconnect to true in a command, the server will finish processing whatever it is currently 
working on (your current A/UCommands), then send a response with finished = true,
and close the connection.
A/UCommands and A/UResponses also implement ack numbers to avoid losing an in-flight
message. The ack mechanism works as follows. For each request inside A/UCommands, there’s
a seqnum (you should keep track of the incrementing of seqnum coming from your side). When
World Simulator receives commands from your side, it will check the seqnum of each request.
Then it will process the request and return responses with acks of those seqnums. The same
thing happens when World Simulator send you responses, which means if you don’t return ack,
World Simulator will send the same responses for multiple times. Don’t assume World Simulator
receives all of the requests in your A/UCommands until you receive those acks.
Note: simspeed is only for testing/debugging. You MUST NOT rely on a particular simspeed for
the correctness of your program. When testing/debugging, if you want to try a large number of
actions quickly, you might set it high. Likewise, if you wish to exercise particular timing-related
conditions, you might set it slow. Your program MUST work correctly at ANY simspeed when the
TAs use. They will have a version of the world server which ignores simspeed commands that you
send and allows them to set the speed directly.


Amazon Commands details: (note all commands include a sequence number for
acknowledgement as described above)
--------------------------------------------------------------------------

1. **buy** You can ask for more of some products to be delivered to a warehouse. Specify item id,
description (any text) and the quantity you want. If this product has never been seen before,
it will be created. If the product has been seen before, you SHOULD provide the same
description (if you use different descriptions for the same product id, the behavior is
undefined). NOTE: buying new stock does not involve UPS.
2. **topack** Pack a shipment for delivery. You will be notified when it is ready. The ware- house that
you request to pack the shipment MUST have sufficient inventory (and the inventory will be
reduced accordingly).
3. **load** Load a shipment on to a truck. In order for this to succeed, the shipment MUST be packed
(and you must have received a ready notification) AND the truck MUST be at the warehouse,
ready to receive the shipment (the shipper must have sent them to pickup and they must
have received notification of completion).
queries ask the status of a package by specifying the packageid. Note you can do query at any
time.

Amazon Response details: 
-----------------------
1. **arrived** - When you buy, you will later get a notification that your orders have arrived. At this time,
you should update your records of what is in stock and may use the goods described in this
message to fulfill orders.
2. **ready** - Notification that packing is complete
3. **loaded** - Notification that you have finished loading a shipment onto a truck
packagestatus tell the current status of one package that you queried. Possible package status:
packing, packed, loading, loaded, delivering, delivered.
4. **error** - indicates that you failed to meet any of the MUST requirements specified at “Amazon
Commands details” above. Read the err string carefully for more information.


UPS Command details:
-------------------
1. **deliveries** - Once a package has been loaded, you can issue this command to send the truck to
deliver it to a particular location. Note that you MAY pickup other packages before making
deliveries. You MAY send more deliveries requests while the truck is delivering other
packages. You MAY even change the destination of a package by sending a delivery request
again before it arrives its destination. World Simulator allows idle truck carrying undelivered
packages. If you specify multiple deliveries at once, they will be performed in the order you
list them in the command.
2. **pickups** - Send a truck to a warehouse to pick up a package. The truck need not have an “idle”
status; it can also have an “arrive warehouse” or “delivering” status. If a truck receives
pickups requests in the middle of a delivering, it will immediately quit the current delivery
and turn to the specified warehouse. Later whenever the truck has a “delivering” status
again, it always starts from where it quits. The package need not be ready to issue this
command. While the truck is in route, it is busy and cannot be given other commands.

3. **queries** - ask the status of a truck by specifying truckid. Note you can do query at any time.

UPS Response details:
--------------------

1. **completions** - You will receive this notification when either (a) a truck reaches the warehouse you
sent it to (with a pickup command) and is ready to load a package or (b) a truck has finished
all of its deliveries (that you sent it to make with a deliveries command).
At this point the truck may be given other instructions. Note that the completion tells you
the current location of the truck. 
2. **delivered** - You will receive this notification when each package is delivered. Note that when each
package is delivered, a delivered response will be sent. When all deliveries are finished,
you will receive a completions response.
3. **truckstatustell** - the current status of a truck that you queried. Possible truck status: idle, traveling
(when receives pickups requests and is on its way to warehouse), arrive warehouse, loading
(loading package, after loading package finish, go back to “arrive warehouse” status),
delivering (when finished all deliver job, go to status “idle”).
4. **error** indicates that you failed to meet any of the MUST requirements specified at “UPS
Commands details” above. Read the err string carefully for more information.


Note World Simulator has a time-out value set to be 10 mins, if you found that you lose the
connection, don’t panic, just connect again.
that the world server’s replies are asynchronous. You may send several requests and receive the
replies many minutes later. You should use appropriate identifiers in the responses to figure out
what request a message is in response to. You also MUST NOT wait for the response to return a
web page – if the response takes a few minutes, the browser will time out.
You MAY wish to separate the handling of world server communication from the handling of the
web front end (hint: good idea). You could even go so far as placing the web server in a different
Docker container from the daemon which interacts with the world server. In such a design, both
programs can communicate through a common postgres database. You might even write these
pieces of software in different languages. 
