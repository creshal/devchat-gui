pkgname=dcgui
pkgver=1.0.4
pkgrel=1
pkgdesc="A Gtk+ based Devchat client."
arch=('i686' 'x86_64')
url="http://dev.yaki-syndicate.de/"
license=('GPL')
depends=('libsoup' 'gtk2' 'libxml2' 'gtkspell' 'libnotify')
makedepends=('pkgconfig' 'python')
source=(http://dev.yaki-syndicate.de/git/cgit.cgi/$pkgname/snapshot/$pkgname-$pkgver.tar.bz2)
md5sums=('b8b592ad26ecaba1dd40221e8ec0376a')
conflicts=('devchat-gui')
provides=('devchat-gui')
build() {
  cd ${srcdir}/${pkgname}-${pkgver}
  ./configure --prefix=/usr
  make
  make DESTDIR=$pkgdir/usr install || return 1
}

