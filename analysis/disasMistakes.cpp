#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdint>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

void printAsBinary(uint32_t value) {
    char s[32];
    for (int i = 31; i >= 0; --i) {
        s[31 - i] = (value & (1 << i)) ? '1' : '0';
    }
    printf("%s\n", s);
}

struct Field {
    Field() {};
    Field(uint32_t m, uint32_t s) : mask(m), shift(s) {};
    uint32_t mask;
    uint32_t shift;
};

class Formatter {
public:
    Formatter() {};
    bool satisfiesFormat(uint32_t insn);
    void addField(string fieldString);
    void addRule(string ruleString);
    std::vector<std::function<bool(uint32_t)>> rules;
    void finish(uint32_t insn);
    uint32_t extractField(string fieldName, uint32_t insn);
    map<string, Field> fieldMap;
    uint32_t fixedForm;
    uint32_t significantMask;
private:
};

void Formatter::finish(uint32_t insn) {
    significantMask = -1;
    for (const auto& field : fieldMap) {
        significantMask &= ~field.second.mask;
    }
    fixedForm = insn & significantMask;
}

uint32_t Formatter::extractField(string fieldName, uint32_t insn) {
    const Field& field = fieldMap[fieldName];
    return (insn & field.mask) >> field.shift;
}

bool Formatter::satisfiesFormat(uint32_t insn) {
	//printAsBinary(insn);
	//printf("t: %u, t2: %u, n: %u\n", extractField("t", insn), extractField("t2", insn), extractField("n", insn));
    if ((insn & significantMask) == fixedForm) {
        //printf("fixedForm satisfied\n");
        for (const auto& rule : rules) {
            if (rule(insn)) {
                //printf("rule true\n");
                return true;
            }
            //printf("rule false\n");
        }
    }
    return false;
}

void Formatter::addField(string fieldString) {
    size_t split = fieldString.find_first_of(" ");
    string fieldName = fieldString.substr(0, split);
    size_t split2 = fieldString.find_last_of(" ");
    string shiftString = fieldString.substr(split + 1, split2 - split);
    string lenString = fieldString.substr(split2 + 1);
    uint32_t len = stoi(lenString);
    uint32_t shift = stoi(shiftString);
    uint32_t mask = ((uint32_t)pow(2, len) - 1) << shift;
    Field f(mask, shift);
    fieldMap.insert(std::pair<string, Field>(fieldName, f));
}



void Formatter::addRule(string ruleString) { //manual :(

}

std::vector<Formatter*> formatters;

void initFormatters() {
    Formatter* formatter = new Formatter; //INS
    formatter->rules.push_back([=](uint32_t insn) {
        return (formatter->extractField("imm5", insn) != 0b10000);
    });
    formatter->addField("imm5 16 5");
    formatter->addField("imm4 11 4");
    formatter->addField("regs 0 10");
    formatter->finish(1847492607);
    formatters.push_back(formatter);

    formatter = new Formatter; //INS
    formatter->rules.push_back([=](uint32_t insn) {
        return (formatter->extractField("imm5", insn) != 0b10000);
    });
    formatter->addField("imm5 16 5");
    formatter->addField("imm4 11 4");
    formatter->addField("regs 0 10");
    formatter->finish(1310658559);
    formatters.push_back(formatter);

    formatter = new Formatter; //INS
    formatter->rules.push_back([=](uint32_t insn) {
        return (formatter->extractField("imm5", insn) != 0b10000);
    });
    formatter->addField("imm5 16 5");
    formatter->addField("regs 0 10");
    formatter->finish(236916735);
    formatters.push_back(formatter);
}

uint32_t readInt(string s) {
    return stoul(s, nullptr, 10);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Need the following arguments: file with insns" << std::endl;
    }
    initFormatters();
    string line;
    uint32_t insn, prevInsn;
    ifstream insns;
    insns.open(argv[1], ios_base::in);

    Formatter formatter;
    formatter.addRule("dummy arg");

    // LDADD LDOR
    formatter.addField("t 0 5");
    formatter.addField("n 5 5");
    formatter.addField("o3 15 1");
    formatter.addField("s 16 5");
    formatter.addField("ar 22 2");
    formatter.addField("size 30 2");
    formatter.addField("opc 12 3");

    bool isUnpredictable = false;
    getline(insns, line);
    insn = readInt(line);
    while (true) {
        isUnpredictable = false;
        for (auto& formatter : formatters) {
            isUnpredictable |= formatter->satisfiesFormat(insn);
        }
        if (!isUnpredictable) {
            printf("%u\n", insn);
        }

        getline(insns, line);
        if (insns.eof()) {
            break;
        }
        insn = readInt(line);
    }
}
