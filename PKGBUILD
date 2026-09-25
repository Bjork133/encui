# Maintainer: Bjork133 <bjorkyotun133@gmail.com>

pkgname=encui
pkgver=0.1.0
pkgrel=1
pkgdesc="Terminal UI tool for encrypting messages (AES, RSA, ChaCha20, custom)"
arch=('x86_64')
url="https://github.com/yourusername/encui"
license=('MIT')
depends=('openssl' 'libsodium' 'ncurses')
makedepends=('cmake' 'git')
source=("git+https://github.com/Bjork133/encui.git#tag=v${pkgver}")
sha256sums=('SKIP')

prepare() {
    cd "$srcdir/$pkgname-$pkgver"
    mkdir -p build
}

build() {
    cd "$srcdir/$pkgname-$pkgver/build"
    cmake ..
    make
}

package() {
    cd "$srcdir/$pkgname-$pkgver/build"
    install -Dm755 encui "$pkgdir/usr/bin/encui"
    install -Dm644 ../README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
}
