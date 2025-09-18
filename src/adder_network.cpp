#include "core.hpp"

using namespace std;

namespace satenc{

    pair<int,int> full_adder(int x, int y, int z, CNF& cnf);

    static pair<int,int> half_adder(int x, int y, CNF& cnf){
        int s = cnf.newVar();
        int c = cnf.newVar();
        // c = x & y
        cnf.addClause({ -x, -y,  c });
        cnf.addClause({  x, -c });
        cnf.addClause({  y, -c });
        // s = x XOR y
        cnf.addClause({ -s, -x, -y });
        cnf.addClause({ -s,  x,  y });
        cnf.addClause({  s, -x,  y });
        cnf.addClause({  s,  x, -y });
        return {s,c};
    }

    vector<int> AdderNetwork::build_sum_bits(const vector<pair<int,int>>& wlits, CNF& cnf){
        // max bit-width
        int B = 0;
        long long sumW = 0;
        for (auto& p: wlits){
            sumW += p.second;
        }
        while ((1LL<<B) <= sumW)
            ++B;
        if (B==0)
            B=1;

        vector<vector<int>> cols(B+2);          

        // scatter weighted literals into bit-columns
        for (auto& [L,w] : wlits){
            int w1 = w;
            for (int j=0; w1; ++j, w1 >>= 1){
                if (w1 & 1) cols[j].push_back(L);
            }
        }

        // full adders for triples, half adders for pairs
        for (int j=0; j<B+1; ++j){
            // while >=3 bits -> full adder
            while ((int)cols[j].size() >= 3){
                int x = cols[j].back(); cols[j].pop_back();
                int y = cols[j].back(); cols[j].pop_back();
                int z = cols[j].back(); cols[j].pop_back();
                auto [s,c] = full_adder(x,y,z, cnf);
                cols[j].push_back( s );
                cols[j+1].push_back( c );
            }
            // if exactly 2 bits -> half adder
            if ((int)cols[j].size() == 2){
                int x = cols[j].back(); cols[j].pop_back();
                int y = cols[j].back(); cols[j].pop_back();
                auto [s,c] = half_adder(x,y, cnf);
                cols[j].push_back( s );
                cols[j+1].push_back( c );
            }
        }

        // Collect S: one bit per column (missing bit means 0)
        vector<int> S(B+1, 0);
        for (int j=0; j<=B; ++j){
            if (!cols[j].empty()) S[j] = cols[j].back();   // the remaining bit
            // else S[j] = 0 (implicit constant 0)
        }
        return S;  // little-endian, S[0] = LSB
    }

    pair<int,int> full_adder(int x, int y, int z, CNF& cnf){
        int s = cnf.newVar();   // s_out
        int c = cnf.newVar();   // c_out

        // c is majority(x,y,z)
        cnf.addClause({ -x, -y,  c });
        cnf.addClause({ -x, -z,  c });
        cnf.addClause({ -y, -z,  c });
        cnf.addClause({  x,  y, -c });
        cnf.addClause({  x,  z, -c });
        cnf.addClause({  y,  z, -c });

        // s = x XOR y XOR z  (two XORs: t = x XOR y; s = t XOR z)
        int t = cnf.newVar();
        // t ↔ x XOR y
        cnf.addClause({ -t, -x, -y });
        cnf.addClause({ -t,  x,  y });
        cnf.addClause({  t, -x,  y });
        cnf.addClause({  t,  x, -y });
        // s ↔ t XOR z
        cnf.addClause({ -s, -t, -z });
        cnf.addClause({ -s,  t,  z });
        cnf.addClause({  s, -t,  z });
        cnf.addClause({  s,  t, -z });

        return {s,c};
    }

    
    int AdderNetwork::ge_constant(const vector<int>& S, long long k, CNF& cnf){
        int B = (int)S.size();
        
        vector<int> b(B+1, 0);
        b[0] = cnf.newVar();
        cnf.addClause({ -b[0] }); // b0 = 0

        for (int j=0; j<B; ++j){
            b[j+1] = cnf.newVar();   
            int s = S[j];            
            int kj = (k >> j) & 1;

            if (kj == 0){
                if (s == 0){
                    // bout <-> b_in
                    cnf.addClause({-b[j+1], b[j]});
                    cnf.addClause({b[j+1], -b[j]});
                }
                else {
                    // bout <-> (~s & b_in)
                    cnf.addClause({ -s, -b[j+1] });   
                    cnf.addClause({ -b[j+1],  b[j] });  
                    cnf.addClause({ s, -b[j], b[j+1] });
                }
            } else {
                if (s == 0) {
                    // b_out = true
                    cnf.addClause({ b[j + 1] }); // unit: b_out = true
                } else {
                    // b_out -> (~s | b_in)
                    cnf.addClause({ -b[j + 1], -s,  b[j] });
                    cnf.addClause({  s,  b[j + 1] });
                    cnf.addClause({ -b[j],  b[j + 1] });
                }
            }
        }
        int ge = cnf.newVar();
        cnf.addClause({  ge,  b[B] });
        cnf.addClause({ -ge, -b[B] });
        return ge;
    }


}