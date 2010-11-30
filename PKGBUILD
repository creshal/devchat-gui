pkgname=dcgui
pkgver=0.6.4
pkgrel=1
pkgdesc="A Gtk+ based Devchat client."
arch=('i686' 'x86_64')
url="http://dev.yaki-syndicate.de/"
license=('GPL')
depends=('libsoup' 'gtk2' 'hicolor-icon-theme' 'libxml2' 'gtkspell' 'libnotify')
makedepends=('pkgconfig' 'python')
source=(http://dev.yaki-syndicate.de/git/cgit.cgi/$pkgname/snapshot/$pkgname-$pkgver.tar.bz2)
md5sums=(fc95e26ba9a2a563fab109178cf0a0b6)
conflicts=('devchat-gui')
provides=('devchat-gui')

build() {
  cd ${srcdir}/${pkgname}-${pkgver}
  ./configure --prefix=/usr
  make
  make DESTDIR=$pkgdir/usr install || return 1
}
