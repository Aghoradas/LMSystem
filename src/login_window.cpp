#include <cstdint>
#include <wx/wx.h>
#include <wx/display.h>
#include <string>
#include <vector>
#include <map>


class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class LoginFrame : public wxFrame {
public:
    LoginFrame();    
private:
    void OnButtonClick(wxCommandEvent& event); // Event handler for button click
    void EnterPressed(wxCommandEvent& event);  // Event after pressing enter.
    wxTextCtrl* login; // Text control for user input
    wxTextCtrl* password;
    wxButton* button;
};

std::map<std::string, std::vector<std::string>> login_records = {{"someone", {"goodness", "1"}},
                                                                    {"you", {"me", "2"}}};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    LoginFrame* frame = new LoginFrame();
    frame->SetWindowStyle(frame->GetWindowStyle());
    frame->Centre(wxBOTH);
    frame->Show(true);
    return true;
}

LoginFrame::LoginFrame() : wxFrame(NULL, wxID_ANY, " Login ") {
    SetWindowStyleFlag(wxSTAY_ON_TOP);
    Raise();
    wxPanel* panel = new wxPanel(this, wxID_ANY);

        
    // Create a label for directions.
    wxStaticText* label = new wxStaticText(panel, wxID_ANY, "LMS CREDENTIALS");
    wxFont font = wxFont(14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier");
    label->SetFont(font);

    // LOGIN
    login = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 100), wxTE_PROCESS_ENTER | wxTE_CENTER);
    login->SetMargins(5,-5);
    login->SetMinSize(wxSize(200, 30));
    login->SetHint(" login ");
    login->SetFont(font);
    login->Bind(wxEVT_TEXT_ENTER, &LoginFrame::EnterPressed, this);

    // PASSWORD
    password = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 100), wxTE_PROCESS_ENTER | wxTE_CENTER | wxTE_PASSWORD);
    password->SetMargins(5,-5);
    password->SetMinSize(wxSize(200, 30));
    password->SetHint(" password ");
    password->SetFont(font);
    password->Bind(wxEVT_TEXT_ENTER, &LoginFrame::EnterPressed, this);

    // Create a button
    wxButton* button = new wxButton(panel, wxID_ANY, "Send");
    button->SetFont(font);
    button->Bind(wxEVT_BUTTON, &LoginFrame::OnButtonClick, this);

    // Set the sizer for the frame
    wxBoxSizer* gbsizer = new wxBoxSizer(wxVERTICAL);

    // Add a stretch spacer before the label
    gbsizer->AddStretchSpacer(2);
    gbsizer->Add(label, 2, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
    gbsizer->AddStretchSpacer(1);
    gbsizer->Add(login, 2, wxEXPAND | wxALL, 5);
    gbsizer->Add(password, 2, wxEXPAND | wxALL, 5);
    gbsizer->AddStretchSpacer(1);
    gbsizer->Add(button, 0, wxALIGN_CENTER | wxALL, 2);

    panel->SetSizer(gbsizer);

    this->Fit();
    this->SetSize(360,300);

}

void LoginFrame::OnButtonClick(wxCommandEvent& event) {
    wxString login_test = login->GetValue(); // Get the text from the text control
    login->Clear();
    wxString password_test = password->GetValue();
    login->Clear();
    const char* user_id = login_test.char_str();
    if (login_records.count(std::string(login_test))) {
        if (login_records[std::string(login_test)].at(0) == password_test) {
            wxLogMessage("Login Confirmed..\nWelcome to LMSystem");
            int exit_code = stoi(login_records[std::string(login_test)].at(1));
            exit(1);
        }
    } else {
        wxLogMessage("Login failed");
        exit(2);
    }
}

void LoginFrame::EnterPressed(wxCommandEvent& event){
    OnButtonClick(event);
}
