#!/bin/bash
export LD_LIBRARY_PATH="/home/jieliu/nvm-middleware/cmake/build:/home/jieliu/HdrHistogram_c/cmake/build/src:/home/jieliu/.local/lib:$LD_LIBRARY_PATH"

interactive_threads=(16)

for t in ${interactive_threads[@]}; do
	echo "Starting test with $t thread(s)"
	sed -i '$ d' test.spec
	echo "interactive=$t" >> test.spec
	#LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=ycsbc4.prof ./ycsbc -db nvmmiddleware -threads 40 -P ./workloads/workloada.spec -P ./test.spec
	#./ycsbc -db memcached -threads 100 -P ./workloads/workloada.spec -P ./test.spec
	./ycsbc -db nvmmiddleware -threads 100 -P ./workloads/workloada.spec -P ./test.spec
	#./ycsbc -db grpcnvmmiddleware -threads 100 -P ./workloads/workloada.spec -P ./test.spec
done
