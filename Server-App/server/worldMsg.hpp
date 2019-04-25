#ifndef __WORLDMSG__
#define __WORLDMSG__

#include <thread>
#include <chrono>
#include "world_ups.pb.h"
#include "socket.hpp"
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "handle_requests.hpp"
#include <pqxx/pqxx>

using namespace std;

class worldMsg {
  static Connector connector;
  static google::protobuf::io::FileOutputStream * out;
  static google::protobuf::io::FileInputStream * in;
  static worldMsg * instance;
  static mutex mtx_in;
  static mutex mtx_out;
  // static worldMsg * mySelfPtr; 
public:
  static worldMsg * getInstance(const char * host, const char * port);
private: 
  worldMsg() {} // Constructor? (the {} brackets) are needed here.
  void initizeTrucks(UConnect & uconnect,int number, int x, int y, connection * C);
public:
  worldMsg(worldMsg const&) = delete;
  void operator=(worldMsg const&)  = delete;
  bool createWorld(bool isAmazon, google::protobuf::int64 worldId, bool isCreate, int numberTrucks, int x, int y, int & newCreatedId, connection * C);
  void goPickUp(int truckId, int whid, int seqnum);
  void goDeliver(int packageid, int x, int y, int truckid, int seqnum);
  void goQuery(int truckid, int seqnum);
  void changeSpeed(int semspeed);
  void sendAck(int ack);
  void disconnect(bool wantDisconnect);
  void recvResponse(UResponses & response);

  
  //this is adpated from code that a Google engineer posted online
  template<typename T> bool sendMesgTo(const T & message,
		  google::protobuf::io::FileOutputStream *out) {
    lock_guard<mutex> lck(mtx_out);
    

    { //extra scope: make output go away before out->Flush()
      // We create a new coded stream for each message.
      // Don’t worry, this is fast.

      google::protobuf::io::CodedOutputStream output(out);
      // Write the size.
      const int size = message.ByteSize();
      output.WriteVarint32(size);
      uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
      if (buffer != NULL) {
	// Optimization: The message fits in one buffer, so use the faster
	// direct-to-array serialization path.
	message.SerializeWithCachedSizesToArray(buffer);
      } else {
	// Slightly-slower path when the message is multiple buffers.
	message.SerializeWithCachedSizes(&output);
	if (output.HadError()) {
	  return false;
	}
      }
    }
    out->Flush();
    return true;
  }

  //this is adpated from code that a Google engineer posted online
  template<typename T> bool recvMesgFrom(T & message,
			 google::protobuf::io::FileInputStream * in ){
    lock_guard<mutex> lck(mtx_in);
    google::protobuf::io::CodedInputStream input(in);
    uint32_t size;
    if (!input.ReadVarint32(&size)) {
      return false;
    }
    // Tell the stream not to read beyond that size.
    google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
    // Parse the message.
    if (!message.MergeFromCodedStream(&input)) {
      return false;
    }
    if (!input.ConsumedEntireMessage()) {
      return false;
    }
    // Release the limit.
    input.PopLimit(limit);
    return true;
  }
};
#endif
