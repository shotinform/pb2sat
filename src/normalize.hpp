#pragma once
#include "core.hpp"
#include <string>
#include <vector>

namespace satenc{

    class Normalizer{

    public:
        Constraint parse_line(const string& line);
        void normalize_inplace(Constraint &c);
        vector<Constraint> to_geq(const Constraint& c);
    };

}