#ifndef YCSB_C_PMEM_MEMCACHED_DB_H_
#define YCSB_C_PMEM_MEMCACHED_DB_H_

#include "core/db.h"

#include <string>
#include <vector>
#include <libmemcached/memcached.h>
#include <libmemcached/util.h>

namespace ycsbc {

class PmemMemcachedDB : public DB {
 public:
  PmemMemcachedDB(memcached_pool_st *memc_pool) {
      memc_pool_ = memc_pool;
  }

  ~PmemMemcachedDB() {
      memcached_pool_destroy(memc_pool_);
  }

  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields,
           std::vector<KVPair> &result);
  int Scan(const std::string &table, const std::string &key,
           int len, const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result) {
      throw "Scan: function not implemented!";
  }
  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values);
  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values) {
      return Update(table, key, values);
  }
  int Delete(const std::string &table, const std::string &key) {
      throw "Delete: function not implemented!";
  }

 private:
  memcached_pool_st *memc_pool_;
};

} // pmem_memcached

#endif // YCSB_C_PMEM_MEMCACHED_DB_H_
