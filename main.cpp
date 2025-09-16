#include <bits/stdc++.h>
#include <vector>
using namespace std;

struct Term { string var; bool neg; int w; };
struct Constraint { vector<Term> terms; string cmp; int k; };

vector<Constraint> constraints;

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

vector<Constraint> to_geq(const Constraint& c) {
    long long sumW = 0;
    for (auto& t : c.terms) sumW += t.w;

    if (c.cmp == ">=") {
        return {c}; // already ≥
    } else if (c.cmp == "<=") {
        Constraint nc;
        nc.cmp = ">=";
        nc.k = sumW - c.k;
        for (auto& t : c.terms) {
            nc.terms.push_back({t.var, !t.neg, t.w}); // flip literal
        }
        return {nc};
    } else if (c.cmp == "=") {
        Constraint c1 = c; c1.cmp = ">="; c1.k = c.k;
        Constraint c2; c2.cmp = ">="; c2.k = sumW - c.k;
        for (auto& t : c.terms) c2.terms.push_back({t.var, !t.neg, t.w});
        return {c1, c2};
    }
    throw runtime_error("Unknown comparator");
}

void normalize_inplace(Constraint &c) {
    vector<Term> out;
    for (auto &t : c.terms) {
        if (t.w >= 0) out.push_back(t);
        else {
            int w = -t.w;
            out.push_back({t.var, !t.neg, w});
            c.k += w;
        }
    }
    c.terms.swap(out);
}

void process_and_store(Constraint& c) {
    normalize_inplace(c);
    std::vector<Constraint> geqs = to_geq(c);
    for (const auto& g : geqs) constraints.push_back(g); // simple copies
}

void print_constraint(const Constraint& c){
    for (auto &t : c.terms)
        cout << t.w << "*" << (t.neg ? "~" : "") << t.var << "  ";
    cout << c.cmp << " " << c.k << "\n";
}

void print_all_constraints(){
    for (auto& c : constraints)
        print_constraint(c);
}

int main() {
    string line;
    getline(cin, line);
    try {
        auto c = parse_line(line);
        cout << "Parsed constraint" << endl;
        print_constraint(c);
    
        process_and_store(c);        
    } catch (exception &e) {
        cerr << "Error: " << e.what() << "\n";
    }
    print_all_constraints();
}
