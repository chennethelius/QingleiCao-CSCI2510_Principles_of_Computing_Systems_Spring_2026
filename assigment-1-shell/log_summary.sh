#!/bin/bash

LINES=$(wc -l < access.log)

echo "LINES: $LINES"

echo "STATUS_COUNTS:"
awk '{print $2}' access.log | sort | uniq -c | sort -nr | awk '{print $2" "$1}'

