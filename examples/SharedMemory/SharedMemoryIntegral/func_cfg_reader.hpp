
#ifndef TEMPLATE_FUNC_CFG_READER_HPP
#define TEMPLATE_FUNC_CFG_READER_HPP

#include <map>
#include <iostream>

enum reading_stance {
    KEYREAD,
    VALUEREAD
};

void read_arguments(std::istream& config, std::map<std::string, double>& configmap) {
    reading_stance stance = KEYREAD;
    std::string line, curr_key, curr_value;

    while (!config.eof()) {
        std::getline(config, line);
        for (char& c : line) {
            if (c == '#' || c == '\n') {
                stance = KEYREAD;
                break;
            }
            else if (isspace(c)) {
                continue;
            }
            if (c == '=') {
                stance = VALUEREAD;
                continue;
            }
            switch (stance) {
                case KEYREAD :
                    curr_key += c;
                    break;
                case VALUEREAD:
                    curr_value += c;
                    break;
            }
        }
        if (!curr_key.empty()){
            configmap.insert({curr_key, stod(curr_value)});
            curr_key.clear();
            curr_value.clear();
        }
    }
}

#endif //TEMPLATE_FUNC_CFG_READER_HPP
