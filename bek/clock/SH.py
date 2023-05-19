#!/usr/bin/python3
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import threading

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

# Main function for server
def server():
    # Set server environment
    host_name = "localhost"
    server_port = get_port()

    # Prepare server
    web_server = ThreadingHTTPServer((host_name, server_port), HTTP_handler)
    
    # Run server until kill signal comes
    try:
        web_server.serve_forever()
    except KeyboardInterrupt:
        pass

    # Close server
    web_server.server_close()
    print(">Server stopped.")

# Print help how to use the program
def print_help(local_host: bool):
    print(">Please use python's time format strings to display current time information, e.g. (%H:%M:%S)")
    print(">For repeating the help message, write 'help'")
    print(">To exit the application, write 'quit'")
    if(local_host):
        print(">For setting the time, write 'set' and follow the procedure")


# Set time - get paramters and call admin process
def set_time():
    format_string = '%d.%m.%Y %H:%M:%S'
    user_input = input(">Set time in format: " + format_string + " (e.g. 01.01.2000 00:00:00) or 'return' to return\n")
    print(user_input)
    if(user_input == 'return'):
        return
    try:
        dt = datetime.strptime(user_input, str(format_string))
        timestamp = int(dt.timestamp())
    except:
        print(">>Could not parse time")
        return
    print(timestamp)
    ###### Start Admin Process


# Main function for local machine
def local_machine():
    while True:
        user_input = ''
        
        # Get user input
        try:
            user_input = input(">Enter string to get time:\n")
        except:
            # EOF found
            print(">Input ended")
            return

        # Set time
        if(user_input == 'set'):
            set_time()

        # Print help
        elif(user_input == 'help'):
            print_help(True)
        
        # Quit the program
        elif(user_input == 'quit'):
            print(">Quitting...")
            return

        # Print current time in given format
        else:
            print(str(datetime.now().strftime(user_input)))


def main():
    # Set server environment
    host_name = "localhost"
    server_port = get_port()

    # Prepare server
    web_server = ThreadingHTTPServer((host_name, server_port), HTTP_handler)
    
    # Run server until kill signal comes
    server_thread = threading.Thread(target=web_server.serve_forever())

    # Server local requests
    local_machine()

    # Close server
    print(">Ending server")
    web_server.server_close()
    print(">Server stopped.")

    print("See you")

# Main function
def main2():
    # Prepare server
    server_thread = threading.Thread(target=web)
    
    # Launch server
    print(">Starting server")
    server_thread.start()
    
    

    # Stop server
    #### send killing signal
    
    server_thread.join()

    

# Run the main function
if __name__ == '__main__':
    main()