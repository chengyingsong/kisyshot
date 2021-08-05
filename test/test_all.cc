#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

std::string expected(const std::string& path) {
    std::string s;
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in) {
        in.seekg(0, std::ios::end);
        s.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&s[0], long(s.size()));
        in.close();
    }
    return s;
}
struct CommandResult {
    std::string output;
    int exitstatus;

    friend std::ostream& operator<<(std::ostream& os,
                                    const CommandResult& result) {
        os << "command exitstatus: " << result.exitstatus
           << " output: " << result.output;
        return os;
    }
    bool operator==(const CommandResult& rhs) const {
        return output == rhs.output && exitstatus == rhs.exitstatus;
    }
    bool operator!=(const CommandResult& rhs) const { return !(rhs == *this); }
};
CommandResult exec(const std::string& command) {
    int exitcode = 255;
    std::array<char, 1048576> buffer{};
    std::string result;
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITSTATUS
#endif
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        throw std::runtime_error("popen() failed!");
    }
    try {
        std::size_t bytesread;
        while ((bytesread = fread(buffer.data(), sizeof(buffer.at(0)),
                                  sizeof(buffer), pipe)) != 0) {
            result += std::string(buffer.data(), bytesread);
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    int code = pclose(pipe);
    exitcode = WEXITSTATUS(code);
    return CommandResult{result, exitcode};
}

int main(int argc, char* argv[]) {
    //--- filenames are unique so we can use a set
    std::set<std::filesystem::path> sorted;

    for (auto& entry :
         std::filesystem::directory_iterator("cases/function_test2020"))
        sorted.insert(entry.path());

    for (const auto& fpath : sorted) {
        if (fpath.extension() == ".S") {
            std::cout << "file: " << fpath.string() << std::endl;
            std::string p = fpath.string();
            p.replace(p.find(".S"), 2, "");
            std::string in = p + ".in";
            
            std::string out = p + ".out";
            std::cout << exec(("./kisyshot -S -o " + fpath.string() + " " + p + ".sy"));
            if (argc > 1 && std::string(argv[1]) == std::string("-r")) {
                std::cout << exec(
                    ("gcc " + fpath.string() + " libsysy.a -o " + p));
                if(std::filesystem::exists(in))
                    p += " < " + in;
                std::cout << exec(("./" + p));
                std::cout << ", and expected: " << expected(out);
            }
            std::cout << std::endl << std::endl << std::endl;
        }
    }
}
