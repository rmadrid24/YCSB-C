#ifndef YCSB_C_GRPCNVMMIDDLEWARE_DB_H_
#define YCSB_C_GRPCNVMMIDDLEWARE_DB_H_

#include "core/db.h"

#include <string>
#include <vector>
#include "NvmMiddleware.h"
#include "client.h"

using nvmmiddleware::NvmMiddleware;

namespace ycsbc {

class GrpcNvmMiddlewareDB : public DB {
 public:
  GrpcNvmMiddlewareDB(MiddlewareClient *mw_ptr) {
      mw_ = mw_ptr;
  }

  ~GrpcNvmMiddlewareDB() {
      delete mw_;
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
  MiddlewareClient *mw_;
};

} // ycsbc

#endif // YCSB_C_GRPCNVMMIDDLEWARE_DB_H_
