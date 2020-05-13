#!/bin/sh

case "$1" in
  'start')
    (cd `dirname $0`;
    docker run -d --rm \
      -v $PWD/dbip-city-lite-2020-05.mmdb.gz:/opt/spt/data/dbip.mmdb.gz:ro \
      -p 8010:8010 \
      -e AUTH_KEY="abc123" \
      --name mmdb-ws mmdb-ws)
    ;;
  'logs')
    docker logs mmdb-ws
    ;;
  'stop')
    docker stop mmdb-ws
    ;;
  *)
    echo "Usage: $0 <start|logs|stop>"
    exit 1
    ;;
esac
