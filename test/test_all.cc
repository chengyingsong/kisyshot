#include <filesystem>
#include <iostream>
int main(){

    for (const auto& entry: std::filesystem::directory_iterator("cases/function_test2020")) {
        if (entry.path().extension() == ".S") {
            std::cout << "file: " << entry.path().string() << std::endl;
            std::string p = entry.path().string();
            std::string i = p.replace(p.find(".S"), 2, "");
            system(("gcc " + entry.path().string() + " libsysy.a -o " + p).c_str());

            int ret = system(("./" + p).c_str());
            std::cout << "result: " << ret;
            std::cout << std::endl << std::endl << std::endl;
        }
    }
}