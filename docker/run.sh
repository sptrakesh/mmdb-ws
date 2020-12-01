#!/bin/sh

case "$1" in
  'start')
    (cd `dirname $0`;
    docker run -d --rm \
      -e LOG_LEVEL=debug \
      -e WS_PORT=8010 \
      -e TCP_PORT=2010 \
      -v $PWD/dbip-city-lite-2020-11.mmdb.gz:/opt/spt/data/dbip.mmdb.gz:ro \
      -p 2010:2010 \
      -p 8010:8010 \
      --name mmdb-ws mmdb-ws)
    ;;
  'logs')
    docker logs mmdb-ws | tail -100
    ;;
  'stats')
    docker stats mmdb-ws
    ;;
  'stop')
    docker stop mmdb-ws
    ;;
  *)
    echo "Usage: $0 <start|logs|stats|stop>"
    exit 1
    ;;
esac
