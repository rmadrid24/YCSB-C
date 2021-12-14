#include "nvmmiddleware_db.h"
#include <iostream>
#include <mutex>

using namespace std;

std::mutex t_mut_;

namespace ycsbc {

int NvmMiddlewareDB::Read(const string &table, const string &key,
         const vector<string> *fields,
         vector<KVPair> &result) {
  string value;
  auto ft = mw_->enqueue_get(&key, &value, nvmmiddleware::Mode::INTERACTIVE);
  auto status = ft.get();
  //auto status = mw_->direct_get(&key, &value);
  if(status == pmem::kv::status::OK) {
	  return DB::kOK;
  }

  if(status == pmem::kv::status::NOT_FOUND) {
	  t_mut_.lock();
      	  cerr << "key not found : " << key << endl;
	  t_mut_.unlock();
	  return DB::kErrorNoData;
  } else {
	  t_mut_.lock();
	  cerr << "read error key: " << key << endl;
	  t_mut_.unlock();
	  //exit(0);
	  return DB::kErrorNoData;
  }
  return DB::kOK;
}

int NvmMiddlewareDB::Update(const string &table, const string &key,
           vector<KVPair> &values) {
  string value;
  for( KVPair &p : values ) {
	value.append(p.second);
  }
  auto ft = mw_->enqueue_put(&key, &value, nvmmiddleware::Mode::INTERACTIVE);
  auto status = ft.get();
  //auto status = mw_->direct_put(&key, &value);
  if(status != pmem::kv::status::OK){
	  cerr<<"insert error " << status << endl <<endl;
	  exit(0);
  }
  return DB::kOK;
}

} // namespace ycsbc
