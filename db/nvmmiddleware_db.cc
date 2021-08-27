#include "nvmmiddleware_db.h"

using namespace std;

namespace ycsbc {

int NvmMiddlewareDB::Read(const string &table, const string &key,
         const vector<string> *fields,
         vector<KVPair> &result) {
  string value;
  auto ft = mw_->enqueue_get(&key, &value, nvmmiddleware::Mode::INTERACTIVE);
  auto status = ft.get();
  //auto status = mw_->direct_get(&key, &value, nvmmiddleware::Mode::INTERACTIVE);
  //cout << "Key " << key << " has value " << value << endl;
  if(status == pmem::kv::status::OK) {
	  return DB::kOK;
  }

  if(status == pmem::kv::status::NOT_FOUND){
	  return DB::kOK;
  } else {
	  cerr << "read error" << endl;
	  exit(0);
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
  //auto status = mw_->direct_put(&key, &value, nvmmiddleware::Mode::INTERACTIVE);
  if(status != pmem::kv::status::OK){
	  cerr<<"insert error\n"<<endl;
	  exit(0);
  }
  return DB::kOK;
}

} // namespace ycsbc
