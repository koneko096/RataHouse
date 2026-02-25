#include "YamlParser.h"
#include "Solver.h"
#include <iostream>
#include <stdexcept>

// ---- YamlParser implementation ----

std::string YamlParser::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

int YamlParser::getIndent(const std::string& line) {
    int count = 0;
    for (char c : line) {
        if (c == ' ') count++;
        else break;
    }
    return count;
}

std::vector<YamlParser::Line> YamlParser::tokenize(const std::string& content) {
    std::vector<Line> lines;
    std::istringstream stream(content);
    std::string rawLine;

    while (std::getline(stream, rawLine)) {
        // Remove \r if present
        if (!rawLine.empty() && rawLine.back() == '\r')
            rawLine.pop_back();

        // Skip empty lines and comments
        std::string trimmed = trim(rawLine);
        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        Line line;
        line.raw = rawLine;
        line.indent = getIndent(rawLine);
        line.isListItem = false;

        // Check if it's a list item (starts with "- ")
        std::string work = trimmed;
        if (work.size() >= 2 && work[0] == '-' && work[1] == ' ') {
            line.isListItem = true;
            work = trim(work.substr(2));
            // Adjust indent to account for "- " prefix
            line.indent += 2;
        }

        // Split key: value
        size_t colonPos = work.find(':');
        if (colonPos != std::string::npos) {
            line.key = trim(work.substr(0, colonPos));
            std::string afterColon = trim(work.substr(colonPos + 1));
            if (!afterColon.empty()) {
                line.value = afterColon;
            }
        } else {
            // Scalar value (like a bare list item value)
            line.value = work;
        }

        lines.push_back(line);
    }

    return lines;
}

YamlNode YamlParser::buildNode(const std::vector<Line>& lines, size_t& index, int parentIndent) {
    YamlNode node;

    while (index < lines.size()) {
        const Line& line = lines[index];

        // If we've dedented past our parent, we're done with this node
        if (line.indent <= parentIndent && index > 0) {
            break;
        }

        if (line.isListItem) {
            // This node is a list
            node.isList = true;

            if (!line.key.empty() && line.value.empty()) {
                // List item with nested map: "- key:"
                index++;
                YamlNode child = buildNode(lines, index, line.indent);
                // The key on this list line is the first key of the child map
                child.isMap = true;
                child.map[line.key] = YamlNode(); // placeholder
                // Actually, we need to re-parse: the list item line has a key
                // and the subsequent indented lines are children of that key
                // Let's handle this differently
                YamlNode listItem;
                listItem.isMap = true;

                // Check if there are children at deeper indent
                YamlNode subChildren = buildNode(lines, index, line.indent);
                listItem.map[line.key] = subChildren;

                // Merge any remaining keys at the same indent
                // (handled by buildNode returning them)

                // Actually let me rethink...
                // Revert and handle more carefully
            } else if (!line.key.empty() && !line.value.empty()) {
                // List item with key-value on same line: "- key: value"
                // This starts a new map in the list
                YamlNode listItem;
                listItem.isMap = true;
                YamlNode valNode;
                valNode.isScalar = true;
                valNode.value = line.value;
                listItem.map[line.key] = valNode;

                index++;
                // Collect remaining keys at the same indent level
                while (index < lines.size() && lines[index].indent >= line.indent
                       && !lines[index].isListItem) {
                    const Line& subLine = lines[index];
                    if (subLine.indent < line.indent) break;

                    if (!subLine.value.empty()) {
                        YamlNode sv;
                        sv.isScalar = true;
                        sv.value = subLine.value;
                        listItem.map[subLine.key] = sv;
                        index++;
                    } else {
                        // Nested structure
                        index++;
                        YamlNode nested = buildNode(lines, index, subLine.indent);
                        listItem.map[subLine.key] = nested;
                    }
                }
                node.list.push_back(listItem);
            } else if (line.value.empty() && line.key.empty()) {
                // Bare list item - shouldn't happen in our schema
                index++;
            } else {
                // List item is just a scalar "- value"
                YamlNode listItem;
                listItem.isScalar = true;
                listItem.value = line.value;
                node.list.push_back(listItem);
                index++;
            }
        } else if (!line.key.empty()) {
            // Map entry
            node.isMap = true;

            if (!line.value.empty()) {
                // key: value on the same line
                YamlNode child;
                child.isScalar = true;
                child.value = line.value;
                node.map[line.key] = child;
                index++;
            } else {
                // key: with children on subsequent lines
                index++;
                YamlNode child = buildNode(lines, index, line.indent);
                node.map[line.key] = child;
            }
        } else {
            index++;
        }
    }

    return node;
}

YamlNode YamlParser::parseString(const std::string& content) {
    std::vector<Line> lines = tokenize(content);
    size_t index = 0;
    return buildNode(lines, index, -1);
}

YamlNode YamlParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open YAML file: " + filename);
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    return parseString(content);
}

// ---- Helper functions to extract values ----

static std::string getStr(const YamlNode& node, const std::string& key, const std::string& defaultVal = "") {
    auto it = node.map.find(key);
    if (it != node.map.end() && it->second.isScalar) {
        return it->second.value;
    }
    return defaultVal;
}

static int getInt(const YamlNode& node, const std::string& key, int defaultVal = 0) {
    auto it = node.map.find(key);
    if (it != node.map.end() && it->second.isScalar) {
        return std::atoi(it->second.value.c_str());
    }
    return defaultVal;
}

// ---- Load YAML into solver globals ----

void YamlFileInput() {
    YamlNode root = YamlParser::parseFile(std::string(input));

    // Grid settings
    YamlNode& grid = root.map["grid"];
    totalSlot = getInt(grid, "total_slots", 48);
    powerLimit = getInt(grid, "power_limit", 600);

    // Pricing intervals
    YamlNode& pricing = root.map["pricing"];
    YamlNode& yamlIntervals = pricing.map["intervals"];

    ninterval = (int)yamlIntervals.list.size();

    // Determine number of pricing levels from the first interval's tiers
    ProLevel = 0;
    if (ninterval > 0) {
        auto it = yamlIntervals.list[0].map.find("tiers");
        if (it != yamlIntervals.list[0].map.end()) {
            ProLevel = (int)it->second.list.size();
        }
    }

    // Clear and resize
    intervals.clear();
    ProLimit.clear();
    proCost.clear();
    devices.clear();

    intervals.resize(ninterval + 2);
    ProLimit.resize(ProLevel);
    proCost.resize(ProLevel);

    for (int i = 0; i < ProLevel; ++i) {
        proCost[i].resize(ninterval);
    }

    // Parse each interval
    for (int j = 0; j < ninterval; ++j) {
        YamlNode& intv = yamlIntervals.list[j];
        int startHour = getInt(intv, "start_hour");
        int endHour = getInt(intv, "end_hour");

        // Convert hours to half-hour slots (multiply by 2)
        intervals[j].begin = startHour * 2;
        intervals[j].end = endHour * 2;

        // Parse tiers
        auto tiersIt = intv.map.find("tiers");
        if (tiersIt != intv.map.end()) {
            for (int i = 0; i < ProLevel && i < (int)tiersIt->second.list.size(); ++i) {
                YamlNode& tier = tiersIt->second.list[i];
                std::string maxWattsStr = getStr(tier, "max_watts", "unlimited");

                if (j == 0) {
                    // Set ProLimit from the first interval's tiers
                    if (maxWattsStr == "unlimited") {
                        ProLimit[i] = 999999;
                    } else {
                        ProLimit[i] = std::atoi(maxWattsStr.c_str());
                    }
                }

                proCost[i][j] = getInt(tier, "cost_per_slot");
            }
        }
    }

    // Parse devices
    YamlNode& yamlDevices = root.map["devices"];
    ndevice = (int)yamlDevices.list.size();
    devices.resize(ndevice);

    for (int i = 0; i < ndevice; ++i) {
        YamlNode& dev = yamlDevices.list[i];
        devices[i].name = getStr(dev, "name");
        devices[i].power = getInt(dev, "power");
        devices[i].slot = getInt(dev, "slots_needed");

        // Convert hours to half-hour slots
        devices[i].permittedRange.begin = getInt(dev, "permitted_start") * 2;
        devices[i].permittedRange.end = getInt(dev, "permitted_end") * 2;

        std::string typeStr = getStr(dev, "type", "optional");
        devices[i].wajib = (typeStr == "mandatory");

        devices[i].nyala = getInt(dev, "sessions", 1);
    }
}
