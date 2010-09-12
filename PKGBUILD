pkgname=dcgui
pkgver=0.5.3
pkgrel=1
pkgdesc="A Gtk+ based Devchat client."
arch=('i686' 'x86_64')
url="http://dev.yaki-syndicate.de/"
license=('GPL')
depends=('libsoup' 'gtk2' 'libnotify' 'hicolor-icon-theme' 'libxml2')
makedepends=('pkgconfig' 'clang')
source=(http://dev.yaki-syndicate.de/git/cgit.cgi/$pkgname/snapshot/$pkgname-$pkgver.tar.bz2)
md5sums=(1fa27e5688f778afabbeafc372225dec)
conflicts=('devchat-gui')
provides=('devchat-gui')

build() {
  cd ${srcdir}/${pkgname}-${pkgver}
  ./configure --prefix=/usr
  make
  make DESTDIR=${pkgdir}/usr/ install || return 1
}
