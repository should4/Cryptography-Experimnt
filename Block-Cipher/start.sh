#!/bin/bash

# 检查是否存在 bin 目录，如果不存在则创建
if [ ! -d "bin" ]; then
  mkdir bin
fi

# 进入 test 目录
cd test

# 执行 make 命令
make
