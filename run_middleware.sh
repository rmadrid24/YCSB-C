#!/bin/bash
export LD_LIBRARY_PATH="/home/jiel/nvm-middleware/cmake/build:/home/jiel/HdrHistogram_c/cmake/build/src:$LD_LIBRARY_PATH"

interactive_threads=(1)

for t in ${interactive_threads[@]}; do
	echo "Starting test with $t thread(s)"
	sed -i '$ d' test.spec
	echo "interactive=$t" >> test.spec
	#LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=ycsbc4.prof ./ycsbc -db nvmmiddleware -threads 40 -P ./workloads/workloada.spec -P ./test.spec
	./ycsbc -db nvmmiddleware -threads 200 -P ./workloads/workloada.spec -P ./test.spec
done
