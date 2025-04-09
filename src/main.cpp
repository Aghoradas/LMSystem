// BookstoreGui-revised

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/display.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../includes/bookdata.h"


wxFont titleFont = wxFont(16, 
                        wxFONTFAMILY_MODERN,
                        wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_EXTRABOLD, 
                        true, 
                        "Courier");
wxFont regularFont = wxFont(12,
                        wxFONTFAMILY_MODERN,
                        wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL,
                        false,
                        "Garamond");
wxColor BGCOLOR(128,128,128);
wxColor FGCOLOR(25,25,25);

vbd_t books = loading_books();

class BookApp : public wxApp {
public:
    virtual bool OnInit() override;
    std::string credentials;
};

// Class declaration for the window where the user will be able
// to update the information about a selected book, as well as be able
// to delete that selected book. However, when delete is selected
// program will make sure the user wants to delete infor. This is to
// insure that the user hasn't done this by mistake.
class UpdateBook : public wxFrame {
public:
    UpdateBook(wxFrame* parent);
    int selection;
    wxTextCtrl* titleEnt;
    wxTextCtrl* authEnt;
    wxTextCtrl* genEnt;
    wxTextCtrl* quanEnt;
    wxTextCtrl* pubEnt;
    wxTextCtrl* datEnt;
    wxTextCtrl* langEnt;
    wxTextCtrl* retEnt;
    wxTextCtrl* isbnEnt;
private:
    DECLARE_EVENT_TABLE();
    void deleteEntry (wxCommandEvent& event);
    void updateEntry (wxCommandEvent& event);
};

const int UPDATE_BOOK = 300;
const int DELETE_BOOK = 301;

BEGIN_EVENT_TABLE(UpdateBook, wxFrame)
    EVT_BUTTON(UPDATE_BOOK, UpdateBook::updateEntry)
    EVT_BUTTON(DELETE_BOOK, UpdateBook::deleteEntry)
END_EVENT_TABLE()


// Class declarationn for the window that is going to be
// in charge of taking the user data from textCtrl entry fields
// and submitting them to BookData for classification and saving to
// bookfile.dat for storage.
class AddBook : public wxFrame {
public:

    AddBook(wxFrame* parent);
    wxStatusBar* statusBar = this->CreateStatusBar(2);
    wxTextCtrl* titleEnt;
    wxTextCtrl* authEnt;
    wxTextCtrl* genEnt;
    wxTextCtrl* quanEnt;
    wxTextCtrl* pubEnt;
    wxTextCtrl* datEnt;
    wxTextCtrl* langEnt;
    wxTextCtrl* retEnt;
    wxTextCtrl* isbnEnt;

private:
    DECLARE_EVENT_TABLE();  
    void submitEntry(wxCommandEvent& event);
};

// Event IDs for AddBook (starting int field at 200)
const int SUB_BOOK = 200;

// Event handlers for AddBook
BEGIN_EVENT_TABLE(AddBook, wxFrame)
    EVT_BUTTON(SUB_BOOK, AddBook::submitEntry)
END_EVENT_TABLE()


class MainFrame : public wxFrame {
public:
    MainFrame();
    void closeWindow(wxCommandEvent& evt);
    void viewInfo(wxCommandEvent& evt);
    wxListBox* bookCollection;

    // This will clear out the listbox and relist all newly added and edited books.
    void refreshList(const vbd_t& updatedVector){
        bookCollection->Clear();  // Clear the current listbox contents

        // Add each item from the vector to the listbox
        for (const auto& item : books){
            bookCollection->Append("     Title:  " + item.title + "     Author:  " + item.author + "     Price:  " + item.retail_price);
        }
    }
    // Add status bar here.
private:
    DECLARE_EVENT_TABLE();
    void addInfo(wxCommandEvent& evt){
        // This will call the frame function AddBook
        // as a new window for add book entries.
        AddBook* addFrame = new AddBook(this);
        addFrame->SetSize(550,415);
        addFrame->Show(true);  // Show the AddBook frame
    }
    void updateInfo(wxCommandEvent& evt){
        // This will allow user to update book information,
        // as well as delete selected books from inventory.
        UpdateBook* updateFrame = new UpdateBook(this);
        updateFrame->SetSize(630,525);
        updateFrame->Show(true);
    }    
};

// Event IDs for MainFrame (starting int field at 100)
const int WINCL = 100;
const int VIEWIN = 101;
const int ADDIN = 102;
const int UPDATEIN = 103;
const int LISTBOX = 104;
const int FILE_QUIT = wxID_EXIT;

// Event Handlers for MainFrame *NEEDS to be after the frame class declaration.
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(FILE_QUIT, MainFrame::closeWindow)
    EVT_BUTTON(VIEWIN, MainFrame::viewInfo)
    EVT_MENU(ADDIN, MainFrame::addInfo)
    EVT_BUTTON(UPDATEIN, MainFrame::updateInfo)
END_EVENT_TABLE()

IMPLEMENT_APP(BookApp)   // *This line is the magic formula.

bool BookApp::OnInit() {
    // This is a fork()ing to call a login function.
    // If it returns 0, the user has access to the BookstoreInv,
    // and if it returns 1, the program exits.
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        // Failed to fork.
        wxLogMessage("Failed process call");
        exit(1);
    }
    if (pid == 0) {
        // Child process: this executes login_window.
        wxLogMessage("Calling login...");
        // const char* loginApp = "./login_window";  // excvp requires const char*
        // char* const arg[] = {"./", nullptr};      // excvp also requires a const char* argument
        execl("./login_window", (char*)1);    // even if we have none (nullptr).
    } else {
        // Parent process: check the exit status  of child.
        waitpid(pid, &status, 0);
        // Check if the child exited normally
        if (WIFEXITED(status)){
            int loginCode = WEXITSTATUS(status);
            if (2 == loginCode) {
                // Quit program if exitCode is 1, incorrect login.
                wxLogMessage("Incorrect Login");
                exit(0);
            } else if (1 >= loginCode) {
                // If the login returns an exitCode of 0, open Bookstore.
                //wxMessageBox("Login Complete");
                switch (loginCode) {
                    case 1:
                        BookApp::credentials = "Admin";
                    case 2:
                        BookApp::credentials = "user";
                }
                //continue;
            } else {
                // In case the exitCode was some other, unexpected result
                wxLogMessage("Return status: Unexpected result...");
                exit(1);
                //continue;
            }
        } else if (WIFSIGNALED(status)) {
            wxLogMessage("Process signaled exit: %d", WTERMSIG(status));
        } else {
            wxLogMessage("Login process terminated abnormally...");
        }
    }
    MainFrame* frame = new MainFrame();
    frame->Show();    
    return true;
}


// MAINFRAME* Beginning of main window function, which displays list of available books with buttons
// for viewing a selection of the list, a button to open a another window to add to the list,
// a button to open an edit window for a selected book, and a button to delete a book item
// from the list as well. Maybe in the future a way to open different book lists, or list
// books specific to particular customers or guests.
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "LMSystem") {
    // Menu bar section
    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ADDIN,
                    wxT("&Add..."),
                    _T("Add a book entry"));
    fileMenu->Append(wxID_OPEN,
                    wxT("&Open..."), 
                    _T("Opens an inventory list"));
    fileMenu->AppendSeparator();
    fileMenu->Append(FILE_QUIT, 
                    wxT("E&xit\tAlt-X"), 
                    _T("Quits the program"));
    menuBar->Append(fileMenu, _T("&File"));
    SetMenuBar(menuBar);

    wxPanel* panel = new wxPanel(this);
    panel->SetBackgroundColour(wxColor(BGCOLOR));
    panel->SetForegroundColour(wxColor(FGCOLOR));

    wxBitmap welcomeTitle("png-files/BInv_title.png", wxBITMAP_TYPE_PNG);
    wxStaticBitmap* titleImage = new wxStaticBitmap(panel,
                                                    wxID_ANY|wxALIGN_CENTER_HORIZONTAL,
                                                    welcomeTitle);
    titleImage->SetForegroundColour(wxColor(25,25,25));
    titleImage->SetFont(titleFont);

    // Listbox for book inventory
    bookCollection = new wxListBox(panel, 
                                    LISTBOX, 
                                    wxDefaultPosition, 
                                    wxSize(650,275));
    bookCollection->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(MainFrame::viewInfo), NULL, this);

    refreshList(books);

    wxBitmapButton* viewButton = new wxBitmapButton(panel, 
                                        VIEWIN, 
                                        wxBitmap("png-files/view-button.png", 
                                        wxBITMAP_TYPE_PNG));
    viewButton->SetFont(regularFont);

    wxBitmapButton* addButton = new wxBitmapButton(panel, 
                                        wxID_ANY, 
                                        wxBitmap("png-files/add_button.png", 
                                        wxBITMAP_TYPE_PNG));
    addButton->SetFont(regularFont);
    addButton->Bind(wxEVT_BUTTON, &MainFrame::addInfo, this);

    wxBitmapButton* updateButton = new wxBitmapButton(panel, 
                                    wxID_ANY, 
                                    wxBitmap("png-files/update.png", 
                                    wxBITMAP_TYPE_PNG));
    updateButton->SetFont(regularFont);
    updateButton->Bind(wxEVT_BUTTON, &MainFrame::updateInfo, this);

    // Insert a Function or window to sell or checkout books


    // Sizer here for all components of MainFrame.
    wxGridBagSizer* mainBox = new wxGridBagSizer(1,10);
    mainBox->Add(titleImage, 
                wxGBPosition(0,0), 
                wxGBSpan(1,6),
                wxALIGN_CENTER);
    mainBox->Add(bookCollection,
                wxGBPosition(2,0),
                wxGBSpan(10,4),
                wxLEFT,10);
    mainBox->Add(viewButton, wxGBPosition(4, 4), wxGBSpan(1,1), wxEXPAND | wxALL, 3);
    mainBox->Add(addButton, wxGBPosition(5, 4), wxGBSpan(1,1), wxEXPAND | wxALL, 3);
    mainBox->Add(updateButton, wxGBPosition(6, 4), wxGBSpan(1,1), wxEXPAND | wxALL, 3);
    panel->SetSizer(mainBox);
    //mainBox->Fit(panel);
    wxStatusBar* statusBar = this->CreateStatusBar(2); 
    MainFrame::SetStatusText(" Ready ");
    this->SetSize(840, 600);
};


// Viewing info from selected book in list
void MainFrame::viewInfo(wxCommandEvent& event){
    // Get selected item from the list in main
    wxListBox* listBox = dynamic_cast<wxListBox*>(FindWindowById(LISTBOX));
    if(listBox){
        int selection = listBox->GetSelection();
        if(selection != wxNOT_FOUND){
            if(!books.empty() && selection >= 0 && selection < books.size()){
                BookData selBookInfo = books[selection];
                selBookInfo.display();
            }else{
                wxLogMessage("*Selection Not Found -2nd layer error*");
            }
        }else{
            wxLogMessage("*Selection Not Found -1st layer error*");
        }
    }
}

// Closing the program
void MainFrame::closeWindow(wxCommandEvent& evt){
    Close(true);
}


// ADDBOOK* Beginning of AddFrame window
// This window simply acquires information specifics associated with a new book
// submital, entry(wxTextCtrl) widgets arranged on screen and tied to one submit button.
// Each field should accept whatever is typed into the given field except for the ISBN field
// The ISBN field must not be NULL and it must be an integer(hopefully). If neither of these
// are met, then the new book submition is refused.
AddBook::AddBook(wxFrame* parent) : wxFrame(parent, wxID_ANY, "New Book Submition"){
    AddBook::SetStatusText("Add Book");
    wxGridBagSizer* addSize = new wxGridBagSizer(2,3);  // For now GridBagSizer should be good enough

    // Starting frame for addInfo.
    wxPanel* panel = new wxPanel(this, wxID_ANY);  // Panel container for the whole window
    panel->SetBackgroundColour(wxColor(BGCOLOR));
    panel->SetForegroundColour(wxColor(FGCOLOR));

    // Create a label for the title.
    wxStaticText* label = new wxStaticText(panel, wxID_ANY|wxCENTER,
                                        " Book Info ");    
    label->SetFont(titleFont);

    // This whole team of lines gets input from users for the new book.
    titleEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    authEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    genEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    quanEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    pubEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    datEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    langEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    retEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));
    isbnEnt = new wxTextCtrl(panel, wxID_ANY,
                "", wxDefaultPosition, wxSize(200,25));

    // These place directive text as examples.
    isbnEnt->SetHint("  isbn#...  ");
    titleEnt->SetHint("  book title...  ");
    authEnt->SetHint("  author...  ");
    genEnt->SetHint("  genre...  ");
    quanEnt->SetHint("  quantity...  ");
    pubEnt->SetHint("  publisher...  ");
    datEnt->SetHint("  date published...  ");
    langEnt->SetHint("  language...  ");
    retEnt->SetHint("  price...  ");
    
    addSize->Add(label, wxGBPosition(1,0), wxGBSpan(1,6), wxEXPAND|wxALIGN_CENTER);
    addSize->Add(titleEnt, wxGBPosition(2,1), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(authEnt, wxGBPosition(2,3), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(genEnt, wxGBPosition(3,1), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(quanEnt, wxGBPosition(3,3), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(pubEnt, wxGBPosition(4,1), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(datEnt, wxGBPosition(4,3), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(langEnt, wxGBPosition(5,1), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(retEnt, wxGBPosition(5,3), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);
    addSize->Add(isbnEnt, wxGBPosition(6,1), wxGBSpan(1,1), wxEXPAND|wxLEFT,1);

    wxButton* suButton = new wxButton(panel, SUB_BOOK,
                                    " Submit ");
    addSize->Add(suButton, wxGBPosition(8,3), wxGBSpan(1,1), wxEXPAND);
    
    panel->SetSizer(addSize);
    
    AddBook::SetStatusText(" Add book information.. ");
    addSize->Fit(panel);
    };


void AddBook::submitEntry(wxCommandEvent &event) {
    AddBook::SetStatusText("Saving...");
    wxString isbn = isbnEnt->GetValue();
    str_t isbn_e = isbn.ToStdString();
    wxString title = titleEnt->GetValue();
    str_t title_e = title.ToStdString();

    // There must be atleast an entry of ISBN# or stock# and a Title.
    if (isbn_e.empty() && title_e.empty()){
        wxMessageBox("ISBN and Title cannot be empty!", "Error", wxICON_ERROR);
        return;
    }

    // Check to see if ISBN entry is a number    
    long isbnNum;
    if (!isbn.ToLong(&isbnNum)) {
        wxMessageBox("ISBN must be a number.");
        return;
    }

    // After the minimal entries are checked then values for entries are taken

    wxString author = authEnt->GetValue();
    str_t author_e = author.ToStdString();
    wxString genre = genEnt->GetValue();     
    str_t genre_e = genre.ToStdString();
    wxString quantity = quanEnt->GetValue();
    str_t quantity_e = quantity.ToStdString();
    wxString publisher = pubEnt->GetValue();
    str_t publisher_e = publisher.ToStdString();
    wxString date = datEnt->GetValue();
    str_t date_e = date.ToStdString();
    wxString language = langEnt->GetValue();
    str_t language_e = language.ToStdString();
    wxString price = retEnt->GetValue();
    str_t price_e = price.ToStdString();
    BookData newBook(isbn_e, title_e, author_e, genre_e, quantity_e,
                    publisher_e, date_e, language_e, price_e);
    books.emplace_back(newBook);
    saving_book(books);
    AddBook::SetStatusText("Saved.");
    wxWindow* parentWin = this->GetParent();
    MainFrame* parent = dynamic_cast<MainFrame*>(parentWin);
    if (parent) {
        parent->refreshList(books);
    }
    Close(true);
}


// UPDATEBOOK* Beginning of UpdateFrame window
// This window will allow users to change/update existing 
// informationg, or add missing information not applied when a
// book was first added to list. Also, this window will give the user
// access to the delete option if there's a need to remove an entry from
// the list.
UpdateBook::UpdateBook(wxFrame* parent) : wxFrame(parent, wxID_ANY, "Update Book"){
    // Get selected item from list in main
    wxListBox* listBox = dynamic_cast<wxListBox*>(FindWindowById(LISTBOX));
    selection = listBox->GetSelection();

    if (selection != wxNOT_FOUND) {            
        BookData selBook = books[selection];    
        wxGridBagSizer* updateSizer = new wxGridBagSizer(1,2);
    
        // Starting frame for updateInfo.
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        panel->SetBackgroundColour(wxColor(BGCOLOR));
        panel->SetForegroundColour(wxColor(FGCOLOR));

        // LABELS AND TEXTBOXES FOR UPDATE
        wxStaticText* titleLabel = new wxStaticText(panel, wxID_ANY, "Title :  ");
        updateSizer->Add(titleLabel, wxGBPosition(2,3),wxGBSpan(1,1), wxEXPAND);    
        titleEnt = new wxTextCtrl(panel, wxID_ANY, selBook.title);
        titleEnt->SetHint(selBook.title);
        updateSizer->Add(titleEnt, wxGBPosition(2,13), wxGBSpan(1,13),wxEXPAND);

        wxStaticText* isbnLabel = new wxStaticText(panel, wxID_ANY, "Isbn :  ");
        updateSizer->Add(isbnLabel, wxGBPosition(3,3),wxGBSpan(1,1), wxEXPAND);
        isbnEnt = new wxTextCtrl(panel, wxID_ANY, selBook.isbn);
        isbnEnt->SetHint(selBook.isbn);
        updateSizer->Add(isbnEnt, wxGBPosition(3,13), wxGBSpan(1,13), wxEXPAND);
    
        wxStaticText* authorLabel = new wxStaticText(panel, wxID_ANY, "Author :  ");
        updateSizer->Add(authorLabel, wxGBPosition(4,3),wxGBSpan(1,1), wxEXPAND);    
        authEnt = new wxTextCtrl(panel, wxID_ANY, selBook.author);
        authEnt->SetHint(selBook.author);
        updateSizer->Add(authEnt, wxGBPosition(4,13), wxGBSpan(1,13),wxEXPAND);

        wxStaticText* genreLabel = new wxStaticText(panel, wxID_ANY, "Genre :  ");
        updateSizer->Add(genreLabel, wxGBPosition(5,3),wxGBSpan(1,1), wxEXPAND);    
        genEnt = new wxTextCtrl(panel, wxID_ANY, selBook.genre);
        genEnt->SetHint(selBook.genre);
        updateSizer->Add(genEnt, wxGBPosition(5,13), wxGBSpan(1,13),wxEXPAND);
    
        wxStaticText* quantityLabel = new wxStaticText(panel, wxID_ANY, "Copies :  ");
        updateSizer->Add(quantityLabel, wxGBPosition(6,3),wxGBSpan(1,1), wxEXPAND);
        quanEnt = new wxTextCtrl(panel, wxID_ANY, selBook.quantity);
        quanEnt->SetHint(selBook.quantity);
        updateSizer->Add(quanEnt, wxGBPosition(6,13), wxGBSpan(1,13),wxEXPAND);
    
        wxStaticText* publisherLabel = new wxStaticText(panel, wxID_ANY, "Publisher :  ");
        updateSizer->Add(publisherLabel, wxGBPosition(7,3),wxGBSpan(1,1), wxEXPAND);
        pubEnt = new wxTextCtrl(panel, wxID_ANY, selBook.publisher);
        pubEnt->SetHint(selBook.publisher);
        updateSizer->Add(pubEnt, wxGBPosition(7,13), wxGBSpan(1,13), wxEXPAND);

        wxStaticText* dateLabel = new wxStaticText(panel, wxID_ANY, "Date Published :  ");
        updateSizer->Add(dateLabel, wxGBPosition(8,3),wxGBSpan(1,1), wxEXPAND);
        datEnt = new wxTextCtrl(panel, wxID_ANY, selBook.published_date);
        datEnt->SetHint(selBook.published_date);
        updateSizer->Add(datEnt, wxGBPosition(8,13), wxGBSpan(1,13), wxEXPAND);

        wxStaticText* languageLabel = new wxStaticText(panel, wxID_ANY, "Language :  ");
        updateSizer->Add(languageLabel, wxGBPosition(9,3),wxGBSpan(1,1), wxEXPAND);
        langEnt = new wxTextCtrl(panel, wxID_ANY, selBook.language);
        langEnt->SetHint(selBook.language);
        updateSizer->Add(langEnt, wxGBPosition(9,13), wxGBSpan(1,13), wxEXPAND);

        wxStaticText* priceLabel = new wxStaticText(panel, wxID_ANY, "Price :  ");
        updateSizer->Add(priceLabel, wxGBPosition(10,3),wxGBSpan(1,1), wxEXPAND);
        retEnt = new wxTextCtrl(panel, wxID_ANY, selBook.retail_price);
        retEnt->SetHint(selBook.retail_price);
        updateSizer->Add(retEnt, wxGBPosition(10,13), wxGBSpan(1,13), wxEXPAND);

        // BUTTON LIST
        wxButton* updateButton = new wxButton(panel, UPDATE_BOOK,
                                    " Update ");
        updateSizer->Add(updateButton, wxGBPosition(12,15), wxGBSpan(1,2), wxEXPAND);

        wxButton* deleteButton = new wxButton(panel, DELETE_BOOK,
                                    " Delete ");
        updateSizer->Add(deleteButton, wxGBPosition(12, 19), wxGBSpan(1,2), wxEXPAND);

        panel->SetSizer(updateSizer);
        updateSizer->Fit(panel);
    } else {
        this->Destroy();
        wxLogError("View Info needs a selection");
    }

}


void UpdateBook::updateEntry(wxCommandEvent &event) {
    wxString isbn = isbnEnt->GetValue();
    str_t isbn_e = isbn.ToStdString();
    wxString title = titleEnt->GetValue();
    str_t title_e = title.ToStdString();

    // There must be atleast an entry of ISBN# or stock# and a Title.
    if (isbn.empty() && title_e.empty()){
        wxMessageBox("ISBN and Title cannot be empty!");
        return;
    }

    // Check to see if ISBN entry is a number    
    long isbnNum;
    if (!isbn.ToLong(&isbnNum)) {
        wxMessageBox("ISBN must be a number.");
        return;
    }

    // After the minimal entries are checked then values for entries are taken
    wxString author = authEnt->GetValue();
    str_t author_e = author.ToStdString();    
    wxString genre = genEnt->GetValue();     
    str_t genre_e = genre.ToStdString();
    wxString quantity = quanEnt->GetValue();
    str_t quantity_e = quantity.ToStdString();
    wxString publisher = pubEnt->GetValue();
    str_t publisher_e = publisher.ToStdString();
    wxString date = datEnt->GetValue();
    str_t date_e = date.ToStdString();
    wxString language = langEnt->GetValue();
    str_t language_e = language.ToStdString();
    wxString price = retEnt->GetValue();
    str_t price_e = price.ToStdString();

  BookData newBook(isbn_e, title_e, author_e, genre_e, quantity_e,
                    publisher_e, date_e, language_e, price_e);
    auto position = books.begin() + selection; 
    books.erase(position);
    books.insert(position, newBook);
    saving_book(books);
    wxWindow* parentWin = this->GetParent();
    MainFrame* parent = dynamic_cast<MainFrame*>(parentWin);
    if (parent) {
        parent->refreshList(books);
    }
    Close(true);
}

void UpdateBook::deleteEntry(wxCommandEvent &event) {
    auto position = books.begin() + selection;
    books.erase(position);
    saving_book(books);
    wxWindow* parentWin = this->GetParent();
    MainFrame* parent = dynamic_cast<MainFrame*>(parentWin);
    if (parent) {
        parent->refreshList(books);
    }
    Close(true);
}
