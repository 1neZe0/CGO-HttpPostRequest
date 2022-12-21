package main

// #include "http_request.c"
import "C"

import (
	"fmt"
	"unsafe"
)

// HttpRequest represents an HTTP request in C.
type HttpRequest struct {
	fd            C.int
	method        *C.char
	url           *C.char
	contentType   *C.char
	contentLength C.int
	requestBody   *C.char
}

// NewRequest creates a new HttpRequest in C.
func NewRequest(method, url, requestBody string) *HttpRequest {
	cmethod := C.CString(method)
	curl := C.CString(url)
	crequestBody := C.CString(requestBody)
	req := C.NewRequest(cmethod, curl, crequestBody)
	return (*HttpRequest)(unsafe.Pointer(req))
}

// SetUrl updates the URL of the HttpRequest in C.
func (req *HttpRequest) SetUrl(url string) {
	curl := C.CString(url)
	C.setUrl((*C.HttpRequest)(unsafe.Pointer(req)), curl)
}

// SetRequestBody updates the request body of the HttpRequest in C.
func (req *HttpRequest) SetRequestBody(requestBody string) {
	crequestBody := C.CString(requestBody)
	C.setRequestBody((*C.HttpRequest)(unsafe.Pointer(req)), crequestBody)
}

// SetContentType updates the content type of the HttpRequest in C.
func (req *HttpRequest) SetContentType(contentType string) {
	ccontentType := C.CString(contentType)
	C.setContentType((*C.HttpRequest)(unsafe.Pointer(req)), ccontentType)
}

// SetContentLength updates the content length of the HttpRequest in C.
func (req *HttpRequest) SetContentLength(contentLength int) {
	C.setContentLength((*C.HttpRequest)(unsafe.Pointer(req)), C.int(contentLength))
}

// PostRequest sends the request and gets the response in C.
func (req *HttpRequest) PostRequest() int {
	return int(C.postRequest((*C.HttpRequest)(unsafe.Pointer(req))))
}

// GetResponseBody gets the response body in C.
func (req *HttpRequest) GetResponseBody() string {
	cresponseBody := C.getResponseBody((*C.HttpRequest)(unsafe.Pointer(req)))
	return C.GoString(cresponseBody)
}

// StopHttpRequest closes the socket in C.
func (req *HttpRequest) StopHttpRequest() {
	C.stopHttpRequest((*C.HttpRequest)(unsafe.Pointer(req)))
}

func main() {
	// Create a new request
	request := NewRequest("POST", "https://api.telegram.org/botYOUR_BOT_TOKEN/sendMessage", `{"chat_id":YOUR_CHAT_ID,"text":"Hello, world!"}`)
	// Set the URL of the request
	request.SetUrl("https://api.telegram.org/bot5185078921:AAHiAV-xaDHa5UGBbQI91KjdlYxk5emlvso/sendMessage")

	// Set the request body
	request.SetRequestBody(`{"chat_id":434057604,"text":"Hello, world!"}`)

	// Set the content type
	request.SetContentType("application/json")

	//Set the content length
	//request.SetContentLength(14)

	// Send the request and get the response
	if err := request.PostRequest(); err != 0 {
		fmt.Println("Error sending request")
	}

	// Get the response body
	responseBody := request.GetResponseBody()
	fmt.Println(responseBody)

	// Close the socket
	request.StopHttpRequest()
}
