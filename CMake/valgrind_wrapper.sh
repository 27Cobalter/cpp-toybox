#!/usr/bin/env bash

TEST_NAME=$(echo "$@" | grep -oP '\-\-gtest_filter=\K[^[:space:]\-]+')

args_=()
for arg in "$@"; do
      arg_="${arg/\$\{REPLACE_TEST_NAME\}/$TEST_NAME}"
      args_+=("$arg_")
      echo "$arg -> $arg_"
done
echo "Wrapper: valgrind ${args_[@]}"
valgrind ${args_[@]}
