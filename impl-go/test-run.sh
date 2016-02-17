#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

pushd ../test-harness/; ./harness init-file.txt workload-file.txt result-file.txt $DIR/run.sh; popd;
