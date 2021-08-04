#include <filesystem>
#include <iostream>
#include <fstream>
std::string expected (const std::string& path){
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
int main(){

    for (const auto& entry: std::filesystem::directory_iterator("cases/function_test2020")) {
        if (entry.path().extension() == ".S") {
            std::cout << "file: " << entry.path().string() << std::endl;
            std::string p = entry.path().string();
            p.replace(p.find(".S"), 2, "");
            std::string out = p + ".out";

            system(("gcc " + entry.path().string() + " libsysy.a -o " + p).c_str());

            int ret = system(("./" + p).c_str());
            std::cout << "result: " << ret << ", and expected: " << expected(out);
            std::cout << std::endl << std::endl << std::endl;
        }
    }
}