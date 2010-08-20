pkgname=dcgui
pkgver=0.4.2
pkgrel=1
pkgdesc="A Gtk+ based Devchat client."
arch=('i686' 'x86_64')
url="http://dev.yaki-syndicate.de/"
license=('GPL')
depends=('libsoup' 'gtk2' 'libnotify' 'hicolor-icon-theme' 'libxml2')
makedepends=('pkgconfig' 'clang')
source=(http://dev.yaki-syndicate.de/git/cgit.cgi/$pkgname/snapshot/$pkgname-$pkgver.tar.bz2)
md5sums=(9cb457b6cb51fbab5e3fb05bb50561c0)
conflicts=('devchat-gui')
provides=('devchat-gui')

build() {
  cd ${srcdir}/${pkgname}-${pkgver}
  ./configure --prefix=/usr
  make
  make DESTDIR=${pkgdir}/usr/ install || return 1
}
