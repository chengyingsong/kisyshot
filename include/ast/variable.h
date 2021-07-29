#pragma once
#include<string>
//感觉必要性不大
namespace kisyshot::ast {

    class TempVar {
    public:
        //临时变量定义，有值和ID
        int value;
        std::string name;

        TempVar(std::string &name,int value){
            this->name = name;
            this->value = value;
        }
        TempVar(std::string &name){
            this->name = name;
        }
        TempVar();
    };
}