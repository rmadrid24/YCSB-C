#include "pmem_memcached_db.h"
#include <iostream>

using namespace std;

namespace ycsbc {

int PmemMemcachedDB::Read(const string &table, const string &key,
         const vector<string> *fields,
         vector<KVPair> &result) {
  const char *key_c = key.c_str();
  char *value;
  memcached_return rc;
  size_t value_length;
  uint32_t flags;
  memcached_st *memc_= memcached_pool_pop(memc_pool_, false, &rc);
  value = memcached_get(memc_, key_c, strlen(key_c), &value_length, &flags, &rc);
  if (rc != MEMCACHED_SUCCESS) {
	cerr<<"get error key " << key_c << " " << memcached_strerror(memc_, rc) << endl;
	memcached_pool_push(memc_pool_, memc_);
	return DB::kErrorNoData;
  }
  memcached_pool_push(memc_pool_, memc_);
  return DB::kOK;
}

int PmemMemcachedDB::Update(const string &table, const string &key,
           vector<KVPair> &values) {
  const char *key_c = key.c_str();
  const char *value_c;
  memcached_return_t rc;
  string value;
  for( KVPair &p : values ) {
	value.append(p.second);
  }
  value_c = value.c_str();
  //cout << "Insert key " << key_c << " value size " << strlen(value_c) << endl;
  memcached_st *memc_= memcached_pool_pop(memc_pool_, false, &rc);
  rc= memcached_set(memc_, key_c, strlen(key_c), value_c, strlen(value_c), (time_t)0, (uint32_t)0);
  if(rc != MEMCACHED_SUCCESS) {
	  cerr<<"insert error " << memcached_strerror(memc_, rc) << endl;
	  memcached_pool_push(memc_pool_, memc_);
	  exit(0);
  }
  memcached_pool_push(memc_pool_, memc_);
  return DB::kOK;
}

} // namespace ycsbc
