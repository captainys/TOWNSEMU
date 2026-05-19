import os
import sys
import time
import threading
import subprocess
from http.server import SimpleHTTPRequestHandler, HTTPServer

def main():
    if len(sys.argv) < 2:
        print("Usage: python http_test.py <http_root_directory> <Tsugaru_Command> <args> ....")
        return 1

    # Grab the folder path passed by CMake/CTest
    http_root = os.path.abspath(sys.argv[1])
    if not os.path.isdir(http_root):
        print("Root directory does not exist.")
        return 1
    
    print(f"--> Hosting HTTP root at: {http_root}")

    # Change the current working directory of Python so SimpleHTTPRequestHandler
    # automatically handles serving files from your specific sandbox directory.
    os.chdir(http_root)

    # Spin up the server on localhost port 8080 (or port 80 if your OS allows it)
    server_address = ("127.0.0.1", 8080)
    handler = SimpleHTTPRequestHandler
    
    # Optional: Suppress default verbose logging cluttering your ctest output
    # handler.log_message = lambda self, format, *args: None

    httpd = HTTPServer(server_address, handler)
    
    # Serve in a background thread so your main script remains responsive
    server_thread = threading.Thread(target=httpd.serve_forever)
    server_thread.daemon = True
    server_thread.start()
    time.sleep(0.5) # Quick pause to let the socket bind
    
    print(f"--> HTTP Server running on http://127.0.0.1:8080")

    print(sys.argv[2:])

    proc=subprocess.Popen(sys.argv[2:])
    proc.communicate()
    returnCode=proc.returncode

    httpd.shutdown() # Safely stops serve_forever loop
    httpd.server_close() # Closes listening socket

    return returnCode


if __name__ == "__main__":
    sys.exit(main())