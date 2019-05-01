#include "thread.hpp"

  
void setUpEnvironment() {
  try {
    string connectionUrl = "dbname="+string(DBNAME)+" user="+USERNAME+" password="+PASSWORD+" host="+HOST;
    // string connectionUrl = "dbname="+string(DBNAME)+" user="+USERNAME+" password="+PASSWORD;
    cout << connectionUrl << endl;
    C = new connection(connectionUrl);
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      exit(1);
    }
    // C->disconnect();
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    exit(1);
  }
  clear_all_tables(C);
  // initiaize seq and packid
  //int tempseq = max_sequence_num(C);
  // int temppack = max_package_num(C);
  // if (tempseq > 0) {
  //  mySeqNum = tempseq + 1;
  // }
  // if (temppack > 0) {
  //  myPackageNum = temppack + 1;
  // }
  
  // initialize status map
  initializeStatusMap();
  
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT); // works
  amazonMsg * amazon = amazonMsg::getInstance();

  // connect and create a new world
  for (int i = 0; i < TIME_RETRY; i++) {
    if (i == TIME_RETRY) {
      perror("FAILED TO CONNECT TO WORLD");
      exit(EXIT_FAILURE);
    }
    if (world->createWorld(false, worldId, true, NUM_TRUCKS, 0, 0, worldId, C)) { // works
      break;
    }
  }
  
  cout << "newCreated World Id is " << worldId << endl; 
  
  // AMAZON reqired to test
  for (int i = 0; i < TIME_RETRY; i++) {
    if (i == TIME_RETRY) {
      perror("FAILED TO CONNECT TO AMAZON");
      exit(EXIT_FAILURE);
    }
    if (amazon->buildConnection(worldId)) {
      cout << "connected(amazon)!!!!!";
      break;
    }
  }


  // cout <<  "-----------------------------------------------" << endl;
  // world->goPickUp(0, 1, 0); // tested
  // cout <<  "-----------------------------------------------" << endl;
  //  world->goDeliver(1, 1, 1, 1, 1); // tested
  // cout <<  "-----------------------------------------------" << endl;
  // world->goQuery(1, 4); // tested
  //  UResponses  response1;
  // world->recvResponse(response1);
  // cout <<  "-----------------------------------------------" << endl;
  // UResponses  response2;
  // world->goQuery(1, 3);
  // world->recvResponse(response2);
  // UResponses response;
  // world->changeSpeed(200);
  // world->recvResponse(response);
  // world->disconnect(true); // tested

  // set up DB Connection here
  
  // wait
  cout << "accept" << endl;
  amazon->acceptConnection();
  // cout << "HIHIHIHI" << endl;
  // drop_all_tables( C);
}

void initializeStatusMap() {
  truckStatus["IDLE"] = "ID";
  truckStatus["TRAVELING"] = "TR";
  // arrive  warehouse
  truckStatus["ARRIVE WAREHOUSE"] = "AW";
  truckStatus["LOADING"] = "LO";
  truckStatus["DELIVERING"] = "DE";
  packageStatus["loading"] = "LG";
  packageStatus["loaded"] = "LD";
  packageStatus["delivering"] = "DG";
  packageStatus["delivered"] = "DD";
  packageStatus["packed"] = "PD";
}

void worldSide() {
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT);
  amazonMsg * amazon = amazonMsg::getInstance();
  while (true) {
    UResponses response;
    world->recvResponse(response);
    iterateUResponses(response); 
  }
}

void iterateUResponses(UResponses & response) {
  
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT);
  amazonMsg * amazon = amazonMsg::getInstance();
  
  if (response.completions_size() != 0) { // handled
    for (int i = 0; i < response.completions_size(); i++) {
      // debug
      cout << "completions sent back " << endl;
      cout << "truckId:" << response.completions(i).truckid() << endl;
      cout << "x:" << response.completions(i).x() << endl;
      cout << "y:" << response.completions(i).y() << endl;
      cout << "status:" << response.completions(i).status() << endl;
      cout << "seqnum:" << response.completions(i).seqnum() << endl; 
      cout << response.DebugString() << endl;
      // update db and send ack message
      world->sendAck(response.completions(i).seqnum());
      string thisTruckStatus = response.completions(i).status();
      int thisTruckId = response.completions(i).truckid();
      // cout << "--------------------------" << endl;
      // cout << thisTruckStatus << endl;
      // ARRIVE WAREHOUSE
      if (thisTruckStatus == "ARRIVE WAREHOUSE") {
	// cout << "--------------------------" << endl;
	vector<int> packed;
	packed_packages(C,  packed, to_string(response.completions(i).x()), to_string(response.completions(i).y()), to_string(thisTruckId));
	// cout << "--------------------------" << endl;
	// cout << "packed size " << packed.size() << endl;
	// cout << response.completions(i).x() << endl;
	// cout << response.completions(i).y() << endl;
	// cout << response.completions(i).truckid() << endl;
	lock_guard<mutex> lock(mtx);
	for (unsigned i = 0; i < packed.size(); i++) {
	  // int tempPackageId = get_order_id_for_package(C, packed[i]);
	  int tempOrderId = stoi(get_order_id_for_package(C, to_string(packed[i])));
	  update_status_of_package(C, to_string(packed[i]), PACKAGE_LOADING);
	  amazon->pickUpRes(mySeqNum, packed[i], tempOrderId, thisTruckId);
	  // amazon->updateDestination(444, packed[i], 3, 3);
	  insert_sequence_num(C, to_string(mySeqNum), "False");
	  ++mySeqNum;
	  // ++myPackageNum
	}
      }
      update_truck_status(C, to_string(response.completions(i).truckid()), truckStatus[response.completions(i).status()]);
      // update_truck_status(C, string(3), statusMap[response.completions(i).status()]);
      world->sendAck(response.completions(i).seqnum());      
    } 
  }

  if (response.delivered_size() != 0) {
    for (int i = 0; i < response.delivered_size(); i++) {
      // update
      // update(packageid, status);
      cout << "delievered sent back " << endl;
      cout << "truckId:" << response.delivered(i).truckid() << endl;
      cout << "packageId:" << response.delivered(i).packageid() << endl;
      cout << "seqnum:" << response.delivered(i).seqnum() << endl;
      // todo : function(packageId) // set the foreign key(points to truck) of the packageid to null
      update_status_of_package(C, to_string(response.delivered(i).packageid()), PACKAGE_DELIVERED);
      world->sendAck(response.delivered(i).seqnum());
      // send amazon
      lock_guard<mutex> lck(mtx);
      amazon->deliverRes(mySeqNum, response.delivered(i).packageid());
      insert_sequence_num(C, to_string(mySeqNum), "False");
      mySeqNum++;
    }
  }   

  if (response.truckstatus_size() != 0) {
     for (int i = 0; i < response.truckstatus_size(); i++) {
       cout << "truck status send back" << endl;
       cout <<  response.truckstatus(i).truckid() << endl;
       cout << response.truckstatus(i).status() << endl;
       cout << response.truckstatus(i).x() << endl;
       cout << response.truckstatus(i).y() << endl;
       cout << response.truckstatus(i).seqnum() << endl;
       // update(int truckId, int x, int y, string status);     
       update_truck_status(C, to_string(response.truckstatus(i).truckid()), truckStatus[response.truckstatus(i).status()]);
       world->sendAck(response.truckstatus(i).seqnum());
     }
   }

  
  if (response.error_size() != 0) { // error : do nothing
    for (int i = 0; i < response.error_size(); i++) {
      cout << "error sent back " << endl;
      cout << response.error(i).err() << endl;
      cout << response.error(i).originseqnum() << endl;
      cout << response.error(i).seqnum() << endl;
      world->sendAck(response.error(i).seqnum());
    }
  }

  if (response.has_finished()) { // I will never send disconnect
    cout << "finished snet back " <<  response.finished() << endl;
  }
  
  if (response.acks_size() != 0) {
    for (int i = 0; i < response.acks_size(); i++) {
      cout << "ack sent back" << endl;
      cout << response.acks(i) << endl;
      // todo: update the corresponding seqnum to acked
      update_acked_of_seqnum(C, to_string(response.acks(i)),  true);
    }
  }   
}

void amazonSide() {
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT);
  amazonMsg * amazon = amazonMsg::getInstance();
  while (true) {
    A2URequest req;
    amazon->receiveRes(req);
    iterateA2URequest(req);
  }
}

void iterateA2URequest(A2URequest & req) {
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT);
  amazonMsg * amazon = amazonMsg::getInstance();

  if (req.pickup_size() != 0) {
    for (int i = 0; i < req.pickup_size(); i++) {
      cout << "pickup requset from amazon" << endl;
      cout << req.pickup(i).seqnum() << endl;
      cout << req.pickup(i).orderid() << endl;
      cout << req.pickup(i).productname() << endl;
      cout << req.pickup(i).wh_id() << endl;
      cout << req.pickup(i).wh_x() << endl;
      cout << req.pickup(i).wh_y() << endl;
      cout << req.pickup(i).dest_x() << endl;
      cout << req.pickup(i).dest_y() << endl;

      string account = "";
      if (req.pickup(i).has_upsaccount()) {
	cout << req.pickup(i).upsaccount() << endl;
	account = req.pickup(i).upsaccount();
      }
      
      lock_guard<mutex> lck(mtx);
      amazon->sendAck(req.pickup(i).seqnum());
     
      // create the package in local database and assign the package to a truck
      if (req.pickup(i).has_upsaccount() && !isExist(C, account)) {
	cout << "The user account does not exit" << endl;
	amazon->sendError("account not exist", req.delivery(i).seqnum(), mySeqNum);
	insert_sequence_num(C, to_string(mySeqNum), "False");
	++mySeqNum;
	continue;
      }
      int truckIdAssigned = countTruck % NUM_TRUCKS;
      ++countTruck;
      cout << account << endl;
      insert_package(C, to_string(to_string(req.pickup(i).orderid())), account, to_string(truckIdAssigned), req.pickup(i).productname(),
		     PACKAGE_PACKED, to_string(req.pickup(i).wh_id()), to_string(req.pickup(i).wh_x()),
		     to_string(req.pickup(i).wh_y()), to_string(req.pickup(i).dest_x()), to_string(req.pickup(i).dest_y()), "False");
      
      insert_into_order_table(C, to_string(req.pickup(i).orderid()), to_string(to_string(req.pickup(i).orderid())));
      // update the truck status
      update_truck_status(C, to_string(truckIdAssigned), TRUCK_TRAVELING);
      // update_status_of_package(C, myPackageNum, "PD");
      // send the emssage to truck

      cout << "fdsjfjdslajflkdsj" << endl;
      cout << truckIdAssigned << endl;
      world->goPickUp(truckIdAssigned, req.pickup(i).wh_id(), mySeqNum);
      insert_sequence_num(C, to_string(mySeqNum), "False");
      ++mySeqNum;
      // ++myPackageNum;
    }
  }

  if (req.ack_size() != 0) {
    for (int i = 0; i < req.ack_size(); i++) {
      cout << "ack from amazon" << endl;
      cout << req.ack(i) << endl;
      update_acked_of_seqnum(C, to_string(req.ack(i)),  true);
    }
  }

  // message DeliveryRequest{
  //  required int64 seqnum = 1;
  //  required int64 tracknum = 2;
  // }

  if (req.delivery_size() != 0) {
    for (int i = 0; i < req.delivery_size(); i++) {
      cout << "delivery request from amazon" << endl;
      cout << req.delivery(i).seqnum() << endl;
      cout << req.delivery(i).tracknum() << endl;
      // local
      lock_guard<mutex> lck(mtx);
      amazon->sendAck(req.delivery(i).seqnum());
      // packageExist(connection * C, string packageid)
      if (!packageExist(C, to_string(req.delivery(i).tracknum()))) {
	amazon->sendError("The track number does not exist", req.delivery(i).seqnum(), mySeqNum);
	insert_sequence_num(C, to_string(mySeqNum), "False");
	++mySeqNum;
	continue;
      }
      string tempId = get_truck_id_for_a_particular_package(C, to_string(req.delivery(i).tracknum()));
      update_status_of_package(C, to_string(req.delivery(i).tracknum()), PACKAGE_DELIVERING);
      update_truck_status(C, tempId, TRUCK_DELIVERING);
      // remote
      // todo : Select xPosition, yPostion from Package where PackageId = x
      int x = stoi(get_xposition_of_a_package(C, to_string(req.delivery(i).tracknum())));
      int y = stoi(get_yposition_of_a_package(C, to_string(req.delivery(i).tracknum())));
      // worldMsg::goDeliver(int packageid, int x, int y, int truckid, int seqnum)
      // cout << "-命运---------------" << endl;
      // cout << req.delivery(i).tracknum() << endl;
      // cout << x << endl;
      // cout << y << endl;
      // cout << stoi(tempId) << endl;
      // cout << mySeqNum << endl;
      world->goDeliver(req.delivery(i).tracknum(), x, y, stoi(tempId), mySeqNum);
      insert_sequence_num(C, to_string(mySeqNum), "False");
      ++mySeqNum;
    }
  }      
}
  
void updateAddress() {
  amazonMsg * amazon = amazonMsg::getInstance();
  worldMsg * world = worldMsg::getInstance(WORLD_HOST, WORLD_PORT);
  while (true) {
    //this_thread::sleep_for(chrono::milliseconds(3000)); 
      vector<vector<string>> my_vec(1000);
      // package id, x, y
      get_parameter_based_on_dest(C, my_vec); // handled required to updated
      if (my_vec.size() == 0) {
	continue;
      }
      lock_guard<mutex> lck(mtx);
      for (unsigned i = 0; i < my_vec.size(); i++) {
	for (int j = 0; j < 3; j++) {
	  //world->updateDestination(mySeqNum, stoi(my_vec[i][0]), stoi(my_vec[i][1]), stoi(my_vec[i][2]));
	  int findTruckId = stoi(get_truck_id_for_a_particular_package(C, my_vec[i][0]));
	  string curStatus = get_package_status(C, my_vec[i][0]);
	  if (curStatus == "DD" ) {
	    cout << "Already Delivered : Update Address Failed";
	    continue;
	  }
	  if (curStatus == "DG") {
	    world->goDeliver(stoi(my_vec[i][0]), stoi(my_vec[i][1]), stoi(my_vec[i][2]), findTruckId, mySeqNum);
	    insert_sequence_num(C, to_string(mySeqNum), "False");
	    ++mySeqNum;
	  }
	  amazon->updateDestination(mySeqNum, stoi(my_vec[i][0]), stoi(my_vec[i][1]), stoi(my_vec[i][2]));
	  insert_sequence_num(C, to_string(mySeqNum), "False");
	  ++mySeqNum;
	}
      }
  }
}

void recvAdreUpdatedACK() {
  amazonMsg * amazon = amazonMsg::getInstance();
  while (true) {
    A2UResponse response;
    amazon->receiveDestUpdatedRes(response);    
    if (response.error_size() != 0) {
      for (int i = 0; i < response.error_size(); i++) {
        cout << "error message from ACKACK" << endl;
        cout << response.error(i).err() << endl;
        cout << response.error(i).originseqnum() << endl;
        cout << response.error(i).seqnum() << endl;
        amazon->sendAck22222(response.error(i).seqnum());
      }
    }

    if (response.ack_size() != 0) {
      for (int i = 0; i < response.ack_size(); i++) {
        cout << "ack from ACKACK" << endl;
        cout << response.ack(i) << endl;
        update_acked_of_seqnum(C, to_string(response.ack(i)),  true);
      }
    }             
  }
}



