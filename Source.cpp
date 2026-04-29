#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

struct Expense {
    string name;
    double amount;
    set<string> excluded;
};
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}
void solve() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    if (!input.is_open()) {
        cerr << "File not found!" << endl;
        return;
    }
    string line;
    vector<string> participants;
    vector<Expense> expenses;
    getline(input, line);
    istringstream iss(line);
    int count;
    iss >> count;
    string name;
    while (iss >> name) {
        participants.push_back(name);
    }
    while (getline(input, line)) {
        if (line.empty()) continue;
        Expense exp;
        size_t slash_pos = line.find('/');

        if (slash_pos != string::npos) {
            string before_slash = line.substr(0, slash_pos);
            string after_slash = line.substr(slash_pos + 1);

            size_t colon_pos = before_slash.find(':');
            if (colon_pos == string::npos) {
                istringstream iss(before_slash);
                iss >> exp.name >> exp.amount;
            }
            else {
                exp.name = before_slash.substr(0, colon_pos);
                exp.name.erase(exp.name.find_last_not_of(" \t") + 1);
                string amount_str = before_slash.substr(colon_pos + 1);
                amount_str.erase(0, amount_str.find_first_not_of(" \t"));
                exp.amount = stod(amount_str);
            }
            vector<string> excluded_names = split(after_slash, ',');
            for (const string& name : excluded_names) {
                exp.excluded.insert(name);
            }
        }
        else {
            size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                exp.name = line.substr(0, colon_pos);
                exp.name.erase(exp.name.find_last_not_of(" \t") + 1);
                string amount_str = line.substr(colon_pos + 1);
                amount_str.erase(0, amount_str.find_first_not_of(" \t"));
                exp.amount = stod(amount_str);
            }
            else {
                istringstream iss(line);
                iss >> exp.name >> exp.amount;
            }
        }
        expenses.push_back(exp);
    }
    input.close();
    map<string, double> total_expenses;
    for (const string& name : participants) {
        total_expenses[name] = 0.0;
    }
    for (const Expense& exp : expenses) {
        total_expenses[exp.name] += exp.amount;
    }
    map<string, double> should_pay;
    for (const string& name : participants) {
        should_pay[name] = 0.0;
    }
    for (const Expense& exp : expenses) {
        vector<string> included;
        for (const string& name : participants) {
            if (exp.excluded.find(name) == exp.excluded.end()) {
                included.push_back(name);
            }
        }
        double share = exp.amount / included.size();
        for (const string& name : included) {
            should_pay[name] += share;
        }
    }
    output << fixed << setprecision(1);
    for (const string& name : participants) {
        output << name << " " << total_expenses[name] << " " << should_pay[name] << endl;
    }
    map<string, double> balance;
    for (const string& name : participants) {
        balance[name] = total_expenses[name] - should_pay[name];
    }
    vector<pair<string, double>> creditors;
    vector<pair<string, double>> debtors;
    for (const string& name : participants) {
        if (balance[name] > 1e-9) {
            creditors.push_back({ name, balance[name] });
        }
        else if (balance[name] < -1e-9) {
            debtors.push_back({ name, -balance[name] });
        }
    }
    sort(creditors.begin(), creditors.end(),
        [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });
    sort(debtors.begin(), debtors.end(),
        [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });
    size_t i = 0, j = 0;
    while (i < debtors.size() && j < creditors.size()) {
        double amount = min(debtors[i].second, creditors[j].second);
        if (amount > 1e-9) {
            output << debtors[i].first << " " << amount << " " << creditors[j].first << endl;
        }

        debtors[i].second -= amount;
        creditors[j].second -= amount;

        if (debtors[i].second < 1e-9) i++;
        if (creditors[j].second < 1e-9) j++;
    }
    output.close();
}
int main() {
    solve();
    return 0;
}
