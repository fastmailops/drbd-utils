# DRBD Utils

This repository contains the user space utilities for DRBD.

DRBD, developed by [LINBIT](https://www.linbit.com), is a software that allows RAID 1 functionality over
TCP/IP and RDMA for GNU/Linux. DRBD is a block device which is designed to build high availability clusters and
software defined storage by providing a virtual shared device which keeps disks in nodes synchronised using
TCP/IP or RDMA. This simulates RAID 1 but avoids the use of uncommon hardware (shared SCSI buses or Fibre Channel).

# Using DRBD
Please read the user-guide provided at [docs.linbit.com](https://docs.linbit.com).

# Support
For further products and professional support, please
[contact](http://links.linbit.com/support) us.

# Contributing
Development is coordinated via [mailing lists](http://lists.linbit.com). Currently, we do not intend to use
github issue tracking/github PRs.

# Releases
Releases generated by git tags on github are snapshots of the git repository at the given time. You most
likely do not want to use these. They might lack things such as generated man pages, the `configure` script,
and other generated files. If you want to build from a tarball, use the ones [provided by us](https://www.linbit.com/en/drbd-community/drbd-download/).
