#!/bin/bash

cmd=`wc -l $1 | awk '{print $1}'`
echo "LINES: $cmd"
