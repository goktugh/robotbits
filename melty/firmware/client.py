#!/usr/bin/env python3

import socket
import os
import sys
import curses

def connect(addr):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect( (addr, 4200) )
    return s

def send_message(spinspeed, sock):
    msg = 'M{}\n'.format(spinspeed)
    msg = msg.encode('ascii')
    sock.send(msg)

def draw(stdscr, addr):
    sock = connect(addr)
    stdscr.clear()
    
     # Start colors in curses
    curses.start_color()
    curses.init_pair(1, curses.COLOR_CYAN, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_RED, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)
    
    # Set delay
    curses.halfdelay(2) # Tenths of a second
    
    stdscr.addstr(0,0, "Melty client: 0 or space=stop, 1-5= Set speed*100, uparrow = +25 downarrow=-25", 
        curses.color_pair(1))
    stdscr.addstr(1,0, "Address: {}".format(addr),
        curses.color_pair(1))
    stdscr.refresh()
    
    key = 0
    spinspeed = 0
    ticks = 0
    
    while (key != ord('q')):
        stdscr.addstr(3,0, "Speed={:6d} ticks={:06d}".format(spinspeed, ticks))
        stdscr.refresh()
        key = stdscr.getch()
        if key == curses.KEY_UP:
            spinspeed += 25
        if key == curses.KEY_DOWN:
            spinspeed -= 25
        if key == ord(' ' ):
            spinspeed = 0
        # Set speed n*100
        if ord('0') <= key <= ord('5'):
            spinspeed = int(chr(key)) * 100
            
        if spinspeed < 0:
            spinspeed = 0
        send_message(spinspeed,sock) 
        ticks += 1
    send_message(0, sock)
        
if __name__ == '__main__':
    curses.wrapper(draw, sys.argv[1])
        
