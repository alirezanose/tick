# maintener: Reza <alirezanose>
pkgname=tick
pkgver=1.0.0
pkgrel=1
pkgdesc="A lightweight terminal countdown, stopwatch, and pomodoro timer with audio clock ticks"
arch=('x86_64' 'aarch64' 'armv7h')
url='https://github.com/alirezanose/tick'
license=('GPL-3.0-or-later')
depends=('ncurses' 'alsa-utils')
makedepends=('gcc' 'make')
source=("$pkgname-$pkgver.tar.gz::$url/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$pkgname-$pkgver"
    make
}

package() {
    cd "$pkgname-$pkgver"
    make install PREFIX=/usr DESTDIR="$pkgdir"
}
