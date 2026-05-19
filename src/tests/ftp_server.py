# pip install pyftpdlib
import subprocess
import os
import sys
import time
import threading
from ftplib import FTP



def main():
    if len(sys.argv) < 2:
        print("Usage: python ftp_test.py <test_downloading_directory> <TsugaruExe> <args> ....")
        return 1
        
    # Grab the folder path passed by CMake/CTest
    ftp_download = os.path.abspath(sys.argv[1])
    if not os.path.isdir(ftp_download):
        print("Root directory does not exist.")
        return 1

    print(sys.argv[2:])

    proc=subprocess.Popen(sys.argv[2:])

    time.sleep(10)

    print("Wake up");

    os.chdir(ftp_download)
    if os.path.isfile("WATTCP.CFG"):
        os.remove("WATTCP.CFG")

    if os.path.isfile("WATTCP.CFG"):
        print("Cannot delete a file for download test.\n")
        return 1

    ftp=FTP()
    forwarded_port = 2122
    localFilename="WATTCP.CFG"
    serverFilename="NET/WATTCP/WATTCP.CFG"

    print(f"--> Connecting to port-forward gateway 127.0.0.1:{forwarded_port}...")
    ftp.connect("127.0.0.1", forwarded_port, timeout=10)

    print("--> Logging in...")
    # Change these credentials to match whatever the FM Towns FTP server expects
    ftp.login(user="anonymous", passwd="tsugaru") 

    # Turn on passive mode. Highly recommended for NAT testing!
    ftp.set_pasv(False) 

    print("--> Downloading from the VM...")
    # 'RETR test.txt' tells the VM server to send the file.
    # We pass a lambda block to write the incoming byte chunks into our local file.
    with open(localFilename, "wb") as f:
        ftp.retrbinary("RETR "+serverFilename, lambda data: f.write(data))
        
    print("--> Download complete! Verifying contents...")

    ftp.quit()

    # Don't need the server at this point.
    proc.terminate()


    # Simple verification check
    if os.path.exists(localFilename) and os.path.getsize(localFilename) > 0:
        print("Test Passed: File successfully pulled from VM via port-forwarding!")
        return 0
    else:
        print("Test Failed: File was created but contains no data.")
        return 1    





if __name__ == "__main__":
    sys.exit(main())

