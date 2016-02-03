---
layout: post
title: Apache Mesos 0.27.0 Released
permalink: /blog/mesos-0-27-0-released/
published: true
post_author:
  display_name: Kapil Arya
  twitter: karya0
tags: Release
---

The latest Mesos release, 0.27.0, is now available for [download](http://mesos.apache.org/downloads).
167+ bugfixes and improvements made it into this release.

The most significant new feature of this release is the introduction _implicit
roles_. If `--roles` is omitted during master startup, _any_ role name can be
used; controlling which principals are allowed to register as which roles should
be done using [ACLs](authorization.md). The role whitelist functionality is
still supported but is deprecated.

For full release notes with all features and bug fixes, please refer to the
[CHANGELOG](https://git-wip-us.apache.org/repos/asf?p=mesos.git;a=blob_plain;f=CHANGELOG;hb=0.27.0).

### Upgrades

Rolling upgrades from a Mesos 0.26.0 cluster to Mesos 0.27.0 are straightforward. There are just some minor, backwards compatible deprecations.
Please refer to the [upgrade guide](http://mesos.apache.org/documentation/latest/upgrades/) for detailed information on upgrading to Mesos 0.27.0.


### Try it out

We encourage you to try out this release and let us know what you think.
If you run into any issues, please let us know on the [user mailing list and IRC](https://mesos.apache.org/community).

### Thanks!

Thanks to the 55 contributors who made 0.27.0 possible:

Adam B, Alexander Rojas, Alexander Rukletsov, Alex Clemmer, Anand Mazumdar,
Andy Pang, Anindya Sinha, Artem Harutyunyan, Avinash sridharan, Bartek Plotka,
Benjamin Bannier, Benjamin Hindman, Benjamin Mahler, Bernd Mathiske,
Bhuvan Arumugam, BrickXu, Chi Zhang, Cong Wang, Dario Rexin, David Forsythe,
Diana Arroyo, Diogo Gomes, Felix Abecassis, Gilbert Song, Greg Mann,
Guangya Liu, haosdent huang, James Peach, Jan Schlicht, Jian Qiu, Jie Yu,
Jihun Kang, Jiri Simsa, Jocelyn De La Rosa, Joerg Schad, Jojy Varghese,
Joris Van Remoortere, Joseph Wu, Kapil Arya, Kevin Klues, Klaus Ma,
Mandeep Chadha, Michael Park, Neil Conway, Olivier Sallou, Qian Zhang,
Shuai Lin, Stephan Erb, Steve Hoffman, Till Toenshoff, Timothy Chen,
Vaibhav Khanduja, Vinod Kone, Vivek Juneja, Zhitao Li,
