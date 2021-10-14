//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <future>
#include <hdr_histogram.h>
#include <numa.h>
#include <unistd.h>
#include "core/utils.h"
#include "core/timer.h"
#include "core/client.h"
#include "core/core_workload.h"
#include "db/db_factory.h"

using namespace std;

void UsageMessage(const char *command);
bool StrStartWith(const char *str, const char *pre);
string ParseCommandLine(int argc, const char *argv[], utils::Properties &props);

std::mutex print_mutex;

int DelegateClient(int rank, ycsbc::DB *db, ycsbc::CoreWorkload *wl, const int num_ops,
    bool is_loading, vector<int> &actual_ops, hdr_histogram* r_histogram, hdr_histogram* m_histogram) {
  db->Init();
  ycsbc::Client client(*db, *wl, r_histogram, m_histogram);
  //atomic<int> oks{0};
  //vector<future<int>> fut_vec;
  int oks;
  for (int i = 0; i < num_ops; i++) {	
	/*fut_vec.emplace_back(
  		std::async(
			[&]()
			{
				if (is_loading) {
					oks += client.DoInsert();
				} else {
					oks += client.DoTransaction();
				}
				return 1;
			}
		)
	);*/

    if (is_loading) {
      oks += client.DoInsert();
    } else {
      oks += client.DoTransaction();
    }
  }
  /*for (auto& fut : fut_vec) {
	fut.wait();
  }
  fut_vec.clear();*/
  db->Close();
  actual_ops[rank] = oks;
  return oks;
}

int main(const int argc, const char *argv[]) {
	vector<cpu_set_t> cpus;
	for (int j = 16; j < 48; j++) {
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(j, &cpuset);
		cpus.emplace_back(move(cpuset));
	}
	for (int k = 64; k < 96; k++) {
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(k, &cpuset);		
		cpus.emplace_back(move(cpuset));
	}
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(23, &cpuset);
	sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);
  utils::Properties props;
  string file_name = ParseCommandLine(argc, argv, props);

  ycsbc::DB *db = ycsbc::DBFactory::CreateDB(props);
  if (!db) {
    cout << "Unknown database name " << props["dbname"] << endl;
    exit(0);
  }

  ycsbc::CoreWorkload wl;
  wl.Init(props);

  struct hdr_histogram* r_histogram;
  struct hdr_histogram* m_histogram;
  hdr_init(
    1,  // Minimum value
    INT64_C(3600000000),  // Maximum value
    3,  // Number of significant figures
    &r_histogram
    );
  hdr_init(
    1,  // Minimum value
    INT64_C(3600000000),  // Maximum value
    3,  // Number of significant figures
    &m_histogram
    );

  //const int num_threads = stoi(props.GetProperty("threadcount", "1"));
  const int num_threads = 64;
  // Loads data
  vector<int> actual_ops;
  vector<std::thread> threads;
  int total_ops = stoi(props[ycsbc::CoreWorkload::RECORD_COUNT_PROPERTY]);
  struct hdr_histogram* r_histogram_l;
  struct hdr_histogram* m_histogram_l;
  hdr_init(
	1,  // Minimum value
	INT64_C(3600000000),  // Maximum value
	3,  // Number of significant figures
	&r_histogram_l
	);
  hdr_init(
	1,  // Minimum value
	INT64_C(3600000000),  // Maximum value
	3,  // Number of significant figures
	&m_histogram_l
	);
  utils::Timer<std::chrono::seconds> load_timer;
  int sum = 0;
  load_timer.Start();
  for (int i = 0; i < num_threads; i++) {
	actual_ops.emplace_back(0);
    	threads.emplace_back(std::thread(DelegateClient, i, db, &wl, total_ops / num_threads, true, ref(actual_ops), r_histogram_l, m_histogram_l));
	pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpus[i]), &cpus[i]);
  }
  assert((int)actual_ops.size() == num_threads);

  for (int i = 0; i < num_threads; i++) {
	threads[i].join();
	//cout << "load ops " << i << " : " << actual_ops[i] << endl;
       	sum += actual_ops[i];
  }
  double loadDuration = load_timer.End();
  cerr << "# Loading records:\t" << sum << " in " << loadDuration << " seconds." << endl;
  cout << "Insert Mean: " << hdr_value_at_percentile(m_histogram_l, 50.0) << endl;
  cout << "Insert 99%: " << hdr_value_at_percentile(m_histogram_l, 99.0) << endl;

  // Peforms transactions
  actual_ops.clear();
  threads.clear();
  total_ops = stoi(props[ycsbc::CoreWorkload::OPERATION_COUNT_PROPERTY]);
  utils::Timer<std::chrono::seconds> timer;
  timer.Start();
  for (int i = 0; i < num_threads; i++) {
	actual_ops.emplace_back(0);
	threads.emplace_back(std::thread(DelegateClient, i, db, &wl, total_ops / num_threads, false, ref(actual_ops), r_histogram, m_histogram));
	pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpus[i]), &cpus[i]);
  }
  assert((int)actual_ops.size() == num_threads);

  sum = 0;
  for (int i = 0; i < threads.size(); i++) {
	threads[i].join();
    	sum += actual_ops[i];
  }
  double duration = timer.End();
  cerr << "# Transaction throughput (KTPS)" << endl;
  cerr << props["dbname"] << '\t' << file_name << '\t' << num_threads << '\t' << total_ops << '\t' << duration << '\t';
  cerr << total_ops / duration / 1000 << endl;
  cout << "Read Mean: " << hdr_value_at_percentile(r_histogram, 50.0) << endl;
  cout << "Read 99%: " << hdr_value_at_percentile(r_histogram, 99.0) << endl;
  cout << "Write Mean: " << hdr_value_at_percentile(m_histogram, 50.0) << endl;
  cout << "Write 99%: " << hdr_value_at_percentile(m_histogram, 99.0) << endl;
  delete db;
}

string ParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
  int argindex = 1;
  string filename;
  while (argindex < argc && StrStartWith(argv[argindex], "-")) {
    if (strcmp(argv[argindex], "-threads") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("threadcount", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-db") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("dbname", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-host") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("host", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-port") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("port", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-slaves") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      props.SetProperty("slaves", argv[argindex]);
      argindex++;
    } else if (strcmp(argv[argindex], "-P") == 0) {
      argindex++;
      if (argindex >= argc) {
        UsageMessage(argv[0]);
        exit(0);
      }
      filename.assign(argv[argindex]);
      ifstream input(argv[argindex]);
      try {
        props.Load(input);
      } catch (const string &message) {
        cout << message << endl;
        exit(0);
      }
      input.close();
      argindex++;
    } else {
      cout << "Unknown option '" << argv[argindex] << "'" << endl;
      exit(0);
    }
  }

  if (argindex == 1 || argindex != argc) {
    UsageMessage(argv[0]);
    exit(0);
  }

  return filename;
}

void UsageMessage(const char *command) {
  cout << "Usage: " << command << " [options]" << endl;
  cout << "Options:" << endl;
  cout << "  -threads n: execute using n threads (default: 1)" << endl;
  cout << "  -db dbname: specify the name of the DB to use (default: basic)" << endl;
  cout << "  -P propertyfile: load properties from the given file. Multiple files can" << endl;
  cout << "                   be specified, and will be processed in the order specified" << endl;
}

inline bool StrStartWith(const char *str, const char *pre) {
  return strncmp(str, pre, strlen(pre)) == 0;
}

