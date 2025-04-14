---

```markdown
# Webserv

_Webserv_ is a School 42 project designed to build a lightweight HTTP server from scratch. This project challenges you to implement low-level programming skills, socket programming, and adherence to web protocols by developing a functioning web server capable of handling HTTP requests and serving static content. The focus is on robustness, efficiency, and compliance with the HTTP/1.1 specification.

---

## Table of Contents

- [Introduction](#introduction)
- [Project Description](#project-description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [Author](#author)
- [License](#license)

---

## Introduction

The _Webserv_ project involves creating a minimal yet fully functional web server using C/C++. You will work directly with sockets, file I/O, and the HTTP protocol to create a server that can communicate with web browsers and serve static files. This project is ideal for gaining hands-on experience with network programming and understanding the mechanics behind web communications.

---

## Project Description

The goal of _Webserv_ is to build an HTTP server that:
- Listens on one or more ports for incoming HTTP requests.
- Parses request headers and determines the appropriate response.
- Serves static files (HTML, CSS, JavaScript, images, etc.).
- Handles multiple HTTP methods such as GET, POST, and DELETE.
- Returns correct HTTP status codes (e.g., 200, 404, 500) based on the request outcome.
- Optionally supports CGI scripts for dynamic content generation.

Each component of the server is modularized to encapsulate different functionalities, such as socket management, request parsing, and response handling.

---

## Features

- **Minimalist HTTP Server:** Directly handles socket communications with a lean implementation.
- **HTTP/1.1 Compliance:** Supports the essential features of HTTP/1.1, including methods and status codes.
- **Static File Serving:** Capable of serving a variety of file types from a specified root directory.
- **Error Handling:** Provides standard HTTP error responses (e.g., 404 Not Found and 500 Internal Server Error).
- **Multi-Port Listening:** Can be configured to listen on multiple ports simultaneously.
- **Optional CGI Support:** Extendable to support dynamic content through CGI.

---

## Prerequisites

Before building and running the _Webserv_ project, ensure that you have the following installed:
- A C/C++ compiler (e.g., `gcc` or `clang`)
- [Make](https://www.gnu.org/software/make/) for build automation
- A Unix/Linux development environment
- Basic knowledge of socket programming and the HTTP protocol

---

## Installation

Follow these steps to build and run the project on your local machine:

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/Kinglo25/webserv.git
   cd webserv
   ```

2. **Build the Project:**

   Use the provided Makefile to clean and build the project. This will ensure that all previous build artifacts are removed:

   ```bash
   make fclean
   make
   ```

   *Note: If your build system supports any cache-related options, make sure to utilize them as needed.*

3. **Verify the Build:**

   Run the server binary to check that it starts correctly:

   ```bash
   ./webserv
   ```

   You should see a message confirming that the server is now listening on the configured port(s).

---

## Configuration

The server configuration is managed via a configuration file (e.g., `config.conf`) or command-line arguments. Typical settings include:

- **Port Number:** The port on which the server listens.
- **Root Directory:** The directory from which static files are served.
- **Error Pages:** Custom HTML pages to display for HTTP errors like 404 or 500.
- **CGI Settings:** (Optional) Parameters for executing CGI scripts.

Example configuration (`config.conf`):

```ini
# Webserv configuration file

[Server]
port = 8080
root = ./www

[ErrorPages]
404 = ./errors/404.html
500 = ./errors/500.html
```

Make sure to adjust the configuration to match your environment and project needs.

---

## Usage

After building and configuring the server, start it with the following command:

```bash
./webserv config.conf
```

- **Access the Server:** Open your web browser and navigate to `http://localhost:8080` (or the port specified in your configuration file).
- **View Logs:** Monitor server logs in your terminal to see HTTP request details and debug information.
- **Stop the Server:** Press `Ctrl + C` in the terminal where the server is running to stop it.

---

## Troubleshooting

- **Compilation Errors:** Ensure that all required development tools are installed and that your environment meets the prerequisites.
- **Server Not Listening:** Verify that the selected port is not being used by another application. Modify the configuration file if necessary.
- **404 Errors:** Confirm that the root directory specified in the configuration file exists and contains the requested files.
- **Permission Issues:** Make sure the server process has appropriate permissions to access the directories and files.

For further assistance, consult the project documentation or reach out to your peers and instructors at School 42.

---

## Contributing

Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch (e.g., `git checkout -b feature/my-new-feature`).
3. Make your changes, ensuring that you adhere to the coding standards.
4. Commit your changes with meaningful messages.
5. Push your branch and create a pull request.

Your contributions will help improve the project and enhance the learning experience for everyone.

---

## Author

- **Your Name**  
  [GitHub: Kinglo25](https://github.com/Kinglo25)

Developed as part of the School 42 curriculum.

---

## License

Distributed under the MIT License. See the `LICENSE` file for details.
```

---
