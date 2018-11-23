#!/usr/bin/env python
#
# System tray notifier for sboui updates. Source code adapted from
# salix-update-notifier by George Vlahavas (gapan).

import gtk
import sys

def accept(data=None):
    sys.exit(0)

def dismiss(data=None):
    sys.exit(1)

def quit(data=None):
    sys.exit(2)

def make_menu(event_button, event_time, data=None):
    menu = gtk.Menu()

    accept_image = gtk.Image()
    accept_image.set_from_icon_name('system-run', gtk.ICON_SIZE_MENU)
    accept_item = gtk.ImageMenuItem('Launch sboui updater')
    accept_item.set_image(accept_image)

    dismiss_image = gtk.Image()
    dismiss_image.set_from_icon_name('window-close', gtk.ICON_SIZE_MENU)
    dismiss_item = gtk.ImageMenuItem('Ignore for now')
    dismiss_item.set_image(dismiss_image)

    quit_image = gtk.Image()
    quit_image.set_from_icon_name('application-exit', gtk.ICON_SIZE_MENU)
    quit_item = gtk.ImageMenuItem('Don\'t remind me again')
    quit_item.set_image(quit_image)

    menu.append(accept_item)
    menu.append(dismiss_item)
    menu.append(quit_item)
    accept_item.connect_object("activate", accept, "Accept")
    accept_item.show()
    dismiss_item.connect_object("activate", dismiss, "Dismiss")
    dismiss_item.show()
    quit_item.connect_object("activate", quit, "Quit")
    quit_item.show()
    menu.popup(None, None, None, event_button, event_time)

def on_right_click(data, event_button, event_time):
    make_menu(event_button, event_time)

def on_left_click(event):
    accept()

if __name__ == '__main__':
    icon = gtk.status_icon_new_from_icon_name("sboui")
    icon.set_tooltip_text('SBo updates are available')
    icon.connect('popup-menu', on_right_click)
    icon.connect('activate', on_left_click)
    gtk.main()
