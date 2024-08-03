from http.server import BaseHTTPRequestHandler, HTTPServer

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Print request details to console
        print(f"Received GET request for: {self.path}")
        print(f"Headers: {self.headers}")
        
        # Send response
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"Hello, world!")
    
    def do_POST(self):
        # Print request details to console
        content_length = int(self.headers['Content-Length']) if 'Content-Length' in self.headers else 0
        post_data = self.rfile.read(content_length)
        print(f"Received POST request for: {self.path}")
        print(f"Headers: {self.headers}")
        print(f"Body: {post_data.decode('utf-8')}")
        
        # Send response
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"POST request received!")
    
    def log_message(self, format, *args):
        return


def run(server_class=HTTPServer, handler_class=SimpleHTTPRequestHandler, port=5000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting httpd server on port {port}...')
    httpd.serve_forever()

if __name__ == "__main__":
    run()