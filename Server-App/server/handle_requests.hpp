#ifndef _HANDLE_REQUESTS_
#define _HANDLE_REQUESTS_

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>
#include <pqxx/pqxx>
#include <string>
#include <fstream>
#include <mutex>


using namespace std;
using namespace pqxx;

//New function
string get_owner_id_from_username(connection * C, string username);


//max fucntions
//Get max-num
int max_sequence_num(connection * C);
int max_package_num(connection * C);
int max_order_id(connection * C);


/*
  vector < vector<string> > my_vec (1000);
  get_parameter_based_on_dest(C, my_vec);
  for (int it = 0; it != my_vec.size(); it++){
    for(int i = 0; i != 3; i++)
      cout << my_vec[it][i] << endl;
  }
 */


void get_parameter_based_on_dest(connection * C, vector< vector<string> > & my_vec);
void packed_packages(connection * C, vector<int> & package_vec,  string whx_position, string why_position, string truck_id);
bool packageExist(connection * C, string packageid);
bool isExist(connection * C, string username);
void drop_all_tables(connection * C);


//functions for order_table
void insert_into_order_table(connection * C, string orderid, string package_num_id);
string get_package_id_for_order(connection * C, string order_id);
string get_order_id_for_package(connection * C, string package_id);


//World table functions
void insert_current_world_id(connection * C, string name, string worldid, string id);
string get_world_id(connection * C, string name);


//functions you wanted
void update_truck_field_of_package(connection * C, string packageid, string value);
void update_acked_of_seqnum(connection * C, string seq,  bool val);
bool get_seqnum_acked_or_not(connection * C, string seqnum); 


//Update functions
void update_package_destination(connection * C, string packageid, string new_x, string new_y);
void update_package_warehouse_location_and_id(connection * C, string warehouse_id, string warehouse_x, string warehouse_y, string packageid);
void update_status_of_package (connection * C, string packageid, string new_status);
void update_truck_status(connection * C, string truck_id, string new_status);
void update_location_of_package(connection * C, string x_position, string y_position);

//Insert functions
void insert_package(connection * C, string packageid, string username, string truck_id, string item, string status, string warehouse_id, string warehouse_x , string warehouse_y, string x_position, string y_position, string destination_update);
void insert_truck(connection * C, string truckid, string status);
void insert_sequence_num(connection * C, string seq_num, string ackedornot);
void clear_all_tables(connection * C);
bool check_seq_num_exists(connection * C, string seq_num);


//Get item from package table
string get_truck_id_for_a_particular_package(connection * C, string packageid); //new
string get_xposition_of_a_package(connection * C, string packageid); //new
string get_yposition_of_a_package(connection * C, string packageid); //new
string get_package_status(connection * C, string packageid); //new
string get_item_description(connection * C, string packageid); //new
string get_warehouse_id_of_package(connection * C, string packageid);
string get_warehouse_xposition(connection * C, string packageid); //new
string get_warehouse_yposition(connection * C, string packageid); //new
string get_owner_id_of_package(connection * C, string packageid); //new
string get_username(connection *C , string packageid);

//Get item from truck table
string get_truck_status(connection * C, string truckid);
string get_truck_id_for_a_particular_status(connection * C, string status);


#endif

