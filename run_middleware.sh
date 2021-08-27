#!/bin/bash
export LD_LIBRARY_PATH="/home/jiel/nvm-middleware/cmake/build:$LD_LIBRARY_PATH"

interactive_threads=(1)

for t in ${interactive_threads[@]}; do
	echo "Starting test with $t thread(s)"
	sed -i '$ d' test.spec
	echo "interactive=$t" >> test.spec
	./ycsbc -db nvmmiddleware -threads 1 -P ./workloads/workloada.spec -P ./test.spec
done
