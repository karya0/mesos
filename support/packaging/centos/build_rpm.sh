#!/bin/bash

CENTOS_VERSION=$(rpm --eval '%{centos_ver}')
MESOS_VERSION=${MESOS_TAG%[-]*}
MESOS_RELEASE=${MESOS_RELEASE:-1}

export HOME="${PWD}/centos${CENTOS_VERSION}"
mkdir -p $HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

PACKAGING_DIR=$(realpath "$(dirname "$(dirname "$0")")")
cp ${PACKAGING_DIR}/common/* $HOME/rpmbuild/SOURCES
cp ${PACKAGING_DIR}/centos/mesos.spec $HOME/rpmbuild/SPECS

if [ -z "MESOS_TAG" ]; then
  gitsha=$(git rev-parse --short HEAD)
  snapshot_version=$(date -u +'%Y%m%d')git$gitsha
  MESOS_RELEASE=0.1.pre.$snapshot_version

  MESOS_VERSION=$($PACKAGING_DIR/../../configure --version|head -1|cut -d' ' -f3)

  TMP_BUILD_DIR=`mktemp -d ./mesos-centos-rpm-build-XXXX`
  pushd $TMP_BUILD_DIR
  $PACKAGING_DIR/../../configure && make dist
  popd

  cp -f $TMP_BUILD_DIR/mesos-$MESOS_VERSION.tar.gz $HOME/rpmbuild/SOURCES/
  rm -rf $TMP_BUILD_DIR
elif [ "$MESOS_VERSION" = "$MESOS_TAG" ]; then
  curl -sSL \
    https://dist.apache.org/repos/dist/release/mesos/${MESOS_VERSION}/mesos-${MESOS_VERSION}.tar.gz \
    -o $HOME/rpmbuild/SOURCES/mesos-${MESOS_VERSION}.tar.gz
else
  curl -sSL \
    https://dist.apache.org/repos/dist/dev/mesos/${MESOS_TAG}/mesos-${MESOS_VERSION}.tar.gz \
    -o $HOME/rpmbuild/SOURCES/mesos-${MESOS_VERSION}.tar.gz
fi

if [ "$CENTOS_VERSION" = "6" ]; then
  source scl_source enable devtoolset-3
fi

rpmbuild \
  --define "MESOS_VERSION $MESOS_VERSION" \
  --define "MESOS_RELEASE $MESOS_RELEASE" \
  -ba $HOME/rpmbuild/SPECS/mesos.spec
