# pip install pyftpdlib
import subprocess
import os
import sys
import time
import threading
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer

def main():
    if len(sys.argv) < 2:
        print("Usage: python ftp_test.py <ftp_root_directory> <TsugaruExe> <args> ....")
        return 1
        
    # Grab the folder path passed by CMake/CTest
    ftp_root = os.path.abspath(sys.argv[1])
    if not os.path.isdir(ftp_root):
        print("Root directory does not exist.")
        return 1

    print(f"--> Hosting FTP root at: {ftp_root}")

    # Set up the server permissions
    authorizer = DummyAuthorizer()
    # testuser / password pointing directly to your CMake directory
    authorizer.add_user("testuser", "password", ftp_root, perm="elradfmwMT")
    
    handler = FTPHandler
    handler.authorizer = authorizer
    handler.passive_ports = range(30000, 30005) # Keeps your C++ NAT mapping predictable

    # Spin up the server on localhost port 2121
    server = FTPServer(("127.0.0.1", 2121), handler)
    
    # Serve in a background thread so your main script remains responsive
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server_thread.start()
    time.sleep(0.5) # Quick pause to let the socket bind
    
    print("--> FTP Server running on 127.0.0.1:2121")
    

    print(sys.argv[2:])

    proc=subprocess.Popen(sys.argv[2:])
    proc.communicate()
    returnCode=proc.returncode

    server.close_all()

    return returnCode





if __name__ == "__main__":
    sys.exit(main())

