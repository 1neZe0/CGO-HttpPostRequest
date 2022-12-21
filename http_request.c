#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Struct to represent an HTTP request
typedef struct {
  int fd;
  const char *method;
  const char *url;
  const char *contentType;
  int contentLength;
  const char *requestBody;
} HttpRequest;

// Function to create a new HTTP request
HttpRequest* NewRequest(const char *method, const char *url, const char *requestBody) {
  HttpRequest *request = malloc(sizeof(HttpRequest));
  request->method = method;
  request->url = url;
  request->requestBody = requestBody;
  request->contentType = "application/json";
  request->contentLength = strlen(requestBody);
  return request;
}

// Function to set the request URL
void setUrl(HttpRequest *request, const char *url) {
  request->url = url;
}

// Function to set the request body
void setRequestBody(HttpRequest *request, const char *requestBody) {
  request->requestBody = requestBody;
  request->contentLength = strlen(requestBody);
}

// Function to set the content type
void setContentType(HttpRequest *request, const char *contentType) {
  request->contentType = contentType;
}

// Function to set the content length
void setContentLength(HttpRequest *request, int contentLength) {
  request->contentLength = contentLength;
}
// Function to send the request and get the response
int postRequest(HttpRequest *request) {
  // Create the socket
  request->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (request->fd < 0) {
    perror("Error creating socket");
    return -1;
  }

  // Set the socket timeout
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if (setsockopt(request->fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
    perror("Error setting socket timeout");
    return -1;
  }

  // Connect to the server
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(80);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
  if (connect(request->fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("Error connecting to server");
    return -1;
  }


  // Create the HTTP request
  char buffer[BUFFER_SIZE];
  snprintf(buffer, sizeof(buffer), "%s %s HTTP/1.1\r\n", request->method, request->url);
  write(request->fd, buffer, strlen(buffer));
  snprintf(buffer, sizeof(buffer), "Content-Type: %s\r\n", request->contentType);
  write(request->fd, buffer, strlen(buffer));
  snprintf(buffer, sizeof(buffer), "Content-Length: %d\r\n", request->contentLength);
  write(request->fd, buffer, strlen(buffer));
  write(request->fd, "\r\n", 2);
  write(request->fd, request->requestBody, request->contentLength);

  // Read the response
  int n;
  char response[BUFFER_SIZE];
  while ((n = read(request->fd, response, sizeof(response))) > 0) {
    // Do something with the response
  }

  if (n < 0) {
    perror("Error reading response");
    return -1;
  }

  return 0;
}

// Function to get the response body
char* getResponseBody(HttpRequest *request) {
  // Read the response
  int n;
  char response[BUFFER_SIZE];
  char *responseBody = malloc(BUFFER_SIZE);
  memset(responseBody, 0, BUFFER_SIZE);
  while ((n = read(request->fd, response, sizeof(response))) > 0) {
    int newSize = strlen(responseBody) + n + 1;
    responseBody = realloc(responseBody, newSize);
    strncat(responseBody, response, newSize - strlen(responseBody) - 1);
  }

  return responseBody;
}

void stopHttpRequest(HttpRequest *request) {
    close(request->fd);
    free(request);
}
// postRequest, getResponseBody and stopHttpRequest
