#ifndef __THREAD__
#define __THREAD__

// PORT
#define WORLD_HOST "vcm-8131.vm.duke.edu"
#define WORLD_PORT "12345"
#define AMAZON_HOST "vcm-7975.vm.duke.edu"
// #define AMAZON_PORT "22222"
#define TIME_RETRY 10
#define NUM_TRUCKS 10

// DATABSSE
#define DBNAME "ups_db"
#define USERNAME "postgres"
#define PASSWORD "password"
#define HOST "db"
#define PORT "5432"

// package status
#define PACKAGE_PACKED "PD"
#define PACKAGE_LOADING "LG"
#define PACKAGE_LOADED "LD"
#define PACKAGE_DELIVERING "DG"
#define PACKAGE_DELIVERED "DD"

// truck status
#define TRUCK_IDLE "ID"
#define TRUCK_TRAVELING "TR"
#define TRUCK_ARRIVE_WAREHOUSE "AW"
#define TRUCK_LOADING "LO"
#define TRUCK_DELIVERING "DE"


#include "handle_requests.hpp"
#include "amazonMsg.hpp"
#include "worldMsg.hpp"
#include <cstdio>
#include <cstdlib>
#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <unordered_map>
#include <chrono>
#include <thread>

using namespace std;
using namespace pqxx; 

static int worldId = -1;
static connection * C;
static mutex mtx;
static int mySeqNum = 0;
// static int myPackageNum = 0;
static unordered_map<string, string> packageStatus;
static unordered_map<string, string> truckStatus;
static int countTruck = 0;

void setUpEnvironment();
void worldSide();
void amazonSide();
void initializeStatusMap();
void iterateStatusMap();
void iterateUResponses(UResponses & response);
void iterateA2URequest(A2URequest & req);
void updateAddress();
void recvAdreUpdatedACK();
#endif
