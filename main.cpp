#include <bits/stdc++.h>
using namespace std;

struct Term { string var; bool neg; int w; };
struct Constraint { vector<Term> terms; string cmp; int k; };

Constraint parse_line(const string& line) {
    size_t pos;
    string cmp;
    if ((pos = line.find("<=")) != string::npos) cmp = "<=";
    else if ((pos = line.find(">=")) != string::npos) cmp = ">=";
    else if ((pos = line.find("=")) != string::npos) cmp = "=";
    else throw runtime_error("No comparator");

    string lhs = line.substr(0, pos);
    string rhs = line.substr(pos + cmp.size());

    stringstream ss(lhs);
    string tok;
    vector<string> tokens;
    while (ss >> tok) tokens.push_back(tok);

    vector<Term> terms;
    int sign = +1;
    for (auto &t : tokens) {
        if (t == "+") { sign = +1; continue; }
        if (t == "-") { sign = -1; continue; }

        int w = 1;
        string var = t;
        bool litneg = false;

        size_t star = t.find('*');
        if (star != string::npos) {
            w = stoi(t.substr(0, star));
            var = t.substr(star + 1);
        }

        if (!var.empty() && var[0] == '~') {
            litneg = true;
            var = var.substr(1);
        }

        terms.push_back({var, litneg, sign * w});
    }

    int k = stoi(rhs);
    return Constraint{terms, cmp, k};
}

void normalize(Constraint &c) {
    vector<Term> out;
    for (auto &t : c.terms) {
        if (t.w >= 0) out.push_back(t);
        else {
            int w = -t.w;
            out.push_back({t.var, !t.neg, w});
            c.k -= w;
        }
    }
    c.terms.swap(out);
}

int main() {
    string line;
    getline(cin, line);
    try {
        auto c = parse_line(line);
        normalize(c);

        for (auto &t : c.terms)
            cout << t.w << "*" << (t.neg ? "~" : "") << t.var << "  ";
        cout << c.cmp << " " << c.k << "\n";
    } catch (exception &e) {
        cerr << "Error: " << e.what() << "\n";
    }
}
