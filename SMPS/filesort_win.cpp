#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <regex>
#include <io.h>

struct sample
{
    int number;
    std::string date;
    std::string time;
    std::string density;
    std::string diameter;
    sample *next;
    std::vector<std::string> data;
};

std::string clean_string(std::string mystring)
{
    mystring.erase( std::remove(mystring.begin(), mystring.end(), '\r'), mystring.end());
    mystring.erase( std::remove(mystring.begin(), mystring.end(), '\n'), mystring.end() );
    return mystring;
}

int convert_csv(std::string filename)
{
    std::ifstream fp(filename); //定义声明一个ifstream对象，指定文件路径
    std::string line;
    std::vector<std::string> line_aft;
    sample *head_of_list = NULL;
    sample *current_item = NULL;
    head_of_list = new sample;
    current_item = head_of_list;
    current_item->number = -1;
    current_item->next = NULL;
    int line_index = 1;
    int index = 1;
    int item_line_index = -1;
    int data_line_index = -1;
    while (getline(fp, line))
    { //循环读取每行数据
        std::string item;
        std::istringstream readstr(line);
        line_aft.clear();
        while (getline(readstr, item, ','))
        {
            line_aft.push_back(clean_string(item));
        }
        if (line_aft[0] == "Sample #")
        {
            if (current_item->number == -1)
            {
                current_item->number = atoi(line_aft[1].c_str());
            }
            else
            {
                current_item->next = new sample;
                current_item = current_item->next;
                current_item->number = atoi(line_aft[1].c_str());
                current_item->next = NULL;
            }
            data_line_index = -1;
            item_line_index = 0;
        }
        switch (item_line_index)
        {
        case 1:
            current_item->date = line_aft[1];
            break;
        case 2:
            current_item->time = line_aft[1];
            break;
        case 4:
            current_item->density = line_aft[1];
            break;
        case 5:
            current_item->diameter = line_aft[1];
            break;
        default:
            break;
        }
        // std::cout << "point 1" << std::endl;
        if (line_aft.size() >= 1 && line_aft[0] == "Time Elapsed (s)")
        {
            data_line_index = 0;
        }
        else if (data_line_index > -1 && line_aft.size() > 1)
        {
            current_item->data.push_back(line_aft[1]);
            // std::cout << line_aft[1] << std::endl;
            data_line_index += 1;
        }
        // std::cout << "point 2" << std::endl;
        if (item_line_index > -1)
            item_line_index += 1;
        line_index += 1;
    }

    // output
    std::ofstream in;
    in.open("result\\formated_"+filename, std::ios::trunc); // ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
    int i;
    std::string temp_line;
    for (int i = 0; i < 5; i++)
    {
        temp_line = "";
        current_item = head_of_list;
        while (current_item != NULL)
        {
            switch (i)
            {
            case 0:
                temp_line += current_item->diameter + ",";
                break;
            case 1:
                temp_line += current_item->date + ",";
                break;
            case 2:
                temp_line += current_item->time + ",";
                break;
            case 3:
                temp_line += current_item->density + ",";
            default:
                break;
            }
            current_item = current_item->next;
        }
        in << temp_line << "\n";
    }
    for (int i = 0; i < 60; i++)
    {
        temp_line = "";
        current_item = head_of_list;
        while (current_item != NULL)
        {
            if (current_item->data.size() > i)
            {
                temp_line += current_item->data[i] + ",";
            }
            else
            {
                temp_line += ",";
            }
            current_item = current_item->next;
        }
        in << temp_line << "\n";
    }
    in.close();
    return 0;
}

int main()
{  
    struct _finddata_t c_file;
    intptr_t hFile;
    std::string finder_path = "*.csv";
    if( (hFile = _findfirst( finder_path.c_str(), &c_file )) == -1L )
        printf( "No *.csv files in current directory!\n" );
    else
    {
        std::cout << "Press Anykey to Continue/" << std::endl;
        std::cin.get();
        do
        {
            std::string temp(c_file.name);
            // output the files located in current folder
            printf( " %-12s %9ld has been converted\n", c_file.name, c_file.size);
            convert_csv(temp);
        } while( _findnext( hFile, &c_file ) == 0 );
        _findclose( hFile );
    }
    
    return 0;
}