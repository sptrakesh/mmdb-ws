#!/bin/sh

LOGDIR=/opt/spt/logs

Decompress()
{
  gzip -d /opt/spt/data/dbip.mmdb.gz
}

Defaults()
{
  if [ -z "$CACHE_DIR" ]
  then
    CACHE_DIR="/opt/spt/data"
    echo "CACHE_DIR not set.  Will default to $CACHE_DIR"
  fi

  if [ -z "$PORT" ]
  then
    PORT=8000
    echo "PORT not set.  Will default to $PORT"
  fi

  if [ -z "$THREADS" ]
  then
    THREADS=8
    echo "THREADS not set.  Will default to $THREADS"
  fi
}

Service()
{
  if [ ! -d $LOGDIR ]
  then
    mkdir -p $LOGDIR
    chown spt:spt $LOGDIR
  fi

  echo "Starting up MaxMind DB websocket server"
  /opt/spt/bin/mmdb-ws -c true -o ${LOGDIR}/ \
    -d $CACHE_DIR -p $PORT -n $THREADS \
    -a "$AUTH_KEY"
}

Decompress && Defaults && Service