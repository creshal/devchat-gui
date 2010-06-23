CC = gcc
LIBS = gtk+-2.0 libxml-2.0 libsoup-2.4 libnotify alsa libotr
DESTDIR = /usr/local/bin

all:
	mkdir -p _build/bin
	$(CC) `pkg-config --cflags --libs $(LIBS)` src/devchat_cb_data.c src/devchat_window.c src/main.c -o _build/bin/dcgui

clean:
	rm -rv _build

distclean: clean
	rm src/config.h

install:
	install -Dm755 _build/bin/dcgui $(DESTDIR)/bin/devchat-gui
	install -Dm644 share/applications/dcgui.desktop $(DESTDIR)/share/applications/devchat-gui.desktop
	mkdir -m755 -p ${DESTDIR}/share/pixmaps/devchat
	install -m644 share/pixmaps/devchat/* -t ${DESTDIR}/share/pixmaps/devchat
