#include "handle_requests.hpp"


using namespace std;
using namespace pqxx;



                                                                                                              
void get_parameter_based_on_dest(connection * C, vector< vector<string> > & my_vec){

  work W(*C);
  bool val = true;
  string sql = "SELECT \"packageId\", \"xPosition\", \"yPosition\" FROM \"upsApp_package\" WHERE \"destRequiredUpdated\" = "  + W.quote(val) + " ;";
  result R = W.exec(sql);
  result::const_iterator res;
  int index = 0;

  for(res = R.begin(); res != R.end(); res++){
    if(res == R.end())
      break;
    my_vec[index].push_back(res[0].as<string>());
    my_vec[index].push_back(res[1].as<string>());
    my_vec[index].push_back(res[2].as<string>());
    string packageid = res[0].as<string>();
    string xposition = res[1].as<string>();
    string yposition = res[2].as<string>();
    
  bool update = false;
  sql = "UPDATE \"upsApp_package\" SET \"destRequiredUpdated\" = " + W.quote(update) + "  WHERE \"packageId\" = " + W.quote(packageid) + "AND \"xPosition\" = " + W.quote(xposition) + " AND \"yPosition\" = " + W.quote(yposition) + " ;";
  W.exec(sql);
  index++;
   }

my_vec.resize(index);
W.commit();

}



string get_owner_id_from_username(connection * C, string username){
  work W(*C);
  string sql = "SELECT id FROM auth_user  WHERE username  = " + W.quote(username) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string owner_id = res[0].as<string>();
  return owner_id;
}




int max_sequence_num(connection * C){

  work W(*C);
  string sql = "SELECT sequence_id FROM \"upsApp_sequence_number\" ORDER BY sequence_id DESC;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  if(res == R.end())
    return 0;
  else{

    int val = res[0].as<int>();
    return val;
  }


}

int max_package_num(connection * C){

  work W(*C);
  string sql = "SELECT \"packageId\" FROM \"upsApp_package\" ORDER BY \"packageId\" DESC;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  if(res == R.end())
    return 0;
  else{

    int val = res[0].as<int>();
    return val;
  }


}


int max_order_id(connection * C){

  work W(*C);
  string sql = "SELECT order_id FROM \"upsApp_order_table\" ORDER BY order_id DESC;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  if(res == R.end())
    return 0;
  else{

    int val = res[0].as<int>();
    return val;
  }

}




void drop_all_tables(connection * C){

  work W(*C);
  string sql =  "DROP TABLE IF EXISTS \"upsApp_sequence_number\", \"upsApp_order_table\",  \"upsApp_current_world\", \"upsApp_truck\", \"upsApp_package\" ;";
  W.exec(sql);
  W.commit();
}


void packed_packages(connection * C, vector<int> & package_vec, string whx_position, string why_position, string truck_id){
  work W(*C);
  string packed = "PD";
  string sql = "SELECT \"packageId\" FROM \"upsApp_package\" WHERE \"whxPosition\" = " + W.quote(whx_position) + " AND \"whyPosition\" = " + W.quote(why_position) + " AND status = " + W.quote(packed) +  " AND \"truck_id\" = " + W.quote(truck_id) + " ORDER BY \"packageId\" ASC ;";
  result R = W.exec(sql);
  cout << sql << endl;
  for (result::const_iterator res = R.begin(); res != R.end(); res++){

    int packageid = res[0].as<int>();
    package_vec.push_back(packageid);
    
  } 
}




bool packageExist(connection * C, string packageid){

  work W(*C);
  string sql = "SELECT COUNT(*) FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  int num = res[0].as<int>();
  if(num  == 0)
    return false;
  else
    return true;

}
            

bool isExist(connection * C, string username){

  work W(*C);
  string sql = "SELECT COUNT(*) FROM auth_user WHERE username = " + W.quote(username) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  int num = res[0].as<int>();
  if(num  == 0)
    return false;
  else
    return true;
 }


void insert_into_order_table(connection * C, string orderid, string package_num_id){
  work W(*C);
  string sql;
  sql = "INSERT INTO \"upsApp_order_table\"(order_id, package_num_id)  VALUES ( " + W.quote(orderid) + " ," + W.quote(package_num_id) + " );";
  W.exec(sql);
  W.commit();

}

string get_package_id_for_order(connection * C, string order_id){

  work W(*C);
  string sql;
  sql = "SELECT package_num_id  FROM \"upsApp_order_table\" WHERE order_id  = " + W.quote(order_id) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string package = res[0].as<string>();
  return package;


}

string get_order_id_for_package(connection * C, string package_id){
  work W(*C);
  string sql;
  sql = "SELECT order_id  FROM \"upsApp_order_table\" WHERE package_num_id  = " + W.quote(package_id) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string order = res[0].as<string>();
  return order;
}



void update_location_of_package(connection * C, string x_position, string y_position, string packageid){

  work W(*C);;
  string sql;
  sql = "UPDATE \"upsApp_package\" SET \"xPosition\"  = " + W.quote(x_position) + " , \"yPosition\" = " + W.quote(y_position) +  " WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  W.commit();

}



void update_truck_field_of_package(connection * C, string packageid, string value){

  work W(*C);;
  string sql;
  sql = "UPDATE \"upsApp_package\" SET truck_id = " + W.quote(value) + " WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  W.commit();
}

void update_acked_of_seqnum(connection * C, string seq,  bool val){

  work W(*C);;
  string sql;
  sql = "UPDATE \"upsApp_sequence_number\" SET ackedornot = " + W.quote(val) + " WHERE sequence_id = " + W.quote(seq) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  W.commit();

}


bool get_seqnum_acked_or_not(connection * C, string seqnum){
 work W(*C);;
 string sql;
 sql = "SELECT ackedornot FROM  \"upsApp_sequence_number\"  WHERE sequence_id = " + W.quote(seqnum) + " ;";
 result R = W.exec(sql);
 result::const_iterator res = R.begin();
 bool result = res[0].as<bool>();
 return result;
}




string get_truck_id_for_a_particular_package(connection * C, string packageid){

  work W(*C);;
  string sql;
  sql = "SELECT truck_id  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string truck_id = res[0].as<string>();
  return truck_id;
}

string get_xposition_of_a_package(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT \"xPosition\"  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string x_id = res[0].as<string>();
  return x_id;

}


string get_yposition_of_a_package(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT \"yPosition\"  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string y_id = res[0].as<string>();
  return y_id;
}

string get_package_status(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT status  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string status = res[0].as<string>();
  return status;
}

string get_item_description(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT item  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string item = res[0].as<string>();
  return item;

}

string get_warehouse_id_of_package(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT whid  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string wh_id = res[0].as<string>();
  return wh_id;
}

string get_warehouse_xposition(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT \"whxPosition\"  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string x_id = res[0].as<string>();
  return x_id;
}


string get_warehouse_yposition(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT \"whyPosition\"  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string y_id = res[0].as<string>();
  return y_id;

}


string get_owner_id_of_package(connection * C, string packageid){
  work W(*C);
  string sql;
  sql = "SELECT owner_id  FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string owner_id = res[0].as<string>();
  return owner_id;
}


string get_truck_id_for_a_particular_status(connection * C, string status){

  work W(*C);
  string sql;
  sql = "SELECT \"truckId\" FROM \"upsApp_truck\" WHERE STATUS = " + W.quote(status) + " ORDER BY \"truckId\" ASC ;";
  result R = W.exec(sql);
  string truck_id;
  result::const_iterator res = R.begin();
  if(res == R.end())
    return "NULL";
  else
    truck_id = res[0].as<string>();
  
  return truck_id;
}


string get_username(connection *C , string packageid){
  work W(*C);
  string sql = "SELECT owner_id FROM \"upsApp_package\" WHERE \"packageId\" = " + W.quote(packageid) + " ;";
  result R = W.exec(sql);
  result::const_iterator res = R.begin();
  string owner_id = res[0].as<string>();
  sql = "SELECT username  FROM auth_user  WHERE id  = " + W.quote(owner_id) + " ;";
  R = W.exec(sql);
  res = R.begin();
  string username = res[0].as<string>();
  return username;
}



void insert_sequence_num(connection * C, string seq_num, string ackedornot){
work W(*C);
string sql = "INSERT INTO \"upsApp_sequence_number\"(sequence_id, ackedornot) VALUES (" + W\
  .quote(seq_num) + " , " + W.quote(ackedornot) + ");";
W.exec(sql);
W.commit();
}


bool check_seq_num_exists(connection * C, string seq_num){
  work W(*C);
string sql = "SELECT * FROM \"upsApp_sequence_number\" WHERE sequence_id = " + \
W.quote(seq_num) + " ; ";
result R = W.exec(sql);
result::const_iterator res = R.begin();
 if(res == R.end())
   return false;
 else
   return true;
}


                                                                                                                                              
                                                                                                                                              

void update_package_destination(connection * C, string packageid, string new_x, string new_y){
string sql;
work W(*C);
sql = "UPDATE \"upsApp_package\" SET \"xPosition\" =  " + W.quote(new_x) + ", \"yPosition\" = " + W.quote(new_y) + " WHERE \"packageId\" = " +W.quote(packageid) + " ;";
W.exec(sql);
W.commit();
}

void update_package_warehouse_location_and_id(connection * C, string warehouse_id, string warehouse_x, string warehouse_y, string packageid){
string sql;
work W(*C);
sql = "UPDATE \"upsApp_package\" SET WHID = " + W.quote(warehouse_id) + ", \"whxPosition\" =  " + W.quote(warehouse_x) + ", \"whyPosition\" = " + W.quote(warehouse_y) + " WHERE \"packageId\" = " + W.quote(packageid) + " ;";
W.exec(sql);
W.commit();
}


void insert_package(connection * C, string packageid, string username, string truck_id, string item, string status = "DG", string warehouse_id = "0", string warehouse_x = "0" , string warehouse_y = "0", string x_position = "0", string y_position = "0", string destination_update = "False"){

  string sql;
  string owner_id;
  work W(*C);

  if(!username.empty()){
  owner_id = get_owner_id_from_username(C, username);
  string space = " , ";
  sql = "INSERT INTO \"upsApp_package\"(\"packageId\", owner_id, truck_id, status, item, whid, \"whxPosition\",\"whyPosition\", \"xPosition\", \"yPosition\", \"destRequiredUpdated\" ) VALUES(" + W.quote(packageid) + " , " +  W.quote(owner_id) +  " , " + W.quote(truck_id) + " , " + W.quote(status) + " , " +  W.quote(item) + " , " +  W.quote(warehouse_id) + " , " +  W.quote(warehouse_x) + " , " + W.quote(warehouse_y) + " , " + W.quote(x_position) + " , " +  W.quote(y_position) + " , " +  W.quote(destination_update) + ");";

  }
  else{
    
      sql = "INSERT INTO \"upsApp_package\"(\"packageId\",truck_id, status, item, whid, \"whxPosition\",\"whyPosition\", \"xPosition\", \"yPosition\", \"destRequiredUpdated\" ) VALUES(" + W.quote(packageid) +  " , " + W.quote(truck_id) + " , " + W.quote(status) + " , " +  W.quote(item) + " , " +  W.quote(warehouse_id) + " , " +  W.quote(warehouse_x) + " , " + W.quote(warehouse_y) + " , " + W.quote(x_position) + " , " +  W.quote(y_position) + " , " +  W.quote(destination_update) + ");";

  }
    try{
    W.exec(sql);
  }
   catch (const std::exception &e) {
      cerr << e.what() << std::endl;
      return;
  }

    W.commit();
}


void update_status_of_package (connection * C, string packageid, string new_status){
 string sql;
 work W(*C);
 sql = "UPDATE \"upsApp_package\" SET STATUS = " + W.quote(new_status) + " WHERE \"packageId\" = " + W.quote(packageid) + " ;";
 W.exec(sql);
 W.commit();
}


//Complete truck related functions
void insert_truck(connection * C, string truckid, string status){
  string sql;
  work W(*C);
  sql = "INSERT INTO \"upsApp_truck\"(\"truckId\",status) VALUES(" + W.quote(truckid) + " ,  " + W.quote(status) + ");";

  try{
    W.exec(sql);
  }
   catch (const std::exception &e) {
      cerr << e.what() << std::endl;
      return;
   }

   W.commit();
}


void update_truck_status(connection * C, string truck_id, string new_status){
  string sql;
  work W(*C);
  sql = "UPDATE \"upsApp_truck\" SET STATUS = " + W.quote(new_status) + " WHERE \"truckId\" = " + W.quote(truck_id)+ " ;";
  W.exec(sql);
  W.commit();
}

 string get_truck_status(connection * C, string truckid){
 string sql;
 work W(*C);
 sql = "SELECT STATUS FROM \"upsApp_truck\" WHERE \"truckId\" = " + W.quote(truckid) + " ; ";
 result R = W.exec(sql);
 string status;
 result::const_iterator res = R.begin();
 if(!(res == R.end()))
     status = res[0].as<string>();
 
 return status;

}


// void clear_all_tables(connection * C){

//   work W(*C);
//   string sql = "TRUNCATE \"upsApp_sequence_number\", \"upsApp_order_table\",  \"upsApp_current_world\", \"upsApp_truck\", \"upsApp_package\";";
//   W.exec(sql);
//   W.commit();

// }


void clear_all_tables(connection * C){

  work W(*C);
    string sql = "TRUNCATE \"upsApp_sequence_number\", \"upsApp_order_table\",  \"upsApp_current_world\", \"upsApp_truck\", \"upsApp_package\", \"auth_user\" CASCADE;";
    W.exec(sql);
    W.commit();

}



