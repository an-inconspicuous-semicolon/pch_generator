//
// Created by An Inconspicuous Semicolon on 18/12/2024.
//

#include <filesystem>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>
#include <Windows.h>

#include "ThreadPool.hpp"
#include "util.hpp"

void prepare_file(const std::string& raw_source_path, const std::string& working_file_path)
{
    std::string contents = file_read(raw_source_path);
    string_replace(contents, "#include <", "@@@include <");
    string_replace(contents, "#include \"", "// #include \"");

    auto defines = string_extract_substrings(contents, "#define", "\n");

    unsigned int offset = 0;
    for (const auto& [define, pos] : defines)
    {
        std::string temp = define;
        string_replace(temp, "#define", "@@@define");
        string_insert(contents, temp + '\n', pos + offset);
        offset += temp.size() + 1;
    }

    file_write(working_file_path, contents);
}

void process_file(
    const std::string& working_file_path,
    const std::string& processed_file_path,
    const std::vector<std::string>& defines,
    const std::string& clang_path
)
{
    STARTUPINFOA startup_info{};
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION process_info{};

    std::ostringstream command_line;
    command_line << clang_path << " " << working_file_path << " -E -o " << processed_file_path;

    for (const auto& define : defines)
        command_line << " -D" << define;

    if (!CreateProcessA(nullptr, const_cast<LPSTR>(command_line.str().c_str()), nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info))
    {
        std::cerr << "Failed to run clang with error " << GetLastError() << std::endl;
        std::exit(-5);
    }

    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
}

std::string post_process_file(const std::string& processed_file)
{
    std::string contents = file_read(processed_file);
    string_replace(contents, "@@@", "#");
    return contents;
}

void generate_output(const std::set<std::string>& includes, const std::string& output_file)
{
    std::ostringstream output;
    output << "// AUTO GENERATED. DO NOT EDIT!\n\n#pragma once\n\n";

    for (const auto& include : includes)
    {
        output << include << '\n';
    }

    file_write(output_file, output.str());
}

/**
 * Arguments: <WorkingDirectory> <OutputFileName> <SourcesList> <DefinesList> <ClangPath>
 */
int main(int argc, char** argv)
{
    if (argc != 6)
    {
        std::cerr << "Usage: pch_generator.exe <WorkingDirectory> <OutputFileName> <SourcesList> <DefinesList> <ClangPath>" << std::endl;
        std::exit(-1);
    }

    std::string working_directory = argv[1];
    std::string output_file       = argv[2];
    std::string sources_file      = argv[3];
    std::string defines_file      = argv[4];
    std::string clang_path        = argv[5];

    std::string sources_file_contents = file_read(sources_file);
    std::vector<std::string> sources  = string_split(sources_file_contents, '\n');

    std::string defines_file_contents = file_read(defines_file);
    std::vector<std::string> defines  = string_split(defines_file_contents, '\n');

    std::mutex unique_includes_mutex;
    std::set<std::string> unique_includes;

    unsigned int num_workers = max(0, std::thread::hardware_concurrency() - 2); {
        ThreadPool pool(num_workers);

        unsigned int file_id = 0;
        for (const auto& source : sources)
        {
            auto work = [id = file_id++, &working_directory, source, &defines, &unique_includes_mutex, &unique_includes, &clang_path]
            {
                std::ostringstream working_file_stream;
                working_file_stream << working_directory << "\\" << id << ".hpp";
                std::string working_file = working_file_stream.str();
                prepare_file(source, working_file);

                std::ostringstream processed_file_stream;
                processed_file_stream << working_directory << "\\" << id << ".i";
                std::string processed_file = processed_file_stream.str();
                process_file(working_file, processed_file, defines, clang_path);

                std::ostringstream post_processed_file_stream;
                post_processed_file_stream << working_directory << "\\" << id << ".pch.hpp";
                std::string post_processed_file    = post_processed_file_stream.str();
                std::string post_processed_content = post_process_file(processed_file);

                auto matches = string_extract_substrings(post_processed_content, "#include", "\n");
                //
                {
                    std::lock_guard guard(unique_includes_mutex);
                    for (const auto& match : matches | std::views::keys)
                    {
                        unique_includes.insert(match);
                    }
                }
            };

            pool.enqueue_task(work);
        }
    }

    generate_output(unique_includes, output_file);

    return 0;
}
