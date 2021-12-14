#include "grpc_nvmmiddleware_db.h"
#include <iostream>
#include <mutex>

using namespace std;

namespace ycsbc {

int GrpcNvmMiddlewareDB::Read(const string &table, const string &key,
         const vector<string> *fields,
         vector<KVPair> &result) {
  string value;
  auto ft = mw_->Get(&key, &value);
  auto status = ft.get();
  if(status != nvmmiddleware::Status::OK) {
	  cerr << "read error key: " << key << endl;
	  exit(0);
	  //return DB::kErrorNoData;
  }
  return DB::kOK;
}

int GrpcNvmMiddlewareDB::Update(const string &table, const string &key,
           vector<KVPair> &values) {
  string value;
  for( KVPair &p : values ) {
	value.append(p.second);
  }
  auto ft = mw_->Put(&key, &value);
  auto status = ft.get();
  if(status != nvmmiddleware::Status::OK) {
	  cerr<<"insert error " << status << endl <<endl;
	  exit(0);
  }
  return DB::kOK;
}

} // namespace ycsbc
