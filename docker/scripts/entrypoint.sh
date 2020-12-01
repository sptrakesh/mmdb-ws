#!/bin/sh

LOGDIR=/opt/spt/logs
DBFILE=/opt/spt/data/dbip.mmdb

Decompress()
{
  if [ -f /opt/spt/data/dbip.mmdb.gz ]
  then
    gzip -dc /opt/spt/data/dbip.mmdb.gz > $DBFILE
  else
    echo "DB IP file not found.  Make sure it is mounted for the running container."
    exit 1
  fi
}

Defaults()
{
  if [ -z "$WS_PORT" ]
  then
    WS_PORT=8010
    echo "WS_PORT not set.  Will default to $WS_PORT"
  fi

  if [ -z "$TCP_PORT" ]
  then
    TCP_PORT=2010
    echo "TCP_PORT not set.  Will default to $TCP_PORT"
  fi

  if [ -z "$THREADS" ]
  then
    THREADS=4
    echo "THREADS not set.  Will default to $THREADS"
  fi

  if [ -z "$LOG_LEVEL" ]
  then
    LOG_LEVEL='info'
    echo "LOG_LEVEL not set.  Will default to $LOG_LEVEL"
  fi
}

Service()
{
  if [ ! -d $LOGDIR ]
  then
    mkdir -p $LOGDIR
    chown spt:spt $LOGDIR
  fi

  echo "Starting up MaxMind DB tcp and websocket server"
  /opt/spt/bin/mmdb-ws --console true --dir ${LOGDIR}/ --log-level $LOG_LEVEL \
    --tcp-port $TCP_PORT \
    --websocket-port $WS_PORT \
    --threads $THREADS --file $DBFILE
}

Decompress && Defaults && Service