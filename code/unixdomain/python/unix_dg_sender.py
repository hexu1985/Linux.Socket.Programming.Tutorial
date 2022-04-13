#!/usr/bin/env python3
# Python Network Programming Cookbook -- Chapter - 3
# This program is optimized for Python 2.7.
# It may run on any other version with/without modifications.

import socket
import sys

SERVER_PATH = "/tmp/python_unix_socket_server"

def run_unix_domain_socket_client():
    """ Run "a Unix domain socket client """
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    
    # Connect the socket to the path where the server is listening
    server_address = SERVER_PATH 
    print("connecting to %s" % server_address)
    try:
        sock.connect(server_address)
    except socket.error as msg:
        print(msg, file=sys.stderr)
        sys.exit(1)
    
    try:
        while True:
            message = input("Insert message(q to quit): ");
            if message[0] == 'q' or message[0] == 'Q':
                break
            sock.sendall(message.encode('ascii'))
    
    finally:
        print("Closing client")
        sock.close()

if __name__ == '__main__':
    if len(sys.argv) == 2:
        SERVER_PATH = sys.argv[1]
    run_unix_domain_socket_client()
