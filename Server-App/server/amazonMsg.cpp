#include "amazonMsg.hpp"

// Listener amazonMsg::listener;
Connector amazonMsg::connector;
amazonMsg * amazonMsg::instance = 0;
google::protobuf::io::FileOutputStream * amazonMsg::out;
google::protobuf::io::FileInputStream * amazonMsg::in;
Listener amazonMsg::listener;
google::protobuf::io::FileOutputStream * amazonMsg::listenerOut = nullptr;
google::protobuf::io::FileInputStream * amazonMsg::listenerIn = nullptr;
mutex amazonMsg::mtx_connector_in;
mutex amazonMsg::mtx_connector_out;
mutex amazonMsg::mtx_listener_in;
mutex amazonMsg::mtx_listener_out;
Socket amazonMsg::tempListener;

amazonMsg * amazonMsg::getInstance() {
  if (instance == 0) {
    instance = new amazonMsg();
    connector = move(Connector(CONNECT_HOST , CONNECT_PORT));
    out = new google::protobuf::io::FileOutputStream(connector.getSocket());
    in = new google::protobuf::io::FileInputStream(connector.getSocket());
    listener = Listener(LISTEN_PORT);
  }
  return instance;
}

void amazonMsg::acceptConnection() {
  if (listenerOut != nullptr) {
    delete(listenerOut);
  }
  if (listenerIn != nullptr) {
    delete(listenerIn);
  }
  tempListener = listener.Accept();
  // cout << tempListener.getSocket() << endl;
  listenerOut = new google::protobuf::io::FileOutputStream(tempListener.getSocket()); 
  listenerIn = new google::protobuf::io::FileInputStream(tempListener.getSocket());
}

bool amazonMsg::buildConnection(int worldid) {
  U2AConnect uconnect;
  uconnect.set_worldid(worldid);
  sendMesgTo(uconnect, out);
  U2AConnected response;
  recvMesgFrom(response, in);
  cout << "Message is " << endl;
  cout << response.DebugString();
  if (response.result() == "connected!") {
    return true;
  }
  return false;
}

// message PickupResponse{
//   required int64 seqnum = 1;
//   required int64 tracknum = 2;
//   required int64 orderid = 3;
//   required int64 truckid = 4;
// }

void amazonMsg::pickUpRes(int seqnum, int tracknum, int orderid, int truckid) {
  U2AResponse response;
  PickupResponse * pickup = response.add_pickup();
  pickup->set_seqnum(seqnum);
  pickup->set_tracknum(tracknum);
  pickup->set_orderid(orderid);
  pickup->set_truckid(truckid);
  lock_guard<mutex> lck (mtx_listener_out);
  cout << "PICKUPRES" << endl;
  sendMesgTo(response, listenerOut);
}

// message DeliveryResponse{
//  required int64 seqnum = 1;
//  required int64 tracknum = 2;
// }
   
void amazonMsg::deliverRes(int seqnum, int tracknum) {
  U2AResponse response;
  DeliveryResponse * delivery = response.add_delivery();
  delivery->set_seqnum(seqnum);
  delivery->set_tracknum(tracknum);
  lock_guard<mutex> lck (mtx_listener_out);
  sendMesgTo(response, listenerOut);
}

// message ErrorMessage{
//   required string err = 1;
//   required int64 originseqnum = 2;
//   required int64 seqnum = 3;
// }

void amazonMsg::sendError(string err, int originseqnum, int seqnum) {
  U2AResponse response;
  ErrorMessage * error = response.add_error();
  error->set_err(err);
  error->set_originseqnum(originseqnum);
  error->set_seqnum(seqnum);
  lock_guard<mutex> lck (mtx_listener_out);
  sendMesgTo(response, listenerOut);
}

void amazonMsg::sendAck(int ack) {
  U2AResponse response;
  response.add_ack(ack);
  lock_guard<mutex> lck (mtx_listener_out);
  sendMesgTo(response, listenerOut);
}

// message A2URequest {
//   repeated  PickupRequest pickup = 1;
//   repeated  DeliveryRequest delivery = 2;
//   repeated int64 ack = 3;
// }
void amazonMsg::receiveRes(A2URequest & req) {
  lock_guard<mutex> lck (mtx_listener_in);
  recvMesgFrom(req, listenerIn);
  
  // if (req.pickup_size() != 0) {
  //   for (int i = 0; i < req.pickup_size(); i++) {
  //     cout << "pickup requset from amazon" << endl;
  //     cout << req.pickup(i).seqnum() << endl;
  //     cout << req.pickup(i).orderid() << endl;
  //     cout << req.pickup(i).productname() << endl;
  //     cout << req.pickup(i).wh_id() << endl;
  //     cout << req.pickup(i).wh_x() << endl;
  //     cout << req.pickup(i).wh_y() << endl;
  //     cout << req.pickup(i).dest_x() << endl;
  //     cout << req.pickup(i).dest_y() << endl;
  //     if (req.pickup(i).has_upsaccount()) {
  // 	cout << req.pickup(i).upsaccount() << endl;
  //     }
  //   }
  // }

  // if (req.ack_size() != 0) {
  //   for (int i = 0; i < req.ack_size(); i++) {
  //     cout << "ack from amazon" << endl;
  //     cout << req.ack(i) << endl;
  //   }
  // }

  // message DeliveryRequest{
  //  required int64 seqnum = 1;
  //  required int64 tracknum = 2;
  // }
  
  // if (req.delivery_size() != 0) {
  //   for (int i = 0; i < req.delivery_size(); i++) {
  //     cout << "delivery request from amazon" << endl;
  //     req.delivery(i).seqnum();
  //     req.delivery(i).tracknum();
  //   }
  // }
  
}


// message UpdateDest{
//   required int64 seqnum = 1;
//   required int64 tracknum = 2;
//   required int64 new_x = 3;
//   required int64 new_y = 4;
//   required string newDest = 3;
// }
void amazonMsg::updateDestination(int seqnum, int tracknum, int new_x, int new_y) {
  U2ARequest req;
  UpdateDest * updatedAddr = req.add_dest();
  updatedAddr->set_seqnum(seqnum);
  updatedAddr->set_tracknum(tracknum);
  updatedAddr->set_new_x(new_x);
  updatedAddr->set_new_y(new_y);
  lock_guard<mutex> lck (mtx_connector_out);
  sendMesgTo(req, out);
}

void amazonMsg::sendAck22222(int ack) {
  U2ARequest req;
  req.add_ack(ack);
  lock_guard<mutex> lck (mtx_connector_out);
  sendMesgTo(req, out);
}


// message ErrorMessage{
//   required string err = 1;
//   required int64 originseqnum = 2;
//   required int64 seqnum = 3;
// }

void amazonMsg::receiveDestUpdatedRes(A2UResponse & response) {
  lock_guard<mutex> lck (mtx_connector_in);
  recvMesgFrom(response, in);
  
  // if (response.error_size() != 0) {
  //   for (int i = 0; i < response.error_size(); i++) {
  //     cout << "error message from amazon" << endl;
  //     cout << response.error(i).err() << endl;
  //     cout << response.error(i).originseqnum() << endl;
  //     cout << response.error(i).seqnum() << endl;
  //     // amazon->sendAck22222(response.error(i).seqnum());
  //   }
  // }

  // if (response.ack_size() != 0) {
  //   for (int i = 0; i < response.ack_size(); i++) {    
  //     cout << "ack from amazon" << endl;
  //     cout << response.ack(i) << endl;
  //     // update_acked_of_seqnum(C, to_string(response.ack(i)),  true)
  //   }
  // }
}









 


























