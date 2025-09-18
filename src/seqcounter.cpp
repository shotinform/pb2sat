#include "core.hpp"

using namespace std;

namespace satenc{
    void SeqCounter::seqcounter_geq(const std::vector<int>& X, int k, CNF& cnf) {
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
            for (int j = 0; j <= J-1; ++j) cnf.addClause({  X[i-1],  s[i-1][j+1], -s[i][j+1] }); 
            for (int j = 1; j <= J-1; ++j) cnf.addClause({  s[i-1][j], s[i-1][j+1], -s[i][j+1] });
        }

                        
        //for (int i = 2; i <= n; ++i)
        //    cnf.addClause({ -s[i][1], s[i-1][1], X[i-1] });


        cnf.addClause({ s[n][k] }); 
    }
}