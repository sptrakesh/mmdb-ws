#!/bin/sh

cd `dirname $0`/..
. docker/env.sh

Docker()
{
  docker buildx build --platform linux/amd64 --compress --force-rm -f docker/Dockerfile --push -t sptrakesh/$NAME:$VERSION .
  docker pull sptrakesh/$NAME:$VERSION
  docker tag sptrakesh/$NAME:$VERSION $NAME
  docker tag sptrakesh/$NAME:$VERSION sptrakesh/$NAME:latest
  docker push sptrakesh/$NAME:latest
  docker buildx build --platform linux/arm64,linux/amd64 --compress --force-rm -f docker/Dockerfile --push -t sptrakesh/$NAME:$VERSION .
}

Docker
