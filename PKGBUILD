pkgname=dcgui
pkgver=1.0.6
pkgrel=1
pkgdesc="A Gtk+ based Devchat client."
arch=('i686' 'x86_64')
url="http://dev.yaki-syndicate.de/"
license=('GPL')
depends=('libsoup' 'gtk2' 'libxml2' 'gtkspell' 'libnotify')
makedepends=('pkgconfig' 'python')
source=(http://dev.yaki-syndicate.de/git/cgit.cgi/$pkgname/snapshot/$pkgname-$pkgver.tar.bz2)
md5sums=('98f6b6cb44b622fd87f25076633db6aa')
conflicts=('devchat-gui')
provides=('devchat-gui')
build() {
  cd ${srcdir}/${pkgname}-${pkgver}
  ./configure --prefix=/usr
  make
  make DESTDIR=$pkgdir/usr install || return 1
}

