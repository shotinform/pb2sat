#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

namespace satenc{

    struct Term { 
        string var;
        bool neg;
        int w; 
    };

    struct Constraint {
        vector<Term> terms;
        string cmp;
        int k;
    };

    struct CNF {
        int nvars = 0;
        vector<vector<int>> clauses;
        
        int newVar();
        void addClause(const vector<int>& c);
        void dumpDIMACS(ostream& os) const;
    };

    struct SymTab {
        unordered_map<string,int> id;

        int get(const string& v, CNF& cnf);
    };

    int to_lit(const Term& t, SymTab& st, CNF& cnf);
    bool all_weights_one(const Constraint& c);
    void print_all_constraints(const vector<Constraint>& constraints);
    void print_constraint(const Constraint& c);

    class SeqCounter {
        
    public:
        void geq(const std::vector<int>& X, int k, CNF& cnf);
    };

    class AdderNetwork{
    
    public:
        vector<int> build_sum_bits(const vector<pair<int,int>>& wlits, CNF& cnf);
        int ge_constant(const vector<int>& S, long long k, CNF& cnf);        
    };
}