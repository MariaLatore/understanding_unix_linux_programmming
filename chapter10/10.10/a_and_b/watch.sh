#!/bin/bash

# 检查是否提供了用户列表文件参数
if [ $# -ne 1 ]; then
  echo "用法: $0 <用户列表文件>"
  exit 1
fi

USER_FILE="$1"

# 确保用户列表文件存在
if [ ! -f "$USER_FILE" ]; then
  echo "错误: 文件 '$USER_FILE' 不存在"
  exit 1
fi

# 初始化监控
who | sort > prev
touch curr

while true; do
  sleep 6
  who | sort > curr
  
  # 提取关注用户的当前和之前状态
  grep -f "$USER_FILE" prev > prev_filtered 2>/dev/null || true
  grep -f "$USER_FILE" curr > curr_filtered 2>/dev/null || true
  
  # 计算登录和注销的用户
  logged_out=$(comm -23 prev_filtered curr_filtered)
  logged_in=$(comm -13 prev_filtered curr_filtered)
  
  # 只有在有变化时才输出
  if [ -n "$logged_out" ] || [ -n "$logged_in" ]; then
    echo "--- $(date) ---"
    [ -n "$logged_out" ] && echo -e "Logged out:\n$logged_out"
    [ -n "$logged_in" ] && echo -e "Logged in:\n$logged_in"
    echo
  fi
  
  mv curr prev
done
