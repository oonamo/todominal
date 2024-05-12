#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// Function to get the home enviroment variable and create the database if it
// does not exist
const string initDataBase()
{
    char *userPath = getenv("HOME");
    string userFile;

    // string userFile = string(userPath);
    string mkdirCmd = "mkdir -p $HOME/.config/todominal";
    string touchCmd = "touch $HOME/.config/todominal/todos.txt";

    if (userPath == NULL)
    {
        userPath = getenv("USERPROFILE");
        userFile = string(userPath);
        replace(userFile.begin(), userFile.end(), '\\', '/');

        // HACK: mkdir only works with '\', but touch is fine as '/' ?
        mkdirCmd = "mkdir " + string(userPath) + "\\.config\\todominal\\";
        touchCmd = "touch " + userFile + "/.config/todominal/todos.txt";
    }
    else
        userFile = string(userPath);

    system(mkdirCmd.c_str());
    system(touchCmd.c_str());
    return userFile;
}

const string userHome = initDataBase();
const string mainFile = userHome + "/.config/todominal/todos.txt";

// Function to remove a line from a specific line from a file
//
void removeLine(const string &filename, int lineNumberToRemove)
{
    ifstream inputFile(filename);
    vector<string> lines;

    if (inputFile.is_open())
    {
        string line;
        int currentLine = 1;
        while (getline(inputFile, line))
        {
            if (currentLine != lineNumberToRemove)
            {
                lines.push_back(
                    line); // Keep the line if it's not the one to remove
            }
            currentLine++;
        }
        inputFile.close();

        // Write the modified content back to the file
        ofstream outputFile(filename);
        if (outputFile.is_open())
        {
            for (const string &line : lines)
            {
                outputFile << line << endl;
            }
            outputFile.close();
        }
        else
        {
            cerr << "Unable to open output file: " << filename << endl;
        }
    }
    else
    {
        cerr << "Unable to open input file: " << filename << endl;
    }
}

// Functiom to get a specific line of a file
string getLineContent(const string &filename, int lineNumber)
{
    ifstream file(filename);
    string lineContent;
    int currentLine = 1;

    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            if (currentLine == lineNumber)
            {
                lineContent = line;
                break;
            }
            currentLine++;
        }
        file.close();
    }
    else
    {
        cerr << "Unable to open file: " << filename << endl;
    }

    return lineContent;
}

// Function to Replace a line with a given line
void replaceLine(const string &filename, int lineNumber, const string &newLine)
{
    ifstream inputFile(filename);
    vector<string> lines;

    if (inputFile.is_open())
    {
        string line;
        int currentLine = 1;
        while (getline(inputFile, line))
        {
            if (currentLine == lineNumber)
            {
                lines.push_back(newLine); // Replace the line with newLine
            }
            else
            {
                lines.push_back(line); // Keep the original line
            }
            currentLine++;
        }
        inputFile.close();

        // Write the modified content back to the file
        ofstream outputFile(filename);
        if (outputFile.is_open())
        {
            for (const string &line : lines)
            {
                outputFile << line << endl;
            }
            outputFile.close();
        }
        else
        {
            cerr << "Unable to open output file\n";
        }
    }
    else
    {
        cerr << "Unable to open input file\n";
    }
}

// Function to StrikeThrough given text
string strikethrough(const string &text)
{
    string result;
    result = "\e[9m" + text + "\e[m";
    return result;
}

// Function to remove a todo
void removeTodo()
{
    string toRemove;
    cout << "Enter Todo Index to Remove: ";
    std::cin >> toRemove;
    int toRemoveint = stoi(toRemove);
    removeLine(mainFile, toRemoveint);
}

// Function to mark a todo as done
void markDone()
{
    string markInput;
    cout << "Enter Todo Index to Mark as Done: ";
    std::cin >> markInput;
    int markNum = stoi(markInput);
    replaceLine(mainFile, markNum,
                strikethrough(getLineContent(mainFile, markNum)));
}

// Function to print indexed contents of a file
void printTodo()
{
    // Display todos
    int i = 1;
    ifstream todos(mainFile);
    if (todos.is_open())
    {
        string line;
        while (getline(todos, line))
        {
            cout << i << ". " << line << endl;
            i++;
        }
        todos.close();
    }
    else
    {
        cerr << "Unable to open file\n";
        cerr << "path: " << mainFile << endl;
        exit(1);
    }
}

// Function to add a new todo
void addTodo(string todoName)
{
    // Append Todo to File
    ofstream todoFile(mainFile, ios::app);
    todoFile << todoName << endl;
    todoFile.close();
}

int main(int argc, char *argv[])
{
    cout << "starting" << endl;

    // Create Todo database file if does not exist
    // system("mkdir -p $HOME/.config/todominal");
    // system("touch $HOME/.config/todominal/todos.txt");
    int argNum = argc - 1; // To avoid confusion

    // If 1 or more arguments are passed (CLI Mode)
    if (argNum > 0)
    {
        // Store arguments in variables
        string firstArg;
        string secondArg;
        // Usage (Help Box)
        string helpBox =
            "Usage:\n   todominal add {Task Name (string)}\n   todominal "
            "remove {Task Index (int)}\n   todominal done {Task Index (int)}\n "
            "  todominal clearall\n   todominal list\n   todominal "
            "help\nExample:\n   todominal remove 2\n   todominal add \"Hello "
            "World!\"\n   todominal done 3\n";
        if (argNum == 1)
        {
            firstArg = argv[1];
            if (firstArg == "clearall")
            {
                ofstream toErase(mainFile, ofstream::out | ofstream::trunc);
                toErase.close();
            }
            else if (firstArg == "help")
            {
                cout << helpBox;
            }
            else if (firstArg == "list")
            {
                printTodo();
            }
            else
            {
                cout << helpBox;
            }
        }
        else if (argNum == 2)
        {
            firstArg = argv[1];
            secondArg = argv[2];
            if (firstArg == "add")
            {
                addTodo(argv[2]);
            }
            else if (firstArg == "remove")
            {
                int lineIndex = stoi(argv[2]);
                string Task = getLineContent(mainFile, lineIndex);
                removeLine(mainFile, lineIndex);
            }
            else if (firstArg == "done")
            {
                int lineIndex = stoi(argv[2]);
                string Task = getLineContent(mainFile, lineIndex);
                replaceLine(mainFile, lineIndex,
                            strikethrough(getLineContent(mainFile, lineIndex)));
            }
            else
            {
                cout << helpBox;
            }
        }
        else
        {
            cout << "Only 2 arguments are allowed!" << endl;
        }
    }
    // If no arguments are passed (Interactive CLI Mode)
    else
    {
        // Input Variables
        string userInput;
        string otherInput;
        string eraseConfirmation;

        // Main Loop
        bool run = true;
        while (run)
        {
            // Header
            system("clear");
            system("figlet TODOMINAL");
            cout << "ToDo List in your Terminal!" << endl << endl;
            system("echo Hey, $(whoami).");

            printTodo();

            // User input
            cout << "\n(A)dd Todo || (R)emove Todo || (M)ark Done || (O)ptions "
                    "|| (E)xit\n>> ";
            std::cin >> userInput;

            // Handle user input
            if (userInput == "A" || userInput == "a")
            {
                string todoName;
                cout << "Enter New Todo Name: ";
                cin.ignore();
                getline(cin, todoName);
                addTodo(todoName);
            }
            else if (userInput == "R" || userInput == "r")
            {
                removeTodo();
            }
            else if (userInput == "M" || userInput == "m")
            {
                markDone();
            }
            else if (userInput == "O" || userInput == "o")
            {
                cout << "1. Clear All Todo\n(E)xit\n>> ";
                std::cin >> otherInput;
                if (otherInput == "1")
                {
                    cout << "Are you sure? (Y/N): ";
                    std::cin >> eraseConfirmation;
                    if (eraseConfirmation == "y" || eraseConfirmation == "Y")
                    {
                        ofstream toErase(mainFile,
                                         ofstream::out | ofstream::trunc);
                        toErase.close();
                    }
                    else
                    {
                    };
                }
                else if (otherInput == "E" || otherInput == "e")
                {
                } // Exit
                else
                {
                    cerr << "Enter a valid response!";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
            else if (userInput == "E" || userInput == "e")
            {
                system("clear");
                exit(0);
            }
            else
            {
                cerr << "Enter a valid response!";
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }
    return 0;
}
