#!/bin/bash

# 检查参数
if [ $# -ne 1 ]; then
    echo "用法: $0 <用户列表文件>" >&2
    exit 1
fi

USER_FILE="$1"
if [ ! -f "$USER_FILE" ]; then
    echo "错误: 文件 '$USER_FILE' 不存在" >&2
    exit 1
fi

# 提取用户名（忽略空行和注释）
get_usernames() {
    grep -v '^\s*$\|^\s*#' "$USER_FILE" | sort -u
}

# 获取当前登录用户（只取用户名部分）
get_logged_in_users() {
    who | awk '{print $1}' | sort -u
}

# 初始化
get_logged_in_users > prev_logins
touch curr_logins

while true; do
    sleep 6
    get_logged_in_users > curr_logins
    
    # 提取关注的用户状态
    comm -12 <(get_usernames) prev_logins > prev_filtered
    comm -12 <(get_usernames) curr_logins > curr_filtered
    
    # 计算状态变化
    logged_out=$(comm -23 prev_filtered curr_filtered)
    logged_in=$(comm -13 prev_filtered curr_filtered)
    
    # 只在有变化时输出
    if [ -n "$logged_out" ] || [ -n "$logged_in" ]; then
        echo "--- $(date) ---"
        [ -n "$logged_out" ] && echo "已注销: $(echo "$logged_out" | tr '\n' ' ')"
        [ -n "$logged_in" ] && echo "新登录: $(echo "$logged_in" | tr '\n' ' ')"
        echo
    fi
    
    mv curr_logins prev_logins
done
