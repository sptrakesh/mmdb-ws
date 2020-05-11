#!/bin/sh

docker run -d --rm \
  -p 8010:8010 \
  -e AUTH_KEY="abc123" \
  --name mmdb-ws mmdb-ws