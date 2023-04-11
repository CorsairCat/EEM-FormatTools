#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <regex>
#include <algorithm>

// This is mac capiable version of source code
#include <dirent.h>

struct data_point
{
    std::string time_stamp;
    std::string transmission;
};

bool compareByTime(const data_point &a, const data_point &b)
{
    return a.time_stamp < b.time_stamp;
}

// extract the time
int getCollectTime(data_point* data_, std::string filename)
{
    int temp_pos_1 = filename.rfind("-");
    int temp_pos_2 = filename.rfind(".");
    std::string temp_time_ms = filename.substr(temp_pos_1+1, temp_pos_2-temp_pos_1-1);
    // printf("time-ms: %s\n", temp_time_ms.c_str());
    temp_pos_2 = filename.rfind("_");
    data_->time_stamp = filename.substr(temp_pos_2+1, temp_pos_1-temp_pos_2-1);
    int pos;
    while ((pos = data_->time_stamp.find("-")) != std::string::npos) {
        data_->time_stamp.replace(pos, 1, ":");
    }
    data_->time_stamp = data_->time_stamp + "." + temp_time_ms;
    printf("time: %s\n", data_->time_stamp.c_str());
    return 1;
}

// collect each datapoint
data_point getDataPoint(std::string filename, double required_wavelength)
{   
    data_point result;
    result.transmission = "No_Data";
    getCollectTime(&result, filename);
    std::ifstream fp(filename); //read files
    std::string line;
    int data_flag = 0;
    int index_ = 0;
    double wavelength = 0.0;
    std::string temp_data;
    if (!fp.good())
    {
        printf("DataPoint Reading Error\n");
        return result;
    }
    while (getline(fp, line))
    {   //read data file by line
        std::string item;
        std::istringstream readstr(line);
        while ((data_flag == 0) && (getline(readstr, item, ' ')))
        {
            if (item == ">>>>>Begin")
            {
                data_flag = 1;
                break;
            }
        }
        index_ = 0;
        while ((data_flag == 2) && (getline(readstr, item, '\t')))
        {   
            switch (index_)
            {
            case 0:
                // printf("current: %s\n", item.c_str());
                wavelength = std::stod(item);
                break;
            case 1:
                temp_data = item;
                break;
            }
            
            index_ += 1;
        }
        if (data_flag == 1)
        {
            data_flag = 2;
        }
        else if ((data_flag == 2) && (wavelength >= required_wavelength))
        {
            result.transmission = temp_data;
            break;
        }
    }
    fp.close();
    return result;
}

int main()
{
    double test = 600.0;
    std::cout << "Input the wavelength you want to collect:" << std::endl;
    while (!(std::cin>>test) && (test>870))
    {
        std::cout << "Not available, Retry:" << std::endl;
    };
    printf("The target wavelength is %.2f;\n", test);
    std::string folder_name_part;
    std::cout << "Provide the data folder name:" << std::endl;
    std::cin >> folder_name_part;
    printf("The target wavelength is %.2f;\nThe target data folder is %s.\n", test, folder_name_part.c_str());
    data_point temp_data;
    std::vector<data_point> result_data;
    struct dirent *dirp;
    std::string filename;
    std::string folder_name = "./"+folder_name_part+"/";
    DIR* dir = opendir(folder_name.c_str());
    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_REG) {
            // printf("%s\n", dirp->d_name);
            filename = dirp->d_name;
            temp_data = getDataPoint(folder_name+filename, test);
            // printf("transmission: %s\n", temp_data.transmission.c_str());
            result_data.push_back(temp_data);
        }
    }
    closedir(dir);
    std::sort(result_data.begin(), result_data.end(), compareByTime);
    // write to csv
    // output
    std::ofstream in;
    std::string output_filename = folder_name_part + ".csv";
    in.open(output_filename.c_str(), std::ios::trunc); // ios::trunc clean file during open
    std::string temp_line;
    if (!in.good())
    {
        printf("Create output file failed \n");
        return 0;
    }
    for (size_t i = 0; i < result_data.size(); i++)
    {
        temp_line = result_data[i].time_stamp + ", " + result_data[i].transmission;
        // std::cout << temp_line << std::endl;
        in << temp_line << "\n";
    }
    in.close();   

    return 0;
}