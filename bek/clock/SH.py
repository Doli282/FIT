#!/usr/bin/python3
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from threading import Thread
from urllib.parse import urlparse
import os.path
import subprocess
import prctl

# ================ SERVER (START) ==============================
# Read port number from config file
def get_port() -> int:
    
    # Prepare path to the config file
    path = os.path.abspath(os.path.expanduser("~/.config/SH/SH.config"))
    
    # Try to open the configuration file
    try:
        file = open(path, "rt")
    except:
        print("Configuration file '" + path + "' cannot be open")
        raise AssertionError

    # Try to read the saved port number
    try:
        port = file.read()
        if(int(port) < 0 or int(port) > 65535):
            raise ValueError
    except:
        file.close()
        print("Invalid configuration - check " + path)
        raise AssertionError

    # By success return the port number
    file.close()
    return int(port)

# Class of HTTP server
class HTTP_handler(BaseHTTPRequestHandler):
    
    # Method accepting GET requests
    def do_GET(self):
        # Send code 200 response  
        self.send_response(200)  
  
        # Send header first  
        self.send_header('Content-type','text-html')  
        self.end_headers()

        # Send HTML page
        self.wfile.write(bytes("<h1>Network Clock</h1>", "utf-8"))
        self.wfile.write(bytes("<p>This application returns current time.</p>", "utf-8"))
        self.wfile.write(bytes("<p>Please put your input as url parameter.</p>", "utf-8"))
        self.wfile.write(bytes("<p>Returned string: <p>", "utf-8"))

        # Send time in requested format
        try:
            self.wfile.write(bytes(str(datetime.now().strftime(urlparse(self.path).query)), "utf-8"))
        except:
            self.wfile.write(bytes("<p> Some problem with conversion occured </p>", "utf-8"))
    
    # Do not write messages
    def log_message(self, format: str, *args) -> None:
        pass
# ================ SERVER (END) ================================

# ============== LOCAL USER (START) ============================
# Print help how to use the program
def print_help():
    print("---- Help ----")
    print("Use python's time format strings to display current time information, e.g. %H:%M:%S.")
    print("Write 'help' for repeating the help message,")
    print("Write 'quit' to exit the application.")
    print("Write 'set' to enter mode for setting time.")
    print("--------------")


# Set time - get paramters and call admin process
def set_time():
    # Get format new time from user
    format_string = "%d.%m.%Y %H:%M:%S"
    try:
        user_input = input("Set time in format: " + format_string + " (e.g. 01.01.2000 00:00:00) or 'return' to return\n>")
    except:
        print("Input interrupted")
        return
    
    # Give a chance to return
    if(user_input == 'return'):
        return
    
    # Convert time to unix timestamp
    try:
        dt = datetime.strptime(user_input, str(format_string))
        timestamp = int(dt.timestamp())
    except:
        print("Could not parse time")
        return

    # Start SUID process to change system time
    try:
        subprocess.run(["/usr/bin/NC", str(timestamp)], check=True)
        print("Time changed successfully")
    except:
        print("Error while setting time.")
        return


# Main function for local machine
def local_machine():

    # Print help for local user
    print_help()
    while True:
        user_input = ''
        
        # Get user input
        try:
            user_input = input("Enter string to get time:\n>")
        except:
            # EOF found / end of input (ctrl+D)
            print("Input ended")
            return

        # Set time
        if(user_input == 'set'):
            set_time()

        # Print help
        elif(user_input == 'help'):
            print_help()
        
        # Quit the program
        elif(user_input == 'quit'):
            print("Quitting...")
            return

        # Print current time in given format
        else:
            try:
                print(str(datetime.now().strftime(user_input)))
            except:
                print("Unable to convert string")
# ============== LOCAL USER (END) ==============================

# ============ CAPABILITIES (START) ============================
# Drop unnecessary capabilities, set needed capabilites
def set_capabilites():
    # Drop all capabilites except ... nothing
    prctl.cap_effective.limit()
    prctl.cap_inheritable.limit()
    prctl.cap_permitted.limit()
# ============== CAPABILITIES (END) ============================

# ================= MAIN (START) ===============================
# Global Main function
def main():

    # Set capabilites
    try:
        set_capabilites()
    except:
        print("Unable to set capabilities. Hopefully, they are set correctly...")
    
    local_ran = False # Flag if local machine was launched
    try:
        # Set server environment
        print("Reading configuration...")
        host_name = ""
        server_port = get_port()

        # Prepare server
        print("Preparing server")
        web_server = ThreadingHTTPServer((host_name, server_port), HTTP_handler)

        # Run server until kill signal comes
        print("Launching server")
        server_thread = Thread(target=web_server.serve_forever, daemon=True)
        server_thread.start()
        print("--------------")
        
        # Serve local requests
        local_ran = True
        local_machine()

        # Close server
        print("--------------")
        print("Ending server...")
        web_server.server_close()
        print("Server stopped")
    except:
        if(not(local_ran)):
            print("Problem with server - continuing only locally")
            local_machine()
    print("See you")

# Run the main function
if __name__ == '__main__':
    main()
    exit(0)
# ================== MAIN (END) ================================