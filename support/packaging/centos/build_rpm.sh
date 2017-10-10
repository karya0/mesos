#!/bin/bash

MESOS_DIR="${MESOS_DIR:-/mesos}"
WORK_DIR="${WORK_DIR:-/mesos}"
# Get curl.
yum install -y wget curl unzip rpm-build redhat-rpm-config


# Add the Extra packages for enterprise linux repo.
yum install -y epel-release

# Add the Subversion repo.
cat > /etc/yum.repos.d/wandisco-svn.repo <<EOF
[WANdiscoSVN]
name=WANdisco SVN Repo 1.9
enabled=1
baseurl=http://opensource.wandisco.com/centos/7/svn-1.9/RPMS/\$basearch/
gpgcheck=1
gpgkey=http://opensource.wandisco.com/RPM-GPG-KEY-WANdisco
EOF

# Add the Apache Maven repo.
curl -sSL \
  http://repos.fedorapeople.org/repos/dchen/apache-maven/epel-apache-maven.repo \
      -o /etc/yum.repos.d/epel-apache-maven.repo

yum makecache

yum-builddep -y ${MESOS_DIR}/support/packaging/centos/mesos.spec

whoami
set -x
MESOS_VERSION=${MESOS_TAG%[-]*}
CENTOS_VERSION=$(rpm --eval '%{centos_ver}')

echo -e 'export JAVA_HOME=/usr/lib/jvm/java-openjdk' >> /etc/profile.d/java-home.sh

export HOME=$WORK_DIR/centos${CENTOS_VERSION}
mkdir -p $HOME

mkdir -p $HOME/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

cp ${MESOS_DIR}/support/packaging/common/* $HOME/rpmbuild/SOURCES
cp ${MESOS_DIR}/support/packaging/centos/mesos.spec $HOME/rpmbuild/SPECS

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
