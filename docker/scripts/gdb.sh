#!/bin/sh

LOGDIR=/opt/spt/logs
WS_PORT=8010
TCP_PORT=2010
THREADS=4
gdb -ex run --args /opt/spt/bin/mmdb-ws --dir ${LOGDIR}/ \
    --websocket-port $WS_PORT --tcp-port $TCP_PORT --threads $THREADS \
    --file /opt/spt/data/dbip.mmdb --log-level debug
