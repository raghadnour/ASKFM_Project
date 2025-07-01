#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct User {
    int Id;
    string Name, Password, Email;
};

struct Question {
    int to, from, idq;
    string ques;
    bool anon;
};

struct Answer {
    int ID_user, ID_ques;
    string ans;
};

class Uploading {
protected:
    vector<User> Users;
    vector<Question> Questions;
    vector<Answer> Answers;
    string line;
    int lineCount;

public:
    void UploadUsers() {
        Users.clear();
        ifstream all_users("users.txt", ios::in);
        lineCount = 0;
        User u;

        while (getline(all_users, line)) {
            switch (lineCount % 4) {
            case 0: u.Email = line; break;
            case 1: u.Name = line; break;
            case 2: u.Password = line; break;
            case 3:
                u.Id = stoi(line);
                Users.push_back(u);
                break;
            }
            lineCount++;
        }
        all_users.close();
    }

    void UploadQuestions() {
        Questions.clear();
        ifstream questions("Question.txt", ios::in);
        lineCount = 0;
        Question q;

        while (getline(questions, line)) {
            switch (lineCount % 5) {
            case 0: q.to = stoi(line); break;
            case 1: q.anon = (stoi(line) == 1); break;
            case 2: q.ques = line; break;
            case 3: q.from = stoi(line); break;
            case 4:
                q.idq = stoi(line);
                Questions.push_back(q);
                break;
            }
            lineCount++;
        }
        questions.close();
    }

    void UploadAnswers() {
        Answers.clear();
        ifstream answers("answers.txt", ios::in);
        lineCount = 0;
        Answer a;

        while (getline(answers, line)) {
            switch (lineCount % 3) {
            case 0: a.ID_user = stoi(line); break;
            case 1: a.ID_ques = stoi(line); break;
            case 2:
                a.ans = line;
                Answers.push_back(a);
                break;
            }
            lineCount++;
        }
        answers.close();
    }
};

class Account : public Uploading {
protected:
    User currentUser;
    Question currentQuestion;
    Answer currentAnswer;
    bool found, isEmpty;

public:
    Account() : found(false), isEmpty(true) {}

    void SignUp() {
        UploadUsers();
        UploadQuestions();
        UploadAnswers();
        ofstream enterUser("users.txt", ios::app);

        cout << "\n=== Account Registration ===\n";
        cout << "Enter your email (format: name@domain.com): ";
        cin >> currentUser.Email;

        bool isValidEmail = false;
        while (!isValidEmail) {
            size_t atPos = currentUser.Email.find('@');
            if (atPos != string::npos && atPos > 0 && atPos < currentUser.Email.size() - 5) {
                string domain = currentUser.Email.substr(currentUser.Email.size() - 4);
                if (domain == ".com") {
                    isValidEmail = true;
                }
            }
            if (!isValidEmail) {
                cout << "[!] Invalid email. Please enter a valid email (name@domain.com): ";
                cin >> currentUser.Email;
            }
        }
        enterUser << currentUser.Email << '\n';

        cout << "Choose a username: ";
        cin >> currentUser.Name;

        bool isUniqueName = false;
        while (!isUniqueName) {
            isUniqueName = true;
            for (const auto& user : Users) {
                if (currentUser.Name == user.Name) {
                    isUniqueName = false;
                    cout << "[!] Username taken. Please choose another: ";
                    cin >> currentUser.Name;
                    break;
                }
            }
        }
        enterUser << currentUser.Name << "\n";

        cout << "Create a password: ";
        cin >> currentUser.Password;
        enterUser << currentUser.Password << "\n";

        cout << "Set your user ID (must be unique): ";
        cin >> currentUser.Id;

        bool isUniqueId = false;
        while (!isUniqueId) {
            isUniqueId = true;
            for (const auto& user : Users) {
                if (currentUser.Id == user.Id) {
                    isUniqueId = false;
                    cout << "[!] ID already exists. Please choose another: ";
                    cin >> currentUser.Id;
                    break;
                }
            }
        }
        enterUser << currentUser.Id << '\n';
        enterUser.close();
        Users.push_back(currentUser);
        cout << "[✓] Registration successful! Welcome, " << currentUser.Name << "!\n";
    }

    void Login() {
        UploadUsers();
        UploadQuestions();
        UploadAnswers();
        string correctPassword;

        cout << "\n=== Login ===\n";
        cout << "Username: ";
        cin >> currentUser.Name;

        found = false;
        while (!found) {
            for (const auto& user : Users) {
                if (currentUser.Name == user.Name) {
                    found = true;
                    correctPassword = user.Password;
                    currentUser.Id = user.Id;
                    break;
                }
            }
            if (!found) {
                cout << "[!] Username not found. Please try again: ";
                cin >> currentUser.Name;
            }
        }

        cout << "Password: ";
        cin >> currentUser.Password;

        while (currentUser.Password != correctPassword) {
            cout << "[!] Incorrect password. Try again: ";
            cin >> currentUser.Password;
        }
        cout << "[✓] Login successful! Welcome back.\n";
    }

    void ListUsers() {
        UploadUsers();
        cout << "\n=== Registered Users ===\n";
        for (const auto& user : Users) {
            cout << "-- " << user.Name << " (ID: " << user.Id << ")\n";
        }
    }

    bool ListQuestionsToMe() {
        UploadUsers();
        UploadQuestions();
        UploadAnswers();
        isEmpty = true;

        cout << "\n=== Questions For You ===\n";
        for (const auto& question : Questions) {
            if (question.to == currentUser.Id) {
                isEmpty = false;
                cout << "Question ID: " << question.idq << "\n";
                cout << "Content: " << question.ques << "\n";

                for (const auto& user : Users) {
                    if (user.Id == question.from) {
                        cout << "From: " << (question.anon ? "Anonymous" : user.Name) << "\n";
                        break;
                    }
                }

                bool isAnswered = false;
                for (const auto& answer : Answers) {
                    if (answer.ID_user == currentUser.Id && answer.ID_ques == question.idq) {
                        isAnswered = true;
                        cout << "Your Answer: " << answer.ans << "\n\n";
                        break;
                    }
                }
                if (!isAnswered) {
                    cout << "[!] Not answered yet\n\n";
                }
            }
        }

        if (isEmpty) {
            cout << "No questions found for you.\n";
            return false;
        }
        return true;
    }

    bool ListQuestionsFromMe() {
        UploadUsers();
        UploadQuestions();
        UploadAnswers();
        bool noQuestions = true;

        cout << "\n=== Your Questions ===\n";
        for (const auto& question : Questions) {
            if (question.from == currentUser.Id) {
                noQuestions = false;
                cout << "Question ID: " << question.idq << "\n";
                cout << "Content: " << question.ques << "\n";

                for (const auto& user : Users) {
                    if (question.to == user.Id) {
                        cout << "To: " << user.Name << "\n";
                        break;
                    }
                }

                bool isAnswered = false;
                for (const auto& answer : Answers) {
                    if (question.idq == answer.ID_ques && answer.ID_user == question.to) {
                        isAnswered = true;
                        cout << "Their Answer: " << answer.ans << "\n\n";
                        break;
                    }
                }
                if (!isAnswered) {
                    cout << "[!] Not answered yet\n\n";
                }
            }
        }

        if (noQuestions) {
            cout << "You haven't asked any questions.\n";
            return false;
        }
        return true;
    }

    void AskQuestion() {
        ofstream questionFile("Question.txt", ios::app);

        cout << "\n=== Ask a Question ===\n";
        cout << "Enter recipient's user ID: ";
        cin >> currentQuestion.to;

        bool isValidUser = false;
        while (!isValidUser) {
            for (const auto& user : Users) {
                if (currentQuestion.to == user.Id) {
                    isValidUser = true;
                    break;
                }
            }
            if (!isValidUser || currentQuestion.to == currentUser.Id) {
                isValidUser = false;
                cout << "[!] Invalid ID or cannot ask yourself. Try again: ";
                cin >> currentQuestion.to;
            }
        }
        questionFile << currentQuestion.to << '\n';

        bool validAnon = false;
        cout << "Ask anonymously? (1 = Yes, 0 = No): ";

        while (!validAnon) {
            if (!(cin >> currentQuestion.anon)) {  // Check if input failed
                cin.clear();  // Clear error state
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard bad input
                cout << "[!] Invalid input. Please enter 1 (Yes) or 0 (No): ";
            }
            else if (currentQuestion.anon == 0 || currentQuestion.anon == 1) {
                validAnon = true;
            }
            else {
                cout << "[!] Please enter only 1 (Yes) or 0 (No): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard any extra characters
            }
        }

        questionFile << currentQuestion.anon << '\n';

        cout << "Enter your question:\n> ";
        cin.ignore();
        getline(cin, currentQuestion.ques);
        questionFile << currentQuestion.ques << '\n';

        currentQuestion.from = currentUser.Id;
        questionFile << currentQuestion.from << '\n';

        currentQuestion.idq = Questions.empty() ? 1 : Questions.back().idq + 1;
        questionFile << currentQuestion.idq << "\n";

        Questions.push_back(currentQuestion);
        questionFile.close();
        cout << "[✓] Question sent successfully!\n";
    }

    void RespondToQuestion() {
        ofstream answerFile("answers.txt", ios::app);

        cout << "\n=== Answer a Question ===\n";
        cout << "Enter question ID to answer: ";
        cin >> currentAnswer.ID_ques;

        bool isValidQuestion = false;
        while (!isValidQuestion) {
            for (const auto& question : Questions) {
                if (question.to == currentUser.Id && question.idq == currentAnswer.ID_ques) {
                    isValidQuestion = true;
                    currentAnswer.ID_user = currentUser.Id;
                    break;
                }
            }
            if (!isValidQuestion) {
                cout << "[!] Invalid question ID. Try again: ";
                cin >> currentAnswer.ID_ques;
            }
        }

        bool isExistingAnswer = false;
        for (auto& answer : Answers) {
            if (currentAnswer.ID_ques == answer.ID_ques) {
                cout << "Existing answer will be updated.\n";
                cout << "Enter your new answer:\n> ";
                cin.ignore();
                getline(cin, answer.ans);
                isExistingAnswer = true;
                break;
            }
        }

        if (!isExistingAnswer) {
            cout << "Enter your answer:\n> ";
            cin.ignore();
            getline(cin, currentAnswer.ans);
            Answers.push_back(currentAnswer);
            answerFile << currentAnswer.ID_user << '\n'
                << currentAnswer.ID_ques << '\n'
                << currentAnswer.ans << '\n';
        }
        else {
            ofstream updateAnswer("answers.txt", ios::trunc);
            for (const auto& answer : Answers) {
                updateAnswer << answer.ID_user << "\n"
                    << answer.ID_ques << '\n'
                    << answer.ans << '\n';
            }
            updateAnswer.close();
        }
        cout << "[✓] Answer saved successfully!\n";
    }

    void Delete() {
        UploadAnswers();
        UploadQuestions();
        int questionId;

        cout << "\n=== Delete Question ===\n";
        cout << "Enter the ID of the question you want to delete: ";
        cin >> questionId;

        // Validate question exists and belongs to current user
        bool validQuestion = false;
        int recipientId = -1;

        for (const auto& question : Questions) {
            if (question.idq == questionId && question.from == currentUser.Id) {
                validQuestion = true;
                recipientId = question.to;
                break;
            }
        }

        while (!validQuestion) {
            cout << "[!] Invalid question ID or you don't own this question.\n";
            cout << "Please enter a valid question ID: ";
            cin >> questionId;

            for (const auto& question : Questions) {
                if (question.idq == questionId && question.from == currentUser.Id) {
                    validQuestion = true;
                    recipientId = question.to;
                    break;
                }
            }
        }

        // Delete question from vector
        for (auto it = Questions.begin(); it != Questions.end(); ) {
            if (it->idq == questionId) {
                it = Questions.erase(it);
                cout << "[✓] Question deleted successfully.\n";
                break;
            }
            else {
                ++it;
            }
        }

        // Delete associated answer if exists
        bool answerDeleted = false;
        for (auto it = Answers.begin(); it != Answers.end(); ) {
            if (it->ID_ques == questionId && it->ID_user == recipientId) {
                it = Answers.erase(it);
                answerDeleted = true;
            }
            else {
                ++it;
            }
        }

        // Update files
        ofstream questionFile("Question.txt", ios::trunc);
        for (const auto& q : Questions) {
            questionFile << q.to << '\n'
                << q.anon << '\n'
                << q.ques << '\n'
                << q.from << '\n'
                << q.idq << '\n';
        }
        questionFile.close();

        ofstream answerFile("answers.txt", ios::trunc);
        for (const auto& a : Answers) {
            answerFile << a.ID_user << '\n'
                << a.ID_ques << '\n'
                << a.ans << '\n';
        }
        answerFile.close();

        if (answerDeleted) {
            cout << "[✓] Associated answer was also removed.\n";
        }
    }
};

int main() {
    Account account;
    int choice;

    cout << "=== Question/Answer System ===\n";
    cout << "1. Register\n2. Login\n0. Exit\n";
    cout << "Choose an option: ";
    cin >> choice;

    if (choice == 1) {
        account.SignUp();
    }
    else if (choice == 2) {
        account.Login();
    }
    else {
        return 0;
    }

    while (true) {
        cout << "\n=== Main Menu ===\n";
        cout << "1. List Users\n";
        cout << "2. View Questions to You\n";
        cout << "3. View Your Questions\n";
        cout << "4. Ask a Question\n";
        cout << "5. Answer a Question\n";
        cout << "6. Delete a Question\n";
        cout << "7. Logout\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            system("cls");
            account.ListUsers();
            break;
        case 2:
            system("cls");
            account.ListQuestionsToMe();
            break;
        case 3:
            system("cls");
            account.ListQuestionsFromMe();
            break;
        case 4:
            system("cls");
            account.ListUsers();
            account.AskQuestion();
            break;
        case 5:
            system("cls");
            if (account.ListQuestionsToMe()) {
                account.RespondToQuestion();
            }
            break;
        case 6:
            system("cls");
            if (account.ListQuestionsFromMe()) {
                account.Delete();
            }
            break;
        case 7:
            system("cls");
            cout << "1. Register\n2. Login\n0. Exit\n";
            cin >> choice;
            if (choice == 1) {
                account.SignUp();
            }
            else if (choice == 2) {
                account.Login();
            }
            break;
        case 0:
            return 0;
        default:
            cout << "[!] Invalid choice. Try again.\n";
        }
    }
    return 0;
}