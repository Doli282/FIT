#!/bin/bash

# set this variable to the path of the acl_demo directory
ACL_DEMO="/home/doli/zsb/hw3/acl_demo"

function printUsage()
{
	echo "$0: usage $0 userName rights file"
	echo "	file ... 'private'/'protected'"
	echo "	rights: sum of: 1-execute, 2-write, 4-read"  
	exit 1
}

if [ $# -ne 3 ];then
	echo "$0: wrong number of arguments. Got '$#'"
	printUsage
fi

if [ $2 -gt 6 -o $2 -lt 1 ]; then
	echo "$0 wrong access rights"
	printUsage
fi

FILE="${ACL_DEMO}/$3"
if [ ! -e "$FILE" ]; then
	echo "$0 invalid file $FILE"
	printUsage
fi

setfacl -m "user:$1:$2" "$FILE"
