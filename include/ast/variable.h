#pragma once

namespace kisyshot::ast {

    class TempVar{
        //临时变量定义，有值和ID
        int value;
        int nameID;
        TempVar(int nameID,int value);
    };


    class SymbolTable{
        //符号表定义
    public:
        //符号表偏移
        int offset;
        //符号表基址
        int base;
        //符号类型
        int type;
        //符号占用字节数
        int width;
        //元素类型
        int element_type;
        //数组纬度
        int dimenson;
        //符号值 
        int value;
        SymbolTable();
    };
}