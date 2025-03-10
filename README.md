Project WebServ - 42 School
The WebServ project is an advanced exercise in network programming and HTTP protocol implementation at 42 School. Students build a lightweight, multi-threaded HTTP server from scratch in C++, capable of handling client requests, serving static/dynamic content, and parsing configuration files. This project teaches socket programming, concurrency, protocol standards, and server architecture, mirroring the functionality of real-world web servers like Nginx or Apache.
Core Objective

Create a customizable HTTP/1.1-compliant server that can process requests, serve files, and manage multiple client connections simultaneously. The server must adhere to RFC standards, parse configuration files, and handle errors gracefully while avoiding crashes or memory leaks.
Key Requirements

    Configuration File Parsing:

        Read a .conf file defining:

            Server blocks with IP/port bindings.

            Routes (locations) with root directories, index files, allowed methods, and error pages.

            Client limits (e.g., max body size).

            Server names (virtual hosts).

        Validate syntax (e.g., no duplicate ports, valid paths).

    HTTP Protocol Support:

        Handle core methods: GET, POST, DELETE.

        Manage headers (e.g., Content-Type, Content-Length, Host).

        Support chunked transfer encoding (bonus).

        Return appropriate status codes (e.g., 200 OK, 404 Not Found, 413 Payload Too Large).

    Static File Serving:

        Serve files (HTML, images, CSS) from specified directories.

        Auto-generate directory listings if no index file exists (bonus).

        Handle MIME types (e.g., text/html, image/jpeg).

    Dynamic Content (Bonus):

        Execute CGI scripts (e.g., Python, PHP) for routes configured with cgi_pass.

        Pass environment variables (e.g., PATH_INFO, QUERY_STRING) to CGI.

    Concurrency:

        Use multiplexing (select, poll, or epoll) or multi-threading to handle multiple clients.

        Prevent race conditions with proper synchronization.

    Error Handling:

        Custom error pages for status codes (e.g., 404, 500).

        Gracefully close connections on timeouts or invalid requests.

        Guard against buffer overflows, infinite loops, and memory leaks.

Technical Implementation

    Socket Programming:

        Create listening sockets with socket(), bind(), listen().

        Accept incoming connections with accept() and manage client sockets.

    Request Parsing:

        Parse HTTP request lines, headers, and bodies.

        Handle URL encoding/decoding and query strings.

        Enforce client body size limits.

    Response Generation:

        Construct HTTP responses with headers and payloads.

        Use sendfile() for efficient file transfers (bonus).

        Handle redirections (3xx status codes) and POST file uploads.

    Configuration Management:

        Store server and route settings in data structures (e.g., std::map).

        Resolve virtual hosts by matching Host headers to server blocks.

    Concurrency Model:

        Approach 1: Single-threaded event loop with epoll/kqueue for async I/O.

        Approach 2: Thread pool to parallelize request handling.

    Logging:

        Log requests, errors, and server status to stdout or files.

Bonus Extensions

Optional features to deepen complexity:

    CGI Support: Execute scripts (e.g., Python, PHP) and return dynamic content.

    Session/Cookies: Track user sessions with cookies (e.g., login systems).

    Reverse Proxy: Forward requests to other servers.

    HTTP/2 or WebSocket support.

    Load Balancing: Distribute traffic across multiple backend servers.

    SSL/TLS: Secure connections with HTTPS (using OpenSSL).

Learning Outcomes

    Network Programming: Mastery of sockets, TCP/IP, and I/O multiplexing.

    HTTP Protocol: Deep understanding of RFC 7230/7231 standards.

    Concurrency: Thread/process management and synchronization.

    Configuration Design: Parsing and applying server rules.

    Performance Optimization: Efficient resource handling for high traffic.

WebServ is a cornerstone project for aspiring backend developers and DevOps engineers, bridging low-level systems programming with modern web standards. It demands meticulous attention to protocol details, scalability, and security, preparing students for real-world challenges in building robust, high-performance servers.