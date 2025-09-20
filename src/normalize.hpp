#pragma once
#include "core.hpp"
#include <string>
#include <vector>
#include <set>

namespace satenc{

    class Normalizer{

    public:
        Constraint parse_line(const string& line, set<string>& var_names);
        void normalize_inplace(Constraint &c);
        vector<Constraint> to_geq(const Constraint& c_in);
    };

}