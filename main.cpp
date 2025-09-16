#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

struct Term { string var; bool neg; int w; };
struct Constraint { vector<Term> terms; string cmp; int k; };

struct CNF {
    int nvars = 0;
    vector<vector<int>> clauses;
    
    int newVar() { return ++nvars; }
    
    void addClause(const vector<int>& c) { clauses.push_back(c); }
    
    void dumpDIMACS(ostream& os) const {
        os << "p cnf " << nvars << " " << clauses.size() << "\n";
        for (auto& cl : clauses) { 
            for (int lit : cl) 
                os << lit << " "; 
            os << "0\n"; 
        }
    }
};

struct SymTab {
    unordered_map<string,int> id;

    int get(const string& v, CNF& cnf) {
        auto it = id.find(v);
        if (it != id.end()) return it->second;
        int vid = cnf.newVar();
        id.emplace(v, vid);
        return vid;
    }
};

int to_lit(const Term& t, SymTab& st, CNF& cnf) {
    int v = st.get(t.var, cnf);
    return t.neg ? -v : +v;
}

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

bool all_weights_one(const Constraint& c){
    for (auto& t : c.terms)
        if (t.w != 1)
            return false;

    return true;
}


static void seqcounter_geq(const std::vector<int>& X, int k, CNF& cnf) {
    const int n = (int)X.size();
    if (k <= 0) return;                  // trivially true
    if (k > n) { cnf.addClause({}); return; } // unsat

    const int J = k; 
    std::vector<std::vector<int>> s(n+1, std::vector<int>(J+1, 0));
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= J; ++j)
            s[i][j] = cnf.newVar();

    // Base (i=1)
    cnf.addClause({ -X[0],  s[1][1] });      
    for (int j = 2; j <= J; ++j)             
        cnf.addClause({ -s[1][j] });
    cnf.addClause({  X[0], -s[1][1] });      

    // Inductive (i >= 2)
    for (int i = 2; i <= n; ++i) {
        cnf.addClause({ -X[i-1],  s[i][1] });                     
        for (int j = 1; j <= J; ++j) cnf.addClause({ -s[i-1][j],  s[i][j] });
        for (int j = 1; j <= J-1; ++j) cnf.addClause({ -X[i-1], -s[i-1][j],  s[i][j+1] }); 
        for (int j = 1; j <= J-1; ++j) cnf.addClause({  X[i-1],  s[i-1][j+1], -s[i][j+1] }); 
        for (int j = 1; j <= J-1; ++j) cnf.addClause({  s[i-1][j], s[i-1][j+1], -s[i][j+1] });
    }

    cnf.addClause({ s[n][k] }); 
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

    CNF cnf;
    SymTab st;

    for (const auto& c : constraints) {
        if (c.cmp != ">=") { 
            cerr << "Expected canonical >= only\n";
            continue; 
        }
        if (!all_weights_one(c)) {
            cerr << "Non-unit weights not supported by sequential counter yet\n";
            continue; 
        }
        vector<int> X; 
        X.reserve(c.terms.size());
        for (auto& t : c.terms) 
            X.push_back(to_lit(t, st, cnf));

        seqcounter_geq(X, c.k, cnf);
    } 


    std::ofstream fout("output.cnf");
    if (!fout) {
        cerr << "Error opening output.cnf for writing\n";
        return 1;
    }
    cnf.dumpDIMACS(fout);
    fout.close();

    return 0;
}
