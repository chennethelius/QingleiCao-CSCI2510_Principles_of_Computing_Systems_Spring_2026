#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <logfile>"
    exit 1
fi

LOGFILE=$1

LINES=$(wc -l < "$LOGFILE")

echo "LINES: $LINES"

echo "STATUS_COUNTS:"
awk '{print $2}' "$LOGFILE" | sort | uniq -c | sort -nr | awk '{print $2" "$1}'

