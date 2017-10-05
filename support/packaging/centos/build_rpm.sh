#!/bin/bash

yum install -y rpm-build redhat-rpm-config
yum-builddeps -y /mesos/support/packaging/centos/mesos.spec
whoami
set -x
MESOS_VERSION=${MESOS_TAG%[-]*}
CENTOS_VERSION=$(rpm --eval '%{centos_ver}')

echo -e 'export JAVA_HOME=/usr/lib/jvm/java-openjdk' >> /etc/profile.d/java-home.sh

export HOME=$PWD/centos${CENTOS_VERSION}
mkdir -p $HOME

mkdir -p $HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

cp support/packaging/common/* $HOME/rpmbuild/SOURCES
cp support/packaging/centos/mesos.spec $HOME/rpmbuild/SPECS

pushd $HOME/rpmbuild/SOURCES
if [ "$MESOS_VERSION" = "$MESOS_TAG" ]; then
  echo wget -nv https://dist.apache.org/repos/dist/release/mesos/${MESOS_VERSION}/mesos-${MESOS_VERSION}.tar.gz
else
  echo wget -nv https://dist.apache.org/repos/dist/dev/mesos/${MESOS_TAG}/mesos-${MESOS_VERSION}.tar.gz
fi
popd

if [ "$CENTOS_VERSION" = "6" ]; then
  source scl_source enable devtoolset-3
fi

rpmbuild \
  --define "MESOS_VERSION $MESOS_VERSION" \
  --define "MESOS_RELEASE $MESOS_RELEASE" \
  -ba $HOME/rpmbuild/SPECS/mesos.spec
