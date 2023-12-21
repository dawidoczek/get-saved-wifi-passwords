#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    try {
        std::ofstream outputFile("output.txt");
        if (!outputFile.is_open()) {
            std::cerr << "Error opening output file." << std::endl;
            return 1;
        }

        std::string netshCommand = "netsh WLAN show profiles";

        std::string output = exec(netshCommand.c_str());

        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line)) {
            size_t colonPos = line.find(":");

            if (colonPos != std::string::npos && colonPos + 2 < line.length()) {
                std::string substring = line.substr(colonPos + 2);

                std::string netshOutput = exec((netshCommand + " name=\"" + substring + "\" key=clear").c_str());

                size_t securityKeyPos = netshOutput.find("Security key");
                if (securityKeyPos != std::string::npos) {
                    size_t valuePos = netshOutput.find(":", securityKeyPos);
                    if (valuePos != std::string::npos) {
                        std::string securityValue = netshOutput.substr(valuePos + 2, 7);

                        if (securityValue == "Present") {
                            size_t keyContentPos = netshOutput.find("Key Content", valuePos);
                            if (keyContentPos != std::string::npos) {
                                outputFile << "login: " << substring << "\nhaslo: "
                                           << netshOutput.substr(netshOutput.find(":", keyContentPos) + 2,
                                                                netshOutput.find("Cost", netshOutput.find(":", keyContentPos) + 2) - keyContentPos - 25)
                                           << "\n\n";
                            } else {
                                outputFile << "nie ma hasla dla: " << substring << "\n\n\n";
                            }
                        } else {
                            outputFile << "Siec pewnie jest otwarta " << substring << "\n\n\n";
                        }
                    }
                }
            }
        }
        outputFile.close();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        
    }

    return 0;
}
