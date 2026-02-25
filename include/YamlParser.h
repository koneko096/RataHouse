#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

// A lightweight YAML node for our specific schema
struct YamlNode {
    std::string value;
    std::map<std::string, YamlNode> map;
    std::vector<YamlNode> list;
    bool isScalar;
    bool isList;
    bool isMap;

    YamlNode() : isScalar(false), isList(false), isMap(false) {}
};

class YamlParser {
public:
    // Parse a YAML file and return the root node
    static YamlNode parseFile(const std::string& filename);

    // Parse YAML content string and return the root node
    static YamlNode parseString(const std::string& content);

private:
    struct Line {
        int indent;
        std::string key;
        std::string value;
        bool isListItem;
        std::string raw;
    };

    static std::vector<Line> tokenize(const std::string& content);
    static int getIndent(const std::string& line);
    static std::string trim(const std::string& s);
    static YamlNode buildNode(const std::vector<Line>& lines, size_t& index, int parentIndent);
};

// Load the YAML input file into solver global variables
void YamlFileInput();

#endif // YAML_PARSER_H
