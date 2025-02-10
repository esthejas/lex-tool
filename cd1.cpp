#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <regex>

using namespace std;

unordered_set<string> keywords = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while",
    "malloc", "calloc", "realloc", "free"
};

unordered_set<string> operators = { "+", "-", "*", "/", "%", "=", "+=", "-=", "*=", "/=", "%=", "++", "--", "&&", "||", "!", "?", ":", "&", "|", "^", "<<", ">>" };
unordered_set<string> relational_operators = { "==", "!=", "<", "<=", ">", ">=" };
unordered_set<char> punctuation = { ';', ',', '(', ')', '{', '}', '[', ']' };

bool isIdentifier(const string& lexeme) {
    return regex_match(lexeme, regex("[a-zA-Z_][a-zA-Z0-9_]*"));
}

bool isNumber(const string& lexeme) {
    return regex_match(lexeme, regex("\\d+(\\.\\d+)?"));
}

void analyze(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    string line;
    bool in_multiline_comment = false;
    cout << left << setw(20) << "Lexeme" << setw(30) << "Category" << "Token" << endl;

    while (getline(file, line)) {
        size_t i = 0;
        while (i < line.length()) {
            if (!in_multiline_comment && line.substr(i, 2) == "/*") {
                in_multiline_comment = true;
                i += 2;
                continue;
            }
            if (in_multiline_comment) {
                size_t end_comment = line.find("*/", i);
                if (end_comment != string::npos) {
                    in_multiline_comment = false;
                    i = end_comment + 2;
                } else {
                    break; // Skip the entire line if still in comment
                }
                continue;
            }

            // Skip single-line comments
            size_t comment_pos = line.find("//", i);
            if (comment_pos != string::npos) {
                line = line.substr(0, comment_pos);
            }

            if (isspace(line[i])) {
                i++;
                continue;
            }

            if (line[i] == '"') { // Handle string literals
                size_t end = line.find('"', i + 1);
                if (end != string::npos) {
                    string str_literal = line.substr(i, end - i + 1);
                    cout << setw(20) << str_literal << setw(30) << "Other" << "<OT, " << str_literal << ">" << endl;
                    i = end + 1;
                    continue;
                }
            }

            if (punctuation.count(line[i])) { // Handle punctuation
                cout << setw(20) << line[i] << setw(30) << "Punctuation Symbol" << "<PS, " << line[i] << ">" << endl;
                i++;
                continue;
            }

            // Check for multi-character operators
            bool matched_operator = false;
            for (const auto& op : operators) {
                if (line.substr(i, op.size()) == op) {
                    cout << setw(20) << op << setw(30) << "Operator" << "<OP, " << op << ">" << endl;
                    i += op.size();
                    matched_operator = true;
                    break;
                }
            }
            if (matched_operator) continue;

            // Extract token
            size_t j = i;
            while (j < line.length() && !isspace(line[j]) && !punctuation.count(line[j]) && operators.count(string(1, line[j])) == 0) j++;
            string token = line.substr(i, j - i);

            if (keywords.count(token)) {
                cout << setw(20) << token << setw(30) << "Keyword" << "<KW, " << token << ">" << endl;
            } else if (relational_operators.count(token)) {
                cout << setw(20) << token << setw(30) << "Relational Operator" << "<RO, " << token << ">" << endl;
            } else if (isNumber(token)) {
                cout << setw(20) << token << setw(30) << "Number" << "<NUM, " << token << ">" << endl;
            } else if (isIdentifier(token)) {
                cout << setw(20) << token << setw(30) << "Identifier" << "<ID, " << token << ">" << endl;
            } else if (!token.empty()) {
                cout << setw(20) << token << setw(30) << "Other" << "<OT, " << token << ">" << endl;
            }
            i = j;
        }
    }

    file.close();
}

int main() {
    string filename;
    cout << "Enter the filename (e.g., Program.c): ";
    cin >> filename;

    analyze(filename);

    return 0;
}
