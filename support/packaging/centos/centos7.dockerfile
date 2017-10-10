FROM centos:7
MAINTAINER Kapil Arya <kapil@apache.org>

# Get curl.
RUN yum install -y              \
      curl                      \
      epel-release              \
      redhat-rpm-config         \
      rpm-build

# Add the Subversion repo.
RUN echo -e '[WANdiscoSVN]\n\
name=WANdisco SVN Repo 1.9\n\
enabled=1\n\
baseurl=http://opensource.wandisco.com/centos/7/svn-1.9/RPMS/\$basearch/\n\
gpgcheck=1\n\
gpgkey=http://opensource.wandisco.com/RPM-GPG-KEY-WANdisco' \
>> /etc/yum.repos.d/wandisco-svn.repo

# Add the Apache Maven repo.
RUN curl -sSL \
      http://repos.fedorapeople.org/repos/dchen/apache-maven/epel-apache-maven.repo \
      -o /etc/yum.repos.d/epel-apache-maven.repo

# Setup JDK
RUN echo -e 'export JAVA_HOME=/usr/lib/jvm/java-openjdk' >> /etc/profile.d/java-home.sh

# Remove 'centos' from the rpm version string.
RUN sed -i -e's:.el7.centos:.el7:' /etc/rpm/macros.dist

ADD mesos.spec /mesos.spec

RUN yum makecache && \
    yum-builddep -y /mesos.spec
