#include "core.hpp"
#include <iostream>

namespace satenc {

    int CNF::newVar() { return ++nvars; }

    void CNF::addClause(const vector<int>& c) { clauses.push_back(c); }

    void CNF::dumpDIMACS(ostream& os) const {
        os << "p cnf " << nvars << " " << clauses.size() << "\n";
        for (auto& cl : clauses) { 
            for (int lit : cl) 
                os << lit << " "; 
            os << "0\n"; 
        }
    }

    int SymTab::get(const string& v, CNF& cnf) {
        auto it = id.find(v);
        if (it != id.end()) 
            return it->second;
        int vid = cnf.newVar();
        id.emplace(v, vid);
        return vid;
    }

    int to_lit(const Term& t, SymTab& st, CNF& cnf) {
        int v = st.get(t.var, cnf);
        return t.neg ? -v : +v;
    }

    bool all_weights_one(const Constraint& c){
        for (auto& t : c.terms)
            if (t.w != 1)
                return false;

        return true;
    }

    void print_constraint(const Constraint& c){
        for (auto &t : c.terms)
            cout << t.w << "*" << (t.neg ? "~" : "") << t.var << "  ";
        cout << c.cmp << " " << c.k << "\n";
    }

    void print_all_constraints(const vector<Constraint>& constraints){
        for (auto& c : constraints)
            print_constraint(c);
    }
}