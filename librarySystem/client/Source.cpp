#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class ServerSocket {
private:
	SOCKET serverSocket;
	SOCKET acceptSocket;
	sockaddr_in service;

public:
	// Constructor for binding to a specific IP address
	ServerSocket(int port, const char* ipAddress) : serverSocket(INVALID_SOCKET), acceptSocket(INVALID_SOCKET) {
		// Initialise the sockaddr_in structure in the member initialisation list.

		service.sin_family = AF_INET; // Sets the address family to IPv4 structure
		if (InetPtonA(AF_INET, ipAddress, &service.sin_addr.s_addr) != 1) { // Checks if the IP conversion from text to binary format failed.
			cout << "Invalid IP address." << endl;
			WSACleanup(); // Releases memory.
			exit(1); // Exit
		} 
		service.sin_port = htons(port);       // Set the port number
	}

	// Destructor
	~ServerSocket() {
		cleanUp();
	}

	// Initalises the winsock dll
	bool initaliseWinsock() {
		WSADATA wsaData; // Holds configuration data for initialising the windows sockets communication.
		WORD wVersionRequested = MAKEWORD(2, 2); // Version 2.2 of winsock.
		int wsaerr = WSAStartup(wVersionRequested, &wsaData); // Initalises the winsock library for communication.
		if (wsaerr != 0) {
			cout << "The Winsock dll not found!" << endl;
			return false;
		}
		cout << "The Winsock dll found!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
		return true;
	}

	// Create network socket for communication.
	bool createSocket() {
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Creates a TCP socket using IPv4.
		if (serverSocket == INVALID_SOCKET) { // Checks if socket is invalid.
			cout << "Error at socket(): " << WSAGetLastError() << endl; // Returns latest error.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "socket() is OK!" << endl;
		return true;
	}

	// Binds a socket to the speficied address and port.
	bool bindSocket() {

		if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) { // Checks if the socket bind failed.
			cout << "bind() failed: " << WSAGetLastError() << endl; // Returns latest error.
			closesocket(serverSocket); // Closes the socket.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "bind() is OK!" << endl;
		return true;
	}

	// Listens for incoming messages.
	bool listener() {
		if (listen(serverSocket, 1) == SOCKET_ERROR) { // Checks if a socket failed to prepare for incomming connections.
			cout << "listen(): Error listening on socket " << WSAGetLastError() << endl; // Returns latest error.
			closesocket(serverSocket); // Closes the socket.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "listen() is OK, I'm waiting for connections..." << endl;
		return true;
	}

	// Accepts incoming connections on the server socket, creating a new socket for communications.
	bool acceptConnection() {
		sockaddr_in clientAddr; // Store client address.
		int clientAddrSize = sizeof(clientAddr); // Sets the size of the client address to specify the buffer size.

		acceptSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize); // Creates a new socket to communicate with the client when it accepts a connection request from a listening socket.

		if (acceptSocket == INVALID_SOCKET) { // Checks if the accepted socket is invalid.
			cout << "accept failed: " << WSAGetLastError() << endl; // Returns latest error.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "Accepted connection" << endl;
		return true;
	}

	// Recieves message from the client.
	bool recieveMessage(string& message) {
		// Creates a character array of 200 bytes, with each value being set to 0.
		// This is to ensure the buffer is cleared before receiving data so it prevents any possible left over data from affecing the recieved message.
		char buffer[200] = { 0 };
		int byteCount = recv(acceptSocket, buffer, sizeof(buffer), 0); // Receive message from client.

		// If bytecount > 0, then a message was recieved.
		if (byteCount > 0) {
			cout << "Message recieved: " << buffer << endl;
			message = string(buffer, byteCount);
			return true;
		}
		cout << "Failed to receive message. Error: " << WSAGetLastError() << endl; // Returns latest error.
		return false;
	}

	// Send a message to the client.
	bool sendMessage(const string& message) {
		int byteCount = send(acceptSocket, message.c_str(), message.length(), 0); // Sends message using the new accepted socket.
		// If bytecount is the same as the socket error code, the message failed to send.
		if (byteCount == SOCKET_ERROR) {
			cout << "\nSending failed: " << WSAGetLastError() << endl; // Returns latest error.
			return false;
		}
		cout << "Message sent: " << message << endl;
		return true;
	}

	// Method used to process recieved message.
	void handleMessage(const string& message) {
		string response;
		vector<string> keyWords = parseMessage(message); // Splits the message into individual words and stores them in a vector.

		if (keyWords.size() > 0) { // Checking if the message received is not empty.
			// In the admin menu, the user is able to add a new book, delete a book and modify both the book title and author.
			// These if statements catch the key words received from the client message to simulate the database being updated.
			// It then sets the response to a string to then be sent back to the client as an appropriate message to simulat the database being updated.
			if (keyWords[0] == "Deleted") {
				response = "Server deleted book from library...\n";
			}
			else if (keyWords[1] == "Online" || keyWords[1] == "Physical") {
				response = "Server added new " + keyWords[1] + " book to the library...\n";
			}
			else if (keyWords[2] == "updated" && keyWords[1] == "title") {
				response = "Server updated book title in the library...\n";
			}
			else if (keyWords[2] == "updated" && keyWords[1] == "author") {
				response = "Server updated book author in the library...\n";
			}
			else {
				response = "Invalid message...";
			}
		}

		sendMessage(response); // Send message back to client.
			
	}

	// Method to separate a message recieved into individual keywords and store them in a vector.
	vector<string> parseMessage(const string& message) {
		vector<string> words;
		stringstream ss(message);
		string word;

		// Split message by spaces
		while (ss >> word) {
			words.push_back(word);
		}

		return words;
	}

	// Close the socket and deallocate memory.
	void cleanUp() {
		if (acceptSocket != INVALID_SOCKET) {
			closesocket(acceptSocket);
		}
		if (serverSocket != INVALID_SOCKET) {
			closesocket(serverSocket);
		}
		WSACleanup(); // Releases memory.
	}
};

// Main Program
int main(int argc, char* argv[]) {

	const char* ipAddress = "127.0.0.1"; // Set IP (local)
	int port = 55555;					 // Set port (local)

	// Instantiate class object
	ServerSocket server(port, ipAddress);

	// These methods start a server connection, waiting for a client connection.
	// This method finds the Winsock dll.
	if (!server.initaliseWinsock())
		return 0;
	// This method creates a server socket.
	if (!server.createSocket())
		return 0;
	// This method binds the server socket to the specified address and port.
	if (!server.bindSocket())
		return 0;
	// This method sets up the socket to listen for incoming clients.
	if (!server.listener())
		return 0;
	// Wait for connection from the client.
	if (!server.acceptConnection())
		return 0;

	string message;

	// Constantly listen for incoming messages.
	while (true) {
		if (!server.recieveMessage(message)) {
			cout << "\nLost connection..." << endl;
			break;
		}

		server.handleMessage(message); // Handle incoming messages.

	}

	return 0;
}
