#include "worldMsg.hpp"
 
Connector worldMsg::connector;
worldMsg * worldMsg::instance = 0;
google::protobuf::io::FileOutputStream * worldMsg::out;
google::protobuf::io::FileInputStream * worldMsg::in;
mutex worldMsg::mtx_in;
mutex worldMsg::mtx_out;

worldMsg * worldMsg::getInstance(const char * host, const char * port) {
  if (instance == 0) {
      instance = new worldMsg();
      cout << host << endl;
      cout << port << endl;
      connector = move(Connector(host, port));
      out = new google::protobuf::io::FileOutputStream(connector.getSocket());
      in = new google::protobuf::io::FileInputStream(connector.getSocket());
  }
  // Instantiated on first use.
  return instance;
}

// message UInitTruck{
//   required int32 id = 1;
//   required int32 x=2;
//   required int32 y=3;
// }

// message UConnect{
//   optional int64 worldid = 1;
//   repeated UInitTruck trucks=2;
//   required bool isAmazon = 3;
// }
  
bool worldMsg::createWorld(bool isAmazon,
			   google::protobuf::int64 worldId,
			   bool isCreate,
			   int numberTrucks,
			   int x,
			   int y,
			   int & newCreatedId,
			   connection * C) {
  UConnect uconnect;
  if (!isCreate) {
    uconnect.set_worldid(worldId);
  } 
  uconnect.set_isamazon(isAmazon);
  initizeTrucks(uconnect, numberTrucks, x, y, C);
  
  sendMesgTo(uconnect, out);
  UConnected response;
  recvMesgFrom(response, in);
  cout << "Message is " << endl;
  cout << response.DebugString();
  if (response.result() == "connected!") {
    newCreatedId = response.worldid();
    return true;
  }
  return false;
}

void worldMsg::goPickUp(int truckId, int whid, int seqnum) {
  UCommands ucommand;
  UGoPickup * ugopickup = ucommand.add_pickups();
  ugopickup->set_truckid(truckId);
  ugopickup->set_whid(whid);
  ugopickup->set_seqnum(seqnum);
  sendMesgTo(ucommand, out);
}

// message UDeliveryLocation{
//   required int64 packageid = 1;
//   required int32 x = 2;
//   required int32 y = 3;
// }

// message UGoDeliver{
//   required int32 truckid = 1;
//   repeated UDeliveryLocation packages = 2;
//   required int64 seqnum = 3;
// }

void worldMsg::goDeliver(int packageid, int x, int y, int truckid, int seqnum) {
  UCommands ucommand;
  UGoDeliver * ugodeliever = ucommand.add_deliveries();
  ugodeliever->set_truckid(truckid);
  UDeliveryLocation * package = ugodeliever->add_packages();
  package->set_packageid(packageid);
  package->set_x(x);
  package->set_y(y);
  ugodeliever->set_seqnum(seqnum);
  sendMesgTo(ucommand, this->out);
}

void worldMsg::goQuery(int truckid, int seqnum) {
  UCommands ucommand;
  UQuery * uquery = ucommand.add_queries();
  uquery->set_truckid(truckid);
  uquery->set_seqnum(seqnum);
  sendMesgTo(ucommand, this->out);
}

void worldMsg::changeSpeed(int simspeed) {
  UCommands ucommand;
  ucommand.set_simspeed(simspeed);
  sendMesgTo(ucommand, this->out);
}

void worldMsg::disconnect(bool wantDisconnect) {
  UCommands ucommand;
  ucommand.set_disconnect(wantDisconnect);
  sendMesgTo(ucommand, out);
}

void worldMsg::sendAck(int ack) {
  UCommands ucommand;
  ucommand.add_acks(ack);
  sendMesgTo(ucommand, out);
}

void worldMsg::initizeTrucks(UConnect & uconnect, int number, int x, int y, connection * C) {
  for (int i = 0; i < number; i++) {
     UInitTruck * newTruck = uconnect.add_trucks();
     newTruck->set_id(i);
     newTruck->set_x(x);
     newTruck->set_y(y);
     cout << "debug information: Truck " << i << endl;
     cout << newTruck->DebugString();
     insert_truck(C, to_string(i), "ID");
  }
}


void worldMsg::recvResponse(UResponses & response) {
  this_thread::sleep_for(std::chrono::milliseconds(1000));
  // UResponses response;
  recvMesgFrom(response, in);
  // if (response.completions_size() != 0) {
  //   for (int i = 0; i < response.completions_size(); i++) {
  //     cout << "completions sent back " << endl;
  //     cout << response.completions(i).truckid() << endl;
  //     cout << response.completions(i).x() << endl;
  //     cout << response.completions(i).y() << endl;
  //     cout << response.completions(i).status() << endl;
  //     cout << response.completions(i).seqnum() << endl;
  //   }
  // }
 
  // if (response.delivered_size() != 0) {
  //   for (int i = 0; i < response.delivered_size(); i++) {
  //     // update
  //     // update(packageid, status);
  //     cout << "delievered sent back " << endl;
  //     cout << response.delivered(i).truckid() << endl;
  //     cout << response.delivered(i).packageid() << endl;
  //     cout << response.delivered(i).seqnum() << endl;
  //   }
  // }
  
  // if (response.truckstatus_size() != 0) {
  //    for (int i = 0; i < response.truckstatus_size(); i++) {
  //      cout << "truck status send back" << endl;
  //      cout <<  response.truckstatus(i).truckid() << endl;
  //      cout << response.truckstatus(i).status() << endl;
  //      cout << response.truckstatus(i).x() << endl;
  //      cout << response.truckstatus(i).y() << endl;
  //      cout << response.truckstatus(i).seqnum() << endl;
  //      // // update(int truckId, int x, int y, string status);                  
  //    }
  //  }

  // // message UErr{
  // //   required string err = 1;
  // //   required int64 originseqnum = 2;
  // //   required int64 seqnum = 3;
  // // }  
  // if (response.error_size() != 0) {
  //   for (int i = 0; i < response.error_size(); i++) {
  //     cout << "error sent back " << endl;
  //     cout << response.error(i).err() << endl;
  //     cout << response.error(i).originseqnum() << endl;
  //     cout << response.error(i).seqnum() << endl;
  //   }
  // }
  // if (response.has_finished()) {
  //   cout << "finished snet back " <<  response.finished() << endl;
  // }
  // if (response.acks_size() != 0) {
  //   for (int i = 0; i < response.acks_size(); i++) {
  //     cout << "ack sent back" << endl;
  //     cout << response.acks(i) << endl;
  //   }
  // }
  
}








