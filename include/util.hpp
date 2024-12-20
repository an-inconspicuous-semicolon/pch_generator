//
// Created by An Inconspicuous Semicolon on 18/12/2024.
//

#pragma once

#include <string>
#include <vector>

// file functions
std::string file_read(const std::string& path);
void file_write(const std::string& path, const std::string& contents);

// string functions
void string_replace(std::string& input, const std::string& from, const std::string& to);
std::vector<std::pair<std::string, size_t>> string_extract_substrings(const std::string& input, const std::string& start, const std::string& end);
void string_insert(std::string& input, const std::string& to_insert, size_t position);
std::vector<std::string> string_split(const std::string& input, char delimiter);