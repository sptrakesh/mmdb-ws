#!/bin/sh

case "$1" in
  'start')
    docker run -d --rm \
      -p 8010:8010 \
      -e AUTH_KEY="abc123" \
      --name mmdb-ws mmdb-ws
    ;;
  'stop')
    docker stop mmdb-ws
    ;;
  *)
    echo "Usage: $0 <start|stop>"
    exit 1
    ;;
esac
