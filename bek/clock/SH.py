#!/usr/bin/python3
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from threading import Thread
from urllib.parse import urlparse

# os.path
# urlib.parse
# threading
# signal
# pyprctl
# subprocess

# Read port number from config file
def get_port():
    return 8080

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
        self.wfile.write(bytes("<p>Please put your input to url parameter.</p>", "utf-8"))
        self.wfile.write(bytes("<p>Returned string: <p>", "utf-8"))

        # Send time in requested format
        self.wfile.write(bytes(str(datetime.now().strftime(urlparse(self.path).query)), "utf-8"))

    def log_message(self, format: str, *args) -> None:
        pass

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
    format_string = '%d.%m.%Y %H:%M:%S'
    user_input = input("Set time in format: " + format_string + " (e.g. 01.01.2000 00:00:00) or 'return' to return\n>")
    print(user_input)
    if(user_input == 'return'):
        return
    try:
        dt = datetime.strptime(user_input, str(format_string))
        timestamp = int(dt.timestamp())
    except:
        print("Could not parse time")
        return
    print(timestamp)
    ###### Start Admin Process


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
            # EOF found
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
            print(str(datetime.now().strftime(user_input)))


# Global Main function
def main():
    # Set server environment
    host_name = "localhost"
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
    local_machine()

    # Close server
    print("--------------")
    print("Ending server...")
    web_server.server_close()
    print("Server stopped")
    print("See you")

# Run the main function
if __name__ == '__main__':
    main()