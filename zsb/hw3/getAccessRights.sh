#!/bin/bash

# Change the path to the directory according to the location of the script
ACL_DEMO="./acl_demo"

getfacl "${ACL_DEMO}"
getfacl "${ACL_DEMO}/public"
getfacl "${ACL_DEMO}/private"
