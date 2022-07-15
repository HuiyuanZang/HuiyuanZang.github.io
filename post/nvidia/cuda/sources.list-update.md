# Update /etc/apt/sources.list for Jeston Nano
![](../../../asset/nvidia/img/nvidia-icon.png)

When you do `$ sudo apt-get update` on Jetson nano, you possibly meet the update errors like: 

```
Ign:35 http://ports.ubuntu.com/ubuntu-ports bionic-security/multiverse amd64 Packages
Err:7 http://ports.ubuntu.com/ubuntu-ports bionic/main amd64 Packages
  404  Not Found [IP: 185.125.190.39 80]
Ign:15 http://ports.ubuntu.com/ubuntu-ports bionic/restricted amd64 Packages
Ign:16 http://ports.ubuntu.com/ubuntu-ports bionic/universe amd64 Packages
Ign:17 http://ports.ubuntu.com/ubuntu-ports bionic/multiverse amd64 Packages
Err:18 http://ports.ubuntu.com/ubuntu-ports bionic-updates/main amd64 Packages
  404  Not Found [IP: 185.125.190.39 80]
Ign:19 http://ports.ubuntu.com/ubuntu-ports bionic-updates/restricted amd64 Packages
Ign:20 http://ports.ubuntu.com/ubuntu-ports bionic-updates/universe amd64 Packages
Ign:29 http://ports.ubuntu.com/ubuntu-ports bionic-updates/multiverse amd64 Packages
Err:30 http://ports.ubuntu.com/ubuntu-ports bionic-backports/main amd64 Packages
  404  Not Found [IP: 185.125.190.39 80]
Ign:31 http://ports.ubuntu.com/ubuntu-ports bionic-backports/universe amd64 Packages
Err:32 http://ports.ubuntu.com/ubuntu-ports bionic-security/main amd64 Packages
  404  Not Found [IP: 185.125.190.39 80]
Ign:33 http://ports.ubuntu.com/ubuntu-ports bionic-security/restricted amd64 Packages
Ign:34 http://ports.ubuntu.com/ubuntu-ports bionic-security/universe amd64 Packages
Ign:35 http://ports.ubuntu.com/ubuntu-ports bionic-security/multiverse amd64 Packages
Fetched 257 kB in 7s (36.8 kB/s)
Reading package lists... Done
N: Skipping acquire of configured file 'main/binary-amd64/Packages' as repository 'https://repo.download.nvidia.com/jetson/common r32.7 InRelease' doesn't support architecture 'amd64'
N: Skipping acquire of configured file 'main/binary-amd64/Packages' as repository 'https://repo.download.nvidia.com/jetson/t210 r32.7 InRelease' doesn't support architecture 'amd64'
E: Failed to fetch http://ports.ubuntu.com/ubuntu-ports/dists/bionic/main/binary-amd64/Packages  404  Not Found [IP: 185.125.190.39 80]
E: Failed to fetch http://ports.ubuntu.com/ubuntu-ports/dists/bionic-updates/main/binary-amd64/Packages  404  Not Found [IP: 185.125.190.39 80]
E: Failed to fetch http://ports.ubuntu.com/ubuntu-ports/dists/bionic-backports/main/binary-amd64/Packages  404  Not Found [IP: 185.125.190.39 80]
E: Failed to fetch http://ports.ubuntu.com/ubuntu-ports/dists/bionic-security/main/binary-amd64/Packages  404  Not Found [IP: 185.125.190.39 80]
E: Some index files failed to download. They have been ignored, or old ones used instead.

```

Lookint at /etc/apt/sources.list, that is obviousely outdated.

```
deb http://ports.ubuntu.com/ubuntu-ports/ bionic main restricted
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic main restricted

## Major bug fix updates produced after the final release of the
## distribution.
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-updates main restricted
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-updates main restricted

## N.B. software from this repository is ENTIRELY UNSUPPORTED by the Ubuntu
## team. Also, please note that software in universe WILL NOT receive any
## review or updates from the Ubuntu security team.
deb http://ports.ubuntu.com/ubuntu-ports/ bionic universe
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic universe
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-updates universe
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-updates universe

## N.B. software from this repository is ENTIRELY UNSUPPORTED by the Ubuntu
## team, and may not be under a free licence. Please satisfy yourself as to
## your rights to use the software. Also, please note that software in
## multiverse WILL NOT receive any review or updates from the Ubuntu
## security team.
deb http://ports.ubuntu.com/ubuntu-ports/ bionic multiverse
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic multiverse
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-updates multiverse
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-updates multiverse

## N.B. software from this repository may not have been tested as
## extensively as that contained in the main release, although it includes
## newer versions of some applications which may provide useful features.
## Also, please note that software in backports WILL NOT receive any review
## or updates from the Ubuntu security team.
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-backports main restricted universe multiverse
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-backports main restricted universe multiverse

## Uncomment the following two lines to add software from Canonical's
## 'partner' repository.
## This software is not part of Ubuntu, but is offered by Canonical and the
## respective vendors as a service to Ubuntu users.
# deb http://archive.canonical.com/ubuntu bionic partner
# deb-src http://archive.canonical.com/ubuntu bionic partner

deb http://ports.ubuntu.com/ubuntu-ports/ bionic-security main restricted
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-security main restricted
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-security universe
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-security universe
deb http://ports.ubuntu.com/ubuntu-ports/ bionic-security multiverse
# deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic-security multiverse


```

Try to replace it with the following one

```
deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic main restricted universe multiverse
deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-updates main restricted universe multiverse
deb [arch=amd64,i386] http://us.archive.ubuntu.com/ubuntu/ bionic-backports main restricted universe multiverse
deb [arch=amd64,i386] http://security.ubuntu.com/ubuntu bionic-security main restricted universe multiverse

deb [arch=arm64,armhf,ppc64el,s390x] http://ports.ubuntu.com/ubuntu-ports/ bionic main restricted universe multiverse
deb [arch=arm64,armhf,ppc64el,s390x] http://ports.ubuntu.com/ubuntu-ports/ bionic-updates main restricted universe multiverse
deb [arch=arm64,armhf,ppc64el,s390x] http://ports.ubuntu.com/ubuntu-ports/ bionic-backports main restricted universe multiverse
deb [arch=arm64,armhf,ppc64el,s390x] http://ports.ubuntu.com/ubuntu-ports/ bionic-security main restricted universe multiverse
```