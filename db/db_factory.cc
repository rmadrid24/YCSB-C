//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db/db_factory.h"

#include <string>
#include "db/basic_db.h"
#include "db/nvmmiddleware_db.h"
#include "db/pmem_memcached_db.h"
#include "db/grpc_nvmmiddleware_db.h"

using namespace std;
using ycsbc::DB;
using ycsbc::DBFactory;

DB* DBFactory::CreateDB(utils::Properties &props) {
  if (props["dbname"] == "basic") {
    return new BasicDB;
  } else if (props["dbname"] == "nvmmiddleware") {
    std::string db_path = props["db"];
    int interactive_threads = stoi(props["interactive"]);
    int batch_threads = stoi(props["batch"]);
    auto mw_prt = new NvmMiddleware(db_path, interactive_threads, batch_threads);
    return new NvmMiddlewareDB(mw_prt);
  } else if (props["dbname"] == "memcached") {
    //std::string host = props["host"];
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers = NULL;
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(servers, "127.0.0.1", 8080, &rc);
    rc = memcached_server_push(memc, servers);
    //memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    memcached_pool_st *memc_pool = memcached_pool_create(memc, 100, 100);
    if (rc == MEMCACHED_SUCCESS)
	fprintf(stderr, "Added server successfully\n");
    else
	fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc, rc));
    
    return new PmemMemcachedDB(memc_pool);
  } else if (props["dbname"] == "grpcnvmmiddleware") {
    MiddlewareClient *mw_ptr = new MiddlewareClient("localhost", 50051, "interactive");
    return new GrpcNvmMiddlewareDB(mw_ptr);
  } return NULL;
}

