/* This contains the class BookData, saving_book function, and loading_book function. */
#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>


typedef std::string str_t;
using std::cout;
str_t BOOKFILE = "bookfile.dat";


// Collects and stores all information about DataBook objects.
class BookData {
private:
	
public:
	// The variables here are all str_t(std::string), though the data being
	// sent to them began as wxString. If there are any type issues regarding
	// these or related variables, check the process behind their conversion from
	// wxString to str_t.
	str_t isbn;
	str_t title;
	str_t author;
	str_t genre;
	str_t quantity;
	str_t publisher;
	str_t published_date;
	str_t language;
	str_t retail_price;

	BookData(str_t num, str_t book, str_t auth, str_t gen, str_t quan,
		str_t publp, str_t p_date, str_t lang, str_t ret_price) {
		//BookData constructor is going to combine all info into an object.
		isbn = num;
		title = book;
		author = auth;
		genre = gen;
		quantity = quan;
		publisher = publp;
		published_date = p_date;
		language = lang;
		retail_price = ret_price;
	};
	// Could potentially format this into a C-style string for printf view messageBox.
	void display() {
		
		wxString message;
		wxString displayTitle;
		displayTitle.Printf("%s",title);
		message.Printf( "\n    Isbn/stock#:   %s"						
						"\n          Title:   %s"
						"\n         Author:   %s"
						"\n         Copies:   %s"
						"\n      Publisher:   %s"
						"\n Date Published:   %s"
						"\n       Language:   %s"
						"\n          Genre:   %s"
						"\n          Price:   $%s",
								isbn,
								title, author,
								quantity,
								publisher, published_date,
								language, genre,
								retail_price);
		wxMessageBox(message, displayTitle);
	}
};

typedef std::vector<BookData> vbd_t;   // Creates user-defined type called vbd_t.


// Saving... deconstructs vector and classes inorder to save information to file.
vbd_t saving_book(vbd_t &record) {
	// Creating and openning file
	std::ofstream books_file(BOOKFILE, std::ios::binary);
	cout << "\n   Saving";

	// Writing to file
	// This FOR loop iterates through the objects of the vector record(collection).
	for (unsigned int item = 0; item < (record).size(); ++item) {
		// Each quality of the object is listed and saved.
		books_file << "\n" << (record)[item].isbn << std::endl;   // New line before saving the first item,
		           											// because getline(loading) skips the first line of a file.
		books_file << (record)[item].title << std::endl;
		books_file << (record)[item].author << std::endl;
		books_file << (record)[item].genre << std::endl;
		books_file << (record)[item].quantity << std::endl;
		books_file << (record)[item].publisher << std::endl;
		books_file << (record)[item].published_date << std::endl;
		books_file << (record)[item].language << std::endl;
		books_file << (record)[item].retail_price << std::endl;
		cout << ".";
		
	}
	wxMessageBox("Saved");
	// Closing file
	books_file.close();
	cout << "  Saved\n\n" << std::endl;
	return (record);
}


// Loading saved data into a vector of class objects.
// Reconstructs class objects and the vector containing them.
static vbd_t loading_books() {
	str_t isbn;
	str_t title;
	str_t author;
	str_t genre;
	str_t quantity;
	str_t publisher;
	str_t published_date;
	str_t language;
	str_t retail_price;
	vbd_t temp_collection;

	std::ifstream books_file(BOOKFILE, std::ios::binary);
	if (books_file.is_open()) {
		while (getline(books_file, isbn)) {   // This loop will run for as long as there is a line to get (getline).

			// These next few lines will get each line of information contained about each book from temp_file(BOOKFILE).
			std::getline(books_file, isbn);
			std::getline(books_file, title);
			std::getline(books_file, author);
			std::getline(books_file, genre);
			std::getline(books_file, quantity);
			std::getline(books_file, publisher);
			std::getline(books_file, published_date);
			std::getline(books_file, language);
			std::getline(books_file, retail_price);

			BookData new_book(isbn, title, author, genre, quantity, publisher, published_date, language, retail_price);   // This reinstates the information as an object of Class BookData.
			temp_collection.push_back(new_book);  // This further reinstates that object as a list of vector temp_collection(collection)
		}
	}
	else {
		// In case the file isn't found error message.
		wxMessageBox("Unable to open file.");
		temp_collection = {};
		saving_book(temp_collection);
	}
	return temp_collection;
}
