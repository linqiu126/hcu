-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: eglibc
Binary: libc-bin, libc-dev-bin, glibc-doc, eglibc-source, locales, locales-all, nscd, multiarch-support, libc6, libc6-dev, libc6-dbg, libc6-prof, libc6-pic, libc6-udeb, libc6.1, libc6.1-dev, libc6.1-dbg, libc6.1-prof, libc6.1-pic, libc6.1-udeb, libc0.3, libc0.3-dev, libc0.3-dbg, libc0.3-prof, libc0.3-pic, libc0.3-udeb, libc0.1, libc0.1-dev, libc0.1-dbg, libc0.1-prof, libc0.1-pic, libc0.1-udeb, libc6-i386, libc6-dev-i386, libc6-sparc, libc6-dev-sparc, libc6-sparc64, libc6-dev-sparc64, libc6-s390, libc6-dev-s390, libc6-amd64, libc6-dev-amd64, libc6-powerpc, libc6-dev-powerpc, libc6-ppc64, libc6-dev-ppc64, libc6-mipsn32, libc6-dev-mipsn32, libc6-mips64, libc6-dev-mips64, libc6-armhf, libc6-dev-armhf, libc6-armel, libc6-dev-armel, libc0.1-i386, libc0.1-dev-i386, libc6-x32, libc6-dev-x32, libc6-i686, libc6-xen, libc0.1-i686, libc0.3-i686, libc0.3-xen, libc6.1-alphaev67, libc6-loongson2f, libnss-dns-udeb, libnss-files-udeb
Architecture: any all
Version: 2.19-0ubuntu6.6
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Clint Adams <clint@debian.org>, Aurelien Jarno <aurel32@debian.org>, Adam Conrad <adconrad@0c3.net>
Homepage: http://www.eglibc.org
Standards-Version: 3.9.4
Vcs-Browser: http://svn.debian.org/wsvn/pkg-glibc/glibc-package/
Vcs-Svn: svn://svn.debian.org/pkg-glibc/glibc-package/
Build-Depends: gettext, dpkg-dev (>= 1.16.0), bzip2, xz-utils, file, quilt, autoconf, gawk, debhelper (>= 7.4.3), rdfind, symlinks, netbase, linux-libc-dev (>= 3.2.1) [linux-any], linux-libc-dev (>= 3.4) [amd64 i386 x32], libaudit-dev [linux-any], libcap-dev [linux-any], libselinux-dev [linux-any], mig (>= 1.3-2) [hurd-i386], hurd-dev (>= 20120520-2~) [hurd-i386], gnumach-dev (>= 2:1.4-2~) [hurd-i386], kfreebsd-kernel-headers [kfreebsd-any], binutils (>= 2.21), g++-4.8, g++-4.8-multilib [amd64 armhf armel i386 kfreebsd-amd64 mips mipsel powerpc ppc64 s390x sparc sparc64 x32]
Build-Depends-Indep: perl, po-debconf (>= 1.0)
Package-List: 
 eglibc-source deb devel optional
 glibc-doc deb doc optional
 libc-bin deb libs required
 libc-dev-bin deb libdevel optional
 libc0.1 deb libs required
 libc0.1-dbg deb debug extra
 libc0.1-dev deb libdevel optional
 libc0.1-dev-i386 deb libdevel optional
 libc0.1-i386 deb libs optional
 libc0.1-i686 deb libs extra
 libc0.1-pic deb libdevel optional
 libc0.1-prof deb libdevel extra
 libc0.1-udeb udeb debian-installer extra
 libc0.3 deb libs required
 libc0.3-dbg deb debug extra
 libc0.3-dev deb libdevel optional
 libc0.3-i686 deb libs extra
 libc0.3-pic deb libdevel optional
 libc0.3-prof deb libdevel extra
 libc0.3-udeb udeb debian-installer extra
 libc0.3-xen deb libs extra
 libc6 deb libs required
 libc6-amd64 deb libs optional
 libc6-armel deb libs optional
 libc6-armhf deb libs optional
 libc6-dbg deb debug extra
 libc6-dev deb libdevel optional
 libc6-dev-amd64 deb libdevel optional
 libc6-dev-armel deb libdevel optional
 libc6-dev-armhf deb libdevel optional
 libc6-dev-i386 deb libdevel optional
 libc6-dev-mips64 deb libdevel optional
 libc6-dev-mipsn32 deb libdevel optional
 libc6-dev-powerpc deb libdevel optional
 libc6-dev-ppc64 deb libdevel optional
 libc6-dev-s390 deb libdevel optional
 libc6-dev-sparc deb libdevel optional
 libc6-dev-sparc64 deb libdevel optional
 libc6-dev-x32 deb libdevel optional
 libc6-i386 deb libs optional
 libc6-i686 deb libs extra
 libc6-loongson2f deb libs extra
 libc6-mips64 deb libs optional
 libc6-mipsn32 deb libs optional
 libc6-pic deb libdevel optional
 libc6-powerpc deb libs optional
 libc6-ppc64 deb libs optional
 libc6-prof deb libdevel extra
 libc6-s390 deb libs optional
 libc6-sparc deb libs optional
 libc6-sparc64 deb libs optional
 libc6-udeb udeb debian-installer extra
 libc6-x32 deb libs optional
 libc6-xen deb libs extra
 libc6.1 deb libs required
 libc6.1-alphaev67 deb libs extra
 libc6.1-dbg deb debug extra
 libc6.1-dev deb libdevel optional
 libc6.1-pic deb libdevel optional
 libc6.1-prof deb libdevel extra
 libc6.1-udeb udeb debian-installer extra
 libnss-dns-udeb udeb debian-installer extra
 libnss-files-udeb udeb debian-installer extra
 locales deb localization standard
 locales-all deb localization extra
 multiarch-support deb libs required
 nscd deb admin optional
Checksums-Sha1: 
 27abcacff191d32d986d108adc4f9cdf8b242a9b 12610800 eglibc_2.19.orig.tar.xz
 e2077fd2fcc6d482c5234c2c0e24fa261c944270 1135296 eglibc_2.19-0ubuntu6.6.debian.tar.xz
Checksums-Sha256: 
 e5d30be72b702dffae527779af1be755f0dfbf13c171998a04f7265cd4da131f 12610800 eglibc_2.19.orig.tar.xz
 28566dd4d41611fb053d8420dc79bf50782ec58ca1557ae19af8b54d7cd1ddec 1135296 eglibc_2.19-0ubuntu6.6.debian.tar.xz
Files: 
 22847c6f3084167ed22b757a7a34bde5 12610800 eglibc_2.19.orig.tar.xz
 25873a2a5dd5948d19af2d4f0b29b5b9 1135296 eglibc_2.19-0ubuntu6.6.debian.tar.xz
Original-Maintainer: GNU Libc Maintainers <debian-glibc@lists.debian.org>
Testsuite: autopkgtest

-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQIcBAEBCgAGBQJU7e3WAAoJEGVp2FWnRL6Tgx8QAK/eNe6IpunBumCJ5Hmt7t4/
AmfAYXaQ1rjzHXJL5XYfi7ZsUK0l73wqO9C3nLUNdHOoZc7Co4mVkkj59bUMQzE7
J5oLY8btOYpgAejuIy8RiDxC68hZM2QIj3GCf5LbLHnjOaCX9wL0UbUIwswetWnh
jttF7dpeJlMBEmaC5vpIRXB3x1PtmnpJUc62YPkGv8cj8zmYZG9DZdyzB5HRcn6F
MYth3wDwO3cfqBfS8+QqpwwnE6+spt6tioLc2MhsIRF4n6+/3vVBRBtygtYGJj+h
j/PyCsjvUCIR9MzyVyn6cMft0lP5RrE1AI83FtqY4uSDQ3ywRofIP4OJJ2vhJcX6
+2jgJnXku+5Fo17VQN0DIvXxHHIJrNFiCH7R+59i4IN4l7ndbpiV3dFGyvQLEJR1
kpeK6J0kf6rVcBv2FyXjU/49zeUqqltqEIjYljn4/yisOSWVNylU012Xc+oFeka9
7SZBDSS+z8UWLH+LucUpnoIDtwBwbKJu25LPjP0+eQxZqI+UoKhuwFjEd7PIByw+
jPthQ63GEDYf+PTfACEAaw1pcyEmzjLFR6vKj9i9racPRNwO04eqEOqJzxGwV3Tm
EvAW/6XNHFLE4LkXdS/kX7/S11t3oc9nxMV5mNAEw1H4MMFru2ue+CIrXVWWC0oY
TU/xZNCs8DCi13geNA2H
=5wBV
-----END PGP SIGNATURE-----
