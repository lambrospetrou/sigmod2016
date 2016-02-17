#!/bin/bash

pushd ../test-harness/; ./harness init-file.txt workload-file.txt result-file.txt ../reference-python/run.sh; popd;
