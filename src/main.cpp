#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <set>

#include "normalize.hpp"
#include "core.hpp"

using namespace std;
using namespace satenc; 


void process_and_store(Constraint& c, Normalizer& n, vector<Constraint>& constraints) {
    n.normalize_inplace(c);
    std::vector<Constraint> geqs = n.to_geq(c);
    for (const auto& g : geqs) constraints.push_back(g); // simple copies
}

void encode_seqcounter(const Constraint& c, SymTab& st, CNF& cnf, SeqCounter& sc){
    //cout << "All weights are 1" << endl;
    vector<int> X; 
    X.reserve(c.terms.size());
    for (auto& t : c.terms) 
        X.push_back(to_lit(t, st, cnf));

    sc.geq(X, c.k, cnf);
}

void encode_adder_networks(const Constraint& c, SymTab& st, CNF& cnf, AdderNetwork& an){
    vector<pair<int,int>> wlits;
    wlits.reserve(c.terms.size());
    for (auto& t : c.terms){
        int v = st.get(t.var, cnf);
        int L = t.neg ? -v : v;
        wlits.emplace_back(L, t.w);   // (literal, weight)
    }

    vector<int> S = an.build_sum_bits(wlits, cnf);

    int g = an.ge_constant(S, c.k, cnf);

    cnf.addClause({ g });
}


int main() {

    Normalizer n;
    vector<Constraint> constraints;
    string line;
    set<string> var_names;

    while (getline(cin, line)) {   // keep reading until EOF
        try {
            auto c = n.parse_line(line, var_names);
            //print_constraint(c);

            process_and_store(c, n, constraints);
        } catch (exception &e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    cout << endl << "Normalized constraints: " << endl;
    print_all_constraints(constraints);

    CNF cnf;
    SymTab st;
    SeqCounter sc;
    AdderNetwork an;

    for (const auto& c : constraints) {
        if (c.cmp != ">=") { 
            cerr << "Expected canonical >= only\n";
            continue; 
        }
        if (all_weights_one(c)) {
            encode_seqcounter(c, st, cnf, sc);
        }
        else {
            encode_adder_networks(c, st, cnf, an);
        }
        
    }

    cout << endl << "Vars mapping:" << endl; 
    for (const auto& name: var_names)
        cout << name << " -> " << st.get(name, cnf) << " ";
    cout << endl;

    std::ofstream fout("output.cnf");
    if (!fout) {
        cerr << "Error opening output.cnf for writing\n";
        return 1;
    }
    cnf.dumpDIMACS(fout);
    fout.close();
    cout << endl << "Output CNF has been written to output.txt" << endl;

    return 0;
}
