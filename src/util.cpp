//
// Created by An Inconspicuous Semicolon on 18/12/2024.
//

#include "util.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

std::string file_read(const std::string& path)
{
    std::ifstream file_handle(path, std::fstream::ate);
    if (!file_handle)
    {
        std::cerr << "Failed to read from \"" << path << "\"" << std::endl;
        std::exit(-2);
    }

    unsigned int file_size = file_handle.tellg();
    file_handle.seekg(0);

    std::string contents;
    contents.resize(file_size);

    file_handle.read(&contents[0], file_size);
    return contents;
}

void file_write(const std::string& path, const std::string& contents)
{
    std::ofstream file_handle(path);

    if (!file_handle)
    {
        std::cerr << "Failed to write to \"" << path << "\"" << std::endl;
        std::exit(-3);
    }

    file_handle.write(contents.data(), contents.size());
}

void string_replace(std::string& input, const std::string& from, const std::string& to)
{
    if (from.empty())
    {
        return;
    }

    size_t start_pos = 0;
    while ((start_pos = input.find(from, start_pos)) != std::string::npos)
    {
        input.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    while (!input.empty() && input.back() == '\0')
    {
        input.pop_back();
    }
}

std::vector<std::pair<std::string, size_t>> string_extract_substrings(const std::string& input, const std::string& start, const std::string& end)
{
    std::vector<std::pair<std::string, size_t>> substrings;
    size_t start_pos = 0;

    while ((start_pos = input.find(start, start_pos)) != std::string::npos)
    {
        size_t end_pos = start_pos;

        while (end_pos < input.size())
        {
            // Find the end string
            size_t potential_end_pos = input.find(end, end_pos);

            // Check if it is escaped
            if (potential_end_pos != std::string::npos &&
                (potential_end_pos == 0 || input[potential_end_pos - 1] != '\\'))
            {
                // Found a valid unescaped end string
                substrings.emplace_back(input.substr(start_pos, potential_end_pos - start_pos), start_pos);
                start_pos = potential_end_pos + end.size(); // Move past the end delimiter
                break;
            }
            else if (potential_end_pos != std::string::npos)
            {
                // Skip the escaped end delimiter
                end_pos = potential_end_pos + end.size();
            }
            else
            {
                // No more valid end delimiters found
                start_pos = input.size();
                break;
            }
        }
    }

    return substrings;
}

void string_insert(std::string& input, const std::string& to_insert, size_t position)
{
    if (position > input.size())
    {
        std::cerr << "Failed to insert a string as the insert position was outside the bounds of the string" << std::endl;
        std::exit(-4);
    }

    input.insert(position, to_insert);
}

std::vector<std::string> string_split(const std::string& input, char delimiter)
{
    std::vector<std::string> result;
    std::string token;
    std::stringstream ss(input);

    while (std::getline(ss, token, delimiter))
    {
        result.push_back(token);
    }

    if (result.size() > 1) result.pop_back();

    return result;
}