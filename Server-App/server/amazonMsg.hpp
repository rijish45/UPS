#ifndef __AMAZONMSG__
#define __AMAZONMSG__ 

#define LISTEN_PORT "44444"
#define CONNECT_HOST "vcm-5947.vm.duke.edu"
// #define CONNECT_HOST "vcm-8273.vm.duke.edu"
#define CONNECT_PORT "22222"

#include "UA.pb.h"
#include "socket.hpp"
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

using namespace std;

class amazonMsg {
  static Connector connector;
  static amazonMsg * instance;
  static google::protobuf::io::FileOutputStream * out;
  static google::protobuf::io::FileInputStream * in;
  static Listener listener;
  static google::protobuf::io::FileOutputStream * listenerOut;
  static google::protobuf::io::FileInputStream * listenerIn;
  static mutex mtx_connector_in;
  static mutex mtx_connector_out;
  static mutex mtx_listener_in;
  static mutex mtx_listener_out;
  static Socket tempListener;
  // -------------------------
public:
  static amazonMsg * getInstance();
private: 
  amazonMsg() {} // Constructor? (the {} brackets) are needed here.
public:
  amazonMsg(amazonMsg const&) = delete;
  void operator=(amazonMsg const&)  = delete;
  bool buildConnection(int worldid);
  void pickUpRes(int seqnum, int tracknum, int orderid, int truckid);
  void deliverRes(int seqnum, int tracknum);
  void sendError(string err, int originseqnum, int seqnum);
  void sendAck(int ack);
  void receiveRes(A2URequest& req); 
  void acceptConnection();
  void updateDestination(int seqnum, int tracknum, int new_x, int new_y);
  void receiveDestUpdatedRes(A2UResponse & response);
  void sendAck22222(int ack);
  
  //this is adpated from code that a Google engineer posted online
  template<typename T> bool sendMesgTo(const T & message,
		  google::protobuf::io::FileOutputStream *out) {
   
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
