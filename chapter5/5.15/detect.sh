#!/usr/bin/bash
ALL_FILES=$(ls /dev)
NO_SUPPORT_READ=""
NO_SUPPORT_WRITE=""
NO_SUPPORT_LSEEK=""
for f in ${ALL_FILES};do
	f=/dev/${f}
	if [ -d ${f} ];then
		continue
	fi

	echo "h" >> ${f}
	if [ $? -ne 0 ];then
		NO_SUPPORT_WRITE="${NO_SUPPORT_WRITE} ${f}"
	fi

	exec 7<${f}
	timeout 1 read -u 7
	if [ $? -ne 0 ];then
		NO_SUPPORT_READ="${NO_SUPPORT_READ} ${f}"
	fi
	exec 7<&-

	./test_lseek ${f}
	if [ $? -ne 0 ];then
		NO_SUPPORT_LSEEK="${NO_SUPPORT_LSEEK} ${f}"
	fi
done

echo "no support READ files are"
for f in ${NO_SUPPORT_READ}; do
	echo "${f}"
done

echo "no support WRITE files are"
for f in ${NO_SUPPORT_WRITE}; do
	echo "${f}"
done

echo "no support LSEEK files are"
for f in ${NO_SUPPORT_LSEEK}; do
	echo "${f}"
done



	 
