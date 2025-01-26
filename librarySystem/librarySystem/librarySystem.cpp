#include <iostream>
#include <string>
#include <vector>
#include "stdafx.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <algorithm> 
#include <cctype>    

using namespace std;

// This is a logger class
// It is helpful for organising and managing logging across the application, providing a consistant logging format and simplifiying error tracking and debugging.
class Logger {
public:
	// Overloading
	// Each method in the class 'Logger' has the same name but takes in different parameters. This is called overloading. 
	// It enables the same function to handle different datatypes or arguments.
	void logMessage(const string& message) {
		cout << "[LOG MESSAGE] " << message << endl;
	}

	void logMessage(string& message) {
		cout << "[LOG MESSAGE] " << message << endl;
	}

	void logMessage(int message) {
		cout << "[LOG MESSAGE] " << message << endl;
	}
};

// Blueprint class for creating book objects.
class Book {

protected:
	mutable string title;	// Mutable so it can be changed by the Librarian class
	mutable string author;  // Mutable so it can be changed by the Librarian class
	static int totalBooks;

public:
	// Constructor 
	// Used to initialise objects of the book class. It initialises the 'title' and 'author' attributes, then increments the 'totalBooks' counter so the library has the correct number of total books.
	Book(const string& title, const string& author) : title(title), author(author) { 
		++totalBooks; // Increase the static book count by 1
	}

	// Destructor 
	// Used to decrement the 'totalBooks' counter and ensure proper exeuction of derived class destructors during cleanup
	// The reason this destructor is declared as virtual is to maintain polymorphic behaviour during the object destruction. 
	virtual ~Book() { 
		--totalBooks; // Decrease the static book count by 1
	}

	// Return book title
	virtual string getTitle() const {
		return title;
	}

	// Return book author
	virtual string getAuthor() const {
		return author;
	}

	// Display function using polymorphism
	virtual void display() const {
		cout << "Title: " << title << ", Author: " << author << endl;
	}

	// Static member function for returning 'totalBooks'
	static int getTotalBooks() {
		return totalBooks;
	}

	friend class Librarian; // Allows the 'Librarian' class to access protected attributes in 'Book' class.

};

int Book::totalBooks = 0; // Initialize static member


// Exception Handling using Exception Classes (Inheriting from Exception) 
// The class LibraryException inherits from the class exception.
// Explicit is used prevent implicit conversions and ensure the constructor is only called when explicitly provded with the correct argument type.
// The constructor takes a 'const char*' parameter which is used to pass a string message which describes the error or exception. 
// 'exception(message)' is calling the constructor of the base class 'exception' and passing the message to it. This ensures the message passed to the LibraryException constructor is then forwarded to the base class which initialises the base class's error message.
// The '{}' in the constructor indicates no additional actions are preformed within the constructor as all initialisation is handled by the initialiser list which is the 'exception' class.
class LibraryException : public exception {
public:
	explicit LibraryException(const char* message) : exception(message) {} // Constructor
};

// Derived class inheriting from the 'Book' class.
class PhysicalBook : public Book {
private:
	int shelfNum;

public:
	// Constructor
	// Used to initialise objects of the PhysicalBook class. 
	// This involves calling the base class 'Book' constructor to initialise the base class members 'title' and 'author'. It then initialises the 'shelfNum' member specific to 'PhysicalBook'
	// 'shelfNum' is passed in by value instead of by reference because it is a small data type and copying it is easier.
	PhysicalBook(const string& title, const string& author, int shelfNum) : Book(title, author), shelfNum(shelfNum) {}

	// Override 
	// This display function overrides the the virtual display function of the base class 'Book'
	void display() const override {
		Book::display();
		cout << "Shelf Number: " << shelfNum << endl;
	}

};

// Derived class inheriting from the 'Book' class.
class OnlineBook : public Book {
private:
	string url;

public:
	// Constructor
	// Used to initialise objects of the OnlineBook class. 
	// This involves calling the base class 'Book' constructor to initialise the base class members 'title' and 'author'. It then initialises the 'url' member specific to 'OnlineBook'
	OnlineBook(const string& title, const string& author, const string& url) : Book(title, author), url(url) {}

	// Override 
	// This display function overrides the the virtual display function of the base class 'Book'
	void display() const override {
		Book::display();
		cout << "Url: " << url << endl;
	}

};

// Friendship
// Admin class 
class Librarian {
public:
	// These methods are for changing the title and author of the book passed in as a parameter to the new title/author also passed in as a parameter.
	void modifiyBookTitle(const Book& book, const string& newTitle) {
		book.title = newTitle; // Accessing private member (friendship)
	}

	void modifiyBookAuthor(const Book& book, const string& newAuthor) {
		book.author = newAuthor; // Accessing private member (friendship)
	}

};

class Library {
private:
	// This creates a vector that stores pointers to 'Book' objects. 
	// Pointers are used to avoid copying the entire 'Book' object which is more effecient for larger objects.
	// This enables effecient memory usage by using pointers.
	vector<Book*> books;

public:
	// Destructor
	// Cleans up memory by iterating over each pointer and deallocating the memory.
	~Library() {
		for (Book* book : books) {
			delete book;
		}
	}

	// Adds the book pointer (which points to the book object) to the end of the books vector.
	void addBook(Book* book) {
		books.push_back(book);
	}

	// Loops through each book pointer and calls the display function which the book points to.
	void showAllBooks() const {
		for (const Book* book : books) {
			book->display();
		}
	}

	// Loops through each book pointer but only displays books from the class specified.
	void showBookByType(const string& type) const {
		for (const Book* book : books) {
			if (type == "PhysicalBook" && dynamic_cast<const PhysicalBook*>(book)) {
				book->display();
			}
			else if (type == "OnlineBook" && dynamic_cast<const OnlineBook*>(book)) {
				book->display();
			}
		}
	}

	// Loops through each book pointer and checks if the book title is the same as the title passed in as a parameter (Book requested by user).
	// If the book is found, it will increment the counter, stopping the for loop, and then displaying that book.
	bool showBookByTitle(const string& title) const {
		int counter = 0;
		for (const Book* book : books) {
			if (book->getTitle() == title) {
				counter++;
				book->display();
				break;
			}
		}
		if (counter >= 1) {
			return true;
		}
		else {
			return false;
		}
	}

	// Loops through each book pointer and checks if the book author is the same as the author passed in as a parameter (Book requested by user).
	// If the book is found, it will increment the counter, stopping the for loop, and then displaying that book.
	bool showBookByAuthor(const string& author) const {
		int counter = 0;
		for (const Book* book : books) {
			if (book->getAuthor() == author) {
				counter++;
				book->display();
				break;
			}
		}
		if (counter >= 1) {
			return true;
		}
		else {
			return false;
		}
	}

	// Loops through each book pointer and checks if the book title is the same as title passed in as a parameter (Book requested by user).
	// It then displays that book and returns it.
	const Book* getBookByTitle(const string& title) const {
		for (const Book* book : books) {
			if (book->getTitle() == title) {
				book->display();
				return book;
			}
		}
		return nullptr;
	}

	// Loops through each book pointer and checks if the book author is the same as author passed in as a parameter (Book requested by user).
	// It then displays that book and returns it.
	const Book* getBookByAuthor(const string& author) const {
		for (const Book* book : books) {
			if (book->getAuthor() == author) {
				book->display();
				return book;
			}
		}
		return nullptr;
	}

	// This method searches for the book pointer that is passed in, if it is found it is removed from the book pointer vector, otherwise the book was not in the library and an appropriate message is outputted.
	void deleteBook(const Book* book) {
		// Searches through the range of books from beginning to end. If the book is found, it returns the iterator pointing to that book, otherwise it returns 'books.end()'.
		auto it = find(books.begin(), books.end(), book);
		if (it != books.end()) {
			// The book pointer is a constant so it cannot be deleted, therefore the const qualifier needs to be removed.
			delete const_cast<Book*>(*it); // Deallocate memory
			books.erase(it); // Remove pointer from vector
			cout << "\nBook deleted...\n";
		}
		else {
			cout << "\nBook not in library...";
		}
	}

	// This method displays the last book in the vector which would be the most recent book added.
	void showNewestBook() const {
		if (!books.empty()) { // check if the books vector is empty
			books.back()->display(); // displays the last element in the vector (newest book added)
		}
		else {
			cout << "Library is empty!";
		}
	}

};

class ClientSocket {
private:
	SOCKET clientSocket;
	sockaddr_in serverAddress;

public:
	// Constructor that initializes the socket and the server address
	ClientSocket(int port, const char* ipAddress) : clientSocket(INVALID_SOCKET) {
		// Initialize the sockaddr_in structure with default values
		memset(&serverAddress, 0, sizeof(serverAddress));

		serverAddress.sin_family = AF_INET; // Sets the address family to IPv4 structure
		if (InetPtonA(AF_INET, ipAddress, &serverAddress.sin_addr.s_addr) != 1) { // Checks if the IP conversion from text to binary format failed.
			cout << "\nInvalid IP address provided..." << endl;
			WSACleanup(); // Releases memory.
			exit(1); // Exit
		}
		serverAddress.sin_port = htons(port); // Set port
	}

	// Destructor
	~ClientSocket() {
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
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Creates a TCP socket using IPv4.
		if (clientSocket == INVALID_SOCKET) { // Checks if socket is invalid.
			cout << "Error at socket(): " << WSAGetLastError() << endl; // Returns latest error.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "socket() is OK!" << endl;
		return true;
	}

	// Connects to a server using the client socket.
	bool connectToServer() {

		if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) { // Attempts to connect to the specified server address and server port.
			cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << endl; // Returns latest error.
			WSACleanup(); // Releases memory.
			return false;
		}
		cout << "Client: connect() is OK." << endl;
		cout << "Client: Can start sending and receiving data..." << endl;
		return true;
		
	}

	// Send a message to the server.
	bool sendMessage(const string& message) {
		int byteCount = send(clientSocket, message.c_str(), message.length(), 0); // Sends message using the connected client socket.

		// If bytecount is the same as the socket error code, the message failed to send.
		if (byteCount == SOCKET_ERROR) {
			cout << "\nMessage failed to send: " << WSAGetLastError() << endl; // Returns latest error.
			return false;
		}
		cout << "\nMessage sent: " << message << endl;
		return true;
	}

	// Recieves message from the server.
	bool recieveMessage(string& result) {
		// Creates a character array of 512 bytes, with each value being set to 0.
		// This is to ensure the buffer is cleared before receiving data so it prevents any possible left over data from affecing the recieved message. from the server.
		char buffer[512] = { 0 }; 
		int byteCount = recv(clientSocket, buffer, sizeof(buffer), 0); // Receive message from server.

		// If bytecount > 0, then a message was recieved.
		if (byteCount > 0) {
			result = string(buffer, byteCount);
			return true;
		}
		else {
			cout << "\nRecieving message failed: " << WSAGetLastError() << endl; // Returns latest error.
		}
		return false;
	}

	// Close the socket and deallocate memory.
	void cleanUp() {
		if (clientSocket != INVALID_SOCKET) {
			closesocket(clientSocket);
		}
		WSACleanup(); // Releases memory.
	}


};

// This method is used to show Functional pointers.
// It displays a simple starting message when the application is started.
void startMessage() {
	cout << "----------Library Management System----------" << endl; 
}

// This method is used to send messages to the winsock server. 
// It checks if the message is sent, if so it gets the response and displays it.
// 'sendMessage' and 'recieveMessage' handle errors.
void sendServerMessage(string message, ClientSocket& client) {
	if (client.sendMessage(message)) {

		string result;
		if (client.recieveMessage(result)) {
			cout << "Server response: " << result << endl;
		}
	}
}
// This function is for displaying the admin menu to add, remove and alter information about the books.
void displayAdminMenu(Library& library, Librarian& librarian, ClientSocket& client) { // pass by reference not value so it can be altered and not cause memory allocation that can't be accessed.
	char adminChoice, bookType;
	string title, author, url, userInput;
	int shelfNum;

	const Book* book = nullptr;

	do {
		cout << "\n---Admin Menu---\n";
		cout << "1: Add New Book\n";
		cout << "2: Delete Book\n";
		cout << "3: Modify Book Title\n";
		cout << "4: Modify Book Author\n";
		cout << "5: Return to Main Menu\n";
		cout << "Enter the number of your choice: ";
		cin >> adminChoice;
		cin.ignore();


		switch (adminChoice) {
		
			// Add New Book
		case '1':
			// First the user enters what type of book they want to add to the library.
			cout << "\nEnter book type, p for Physical or o for Online: ";
			cin >> bookType;
			cin.ignore();
			if (bookType == 'p' || bookType == 'P' || bookType == 'o' || bookType == 'O') {

				// Then the user is asked to enter the title and author of the book.
				cout << "\nEnter book title: ";
				getline(cin, title); // Using getline instead of 'cin >>' to include spaces for multiple words
				cout << "\nEnter book author: ";
				getline(cin, author);

				// Depending on what type of book the user wants to add to the library, they are required to enter either a shelf number or URL.
				if (bookType == 'p' || bookType == 'P') {
					cout << "\nEnter book shelf number: ";
					cin >> shelfNum;
					// The book is then added to the library.
					library.addBook(new PhysicalBook(title, author, shelfNum));
					// A message is sent to the server to simulate updating its database with the books title and author.
					sendServerMessage("New Physical Book added to library titled: " + title + ", author: " + author, client);
					// The latest book is then displayed (This will be the book just added as it is appended to the end of the library)
					library.showNewestBook();
				}
				else {
					cout << "\nEnter book url: ";
					cin >> url;
					// The book is then added to the library.
					library.addBook(new OnlineBook(title, author, url));
					// A message is sent to the server to simulate updating its database with the books title and author.
					sendServerMessage("New Online Book added to library titled: " + title + ", author: " + author, client);
					// The latest book is then displayed (This will be the book just added as it is appended to the end of the library)
					library.showNewestBook();
				}
			}
			else {
				cout << "\nInvalid Input...";
			}
			break;

			// Delete Book
		case '2':
			cout << "\nEnter book title: ";
			getline(cin, title);
			// Get book pointer from user input
			book = library.getBookByTitle(title);
			if (book) {
				// Set the book title and author as they are going to be deleted so they cannot be called when sending a message to the server to tell it what book is deleted.
				const string bookTitle = book->getTitle();
				const string bookAuthor = book->getAuthor();
				cout << "\n Book Found...";
				cout << "\n Please type confirm to delete book, otherwise any other input to cancel: ";
				cin >> userInput;
				if (userInput == "confirm" || userInput == "Confirm") {
					// Delete book if found
					library.deleteBook(book);
					// A message is sent to the server to simulate updating its database with the books title and author of the book deleted.
					sendServerMessage("Deleted book from library titled: " + bookTitle + ", author: " + bookAuthor, client);
				}
				else {
					cout << "\nBook deletion canceled..." << endl;
				}
			}
			else {
				
				cout << "\n No books with that title were found...\n";
			}

			break;

			// Modify Book Title
		case '3':
			cout << "\nEnter book title: ";
			getline(cin, title);
			// Get book pointer from user input
			book = library.getBookByTitle(title);
			if (book) {
				cout << "\n Book Found...";
				cout << "\n Enter new title: ";
				getline(cin, title);
				// Set the currentTitle of the book so it can be used to simulate updating the server database.
				const string currentTitle = book->getTitle();
				// The book title is then changed with this method.
				librarian.modifiyBookTitle(*book, title);
				cout << "\n Book updated to title: " << title << endl;
				// A message is sent to the server to simulate updating its database with the old book title and the new book title.
				sendServerMessage("Book title updated in library from: " + currentTitle + ", to: " + title, client);
			} 
			else {
				cout << "\nBook with title " << title << " not found" << endl;
			}
			break;

			// Modify Book Author
		case '4':
			cout << "\nEnter book author: ";
			getline(cin, author);
			// Get book pointer from user input
			book = library.getBookByAuthor(author);
			if (book) {
				cout << "\n Book Found...";
				cout << "\n Enter new author: ";
				getline(cin, author);
				// Set the currentAuthor of the book so it can be used to simulate updating the server database.
				const string currentAuthor = book->getAuthor();
				// The book author is then changed with this method.
				librarian.modifiyBookAuthor(*book, author);
				cout << "\n Book updated to author: " << author << endl;
				// A message is sent to the server to simulate updating its database with the old book author and the new book author.
				sendServerMessage("Book author updated in library from: " + currentAuthor + ", to: " + author, client);
			}
			else {
				cout << "\nBook with author " << author << " not found" << endl;
			}

			break;

			// Exit Admin Menu
		case 'q':
		case '5':
			cout << "\nExiting Admin Menu..." << endl;
			break;
		default:
			cout << "\nInvalid choice. Please try again.\n";

		}
	} while (adminChoice != 'q' && adminChoice != '5'); // Loops through the admin menu until the user quits using 'q' or '5' as an input.
}

// Main Program
int main(int argc, char* argv[]) {
	try {
		// Functional Pointer
		void (*startFunction)() = startMessage;
		startFunction();
		
		// Instantiate class object
		Library library;

		// Adding books manually on startup
		library.addBook(new PhysicalBook("The Silent Echo", "Emma Blackwood", 82));
		library.addBook(new PhysicalBook("Whispers in the Dark", "Liam Hunter", 150));
		library.addBook(new PhysicalBook("The Last Embrace", "Dylan Cooper", 199));
		library.addBook(new PhysicalBook("The Forgotten Path", "James Whitmore", 177));
		library.addBook(new PhysicalBook("Shadows of the Lost", "Grace Bennett", 43));
		library.addBook(new PhysicalBook("The Hidden Garden", "Ava Montgomery", 37));
		library.addBook(new PhysicalBook("Fragments of the Past", "Nora Stevens", 163));
		library.addBook(new PhysicalBook("The Burning Sky", "Oliver Gray", 185));
		library.addBook(new PhysicalBook("Dance of the Stars", "Harper Wilson", 135));
		library.addBook(new PhysicalBook("Shattered Glass", "Sebastian Cole", 180));
		library.addBook(new PhysicalBook("Between Worlds", "Jasper Ford", 211));
		library.addBook(new PhysicalBook("The Heart of the Storm", "Mason White", 129));
		library.addBook(new PhysicalBook("A Symphony of Souls", "Leo Knight", 53));

		library.addBook(new OnlineBook("The Shadow's Edge", "Clara Mills", "www.myOnlineBook/index/ApbD7fI2"));
		library.addBook(new OnlineBook("The Forgotten Kingdom", "Henry Wright", "www.myOnlineBook/index/W7kRm9Dg"));
		library.addBook(new OnlineBook("The Depths of Desire", "Lena Murphy", "www.myOnlineBook/index/X4nB2qJk"));
		library.addBook(new OnlineBook("The Silent Witness", "David Reed", "www.myOnlineBook/index/F3nGp0Ls"));
		library.addBook(new OnlineBook("Requiem for the Lost", "Natalie Stone", "www.myOnlineBook/index/P1cQxR8I"));
		library.addBook(new OnlineBook("Journey into the Unknown", "Isobel Price", "www.myOnlineBook/index/Uj4Cz1T9"));
		library.addBook(new OnlineBook("A World of Dreams", "Mason Hart", "www.myOnlineBook/index/L1zX9fT6"));
		library.addBook(new OnlineBook("The Edge of Tomorrow", "Ethan Matthews", "www.myOnlineBook/index/M0yJq2F7"));
		library.addBook(new OnlineBook("Whispers in the Wind", "Benjamin Miles", "www.myOnlineBook/index/Z0mV9p4J"));

		// Instantiate class object
		Librarian librarian;

		// LOGGING (Testing)
		Logger logger;
		logger.logMessage(Book::getTotalBooks()); // Log the total number of books currently in the library

		const char* serverIp = "127.0.0.1"; // Set IP (local)
		int port = 55555;					// Set port (local)

		// Instantiate class object
		ClientSocket client(port, serverIp); 

		// These methods start a client connection to the server.
		// This method finds the Winsock dll.
		if (!client.initaliseWinsock()) {
			logger.logMessage("\nFailure to initialise winsock client...");
			return 0;
		}
		// This method creates a client socket.
		if (!client.createSocket()) {
			logger.logMessage("\nFailure to create socket...");
			return 0;
		}

		// This method connects to the server.
		if (!client.connectToServer()) {
			logger.logMessage("\nFailure to connect to server");
			return 0;
		}

		char choice;
		string userInput;
		bool result;

		// This is the console applications main menu.
		do {
			cout << "\n---Main Menu---\n";
			cout << "1: View All Books\n";
			cout << "2: View Physical Books\n";
			cout << "3: View Online Books\n";
			cout << "4: Total Number of Books\n";
			cout << "5: Search for book by title\n";
			cout << "6: Search for book by author\n";
			cout << "7: Admin Menu\n";
			cout << "8: Quit\n";
			cout << "Enter the number of your choice: ";
			cin >> choice;

			// Clear the newline character left in the buffer
			cin.ignore();  // This will clear the '\n' character

			cout << "\n";

			switch (choice) {

				// View all books
			case '1':
				library.showAllBooks(); // Calls the 'showAllBooks()' method which displays all books.
				break;
				// View all Physical books
			case '2': 
				library.showBookByType("PhysicalBook"); // Displays all books with type 'PhysicalBook'
				break;
				// View all Online books
			case '3':
				library.showBookByType("OnlineBook"); // Displays all books with type 'OnlineBook'
				break;
				// Display total number of books
			case '4':
				cout << "\nTotal number of books: " << Book::getTotalBooks() << endl; // Displays total number of books using 'getTotalBooks()' method.
				break;
				// Search for a book by title
			case '5':
				cout << "\nEnter the title of the book: ";
				getline(cin, userInput);
				cout << "" << endl;
				// Displays book that the user inputted (by title), returning false if an error occured.
				result = library.showBookByTitle(userInput);
				// If no books are found with that title, appropriate error is displayed
				if (!result) {
					cout << "\n No books with that title were found...\n";
				}
				break;
				// Search for a book by author
			case '6':
				cout << "\nEnter the author of the book: ";
				getline(cin, userInput);
				cout << "" << endl;
				// Displays book that the user inputted (by author), returning false if an error occured.
				result = library.showBookByAuthor(userInput);
				// If no books are found with that title, appropriate error is displayed
				if (!result) {
					cout << "\n No books with that author were found...\n";
				}
				break;
				// Display the admin menu
			case '7':
				displayAdminMenu(library, librarian, client);
				break;
				// Exit application
			case 'q':
			case '8':
				cout << "Exiting Program" << endl;
				break;
			default:
				cout << "Invalid choice. Please try again.\n";
			}

		} while (choice != 'q' && choice != '8'); // Loops through the main menu until the user quits using 'q' or '8' as an input.

		client.cleanUp(); // Deallocating memory 

	}
	// Catch custom library exception
	catch (const LibraryException& e) {
		cerr << "Library Error: " << e.what() << endl;
	}
	// Catch normal exception
	catch (const exception& e) {
		cerr << "Unexpected Error: " << e.what() << endl;
	}

	return 0;

}