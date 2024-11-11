//! @file
//! @brief Helper class with a CSV file reader used to load the test data.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

#include "fmt/format.h"

namespace fgc4::utils::test
{
    template<size_t NumberValues>
    class ReadCSV
    {
      public:
        //! Constructs a ReadCSV object.
        //!
        //! @param path Path to the file to be read.
        //! @param separator Character that is expected to separate columns
        //! @throws std::runtime_error if the file cannot be opened for any reason
        ReadCSV(std::filesystem::path path, const char& separator = ',')
            : m_in_file(path),
              path(path),
              m_separator{separator}
        {
            // regex expression matching arbitrary collection of numbers starting with a '+' or '-' sign, followed up by
            // a digit, with an optional decimal point ('.'), and optional exponent (e) separated with user-defined
            // separators
            m_regex_expr = std::string("((\\+|-)?[0-9]*\\.?[0-9]+(e(\\+|-)?[0-9]+)?)") + "(?:" + "\\" + separator
                           + std::string("(\\+|-)?[0-9]*\\.?[0-9]+(e(\\+|-)?[0-9]+)?)*");

            if (!m_in_file.is_open())
            {
                throw std::runtime_error(fmt::format("Failed to open file: {}.", std::string(path)));
            }
        }

        //! Destructs a ReadCSV object, closes the open file.
        ~ReadCSV()
        {
            if (m_in_file.is_open())
            {
                m_in_file.close();
            }
        }

        //! Returns the flag whether the end of file has been reached.
        //!
        //! @return True if EOF reached, false otherwise
        bool eof() const noexcept
        {
            return m_in_file.eof();
        }

        //! Reads one line from the file, if the file has not reached its end, and splits it an array of output values.
        //!
        //! @return Array with read values
        std::optional<std::array<double, NumberValues>> readLine()
        {
            std::string line_str;
            if (!std::getline(m_in_file, line_str) || line_str.empty())
            {
                return std::nullopt;
            }
            // check if the line is a (possible) header
            if (m_first && possibleHeader(line_str))
            {
                // recursively ignore possible headers
                return readLine();
            }
            else
            {
                m_first = false;
            }
            processLine(line_str);
            return m_read_values;
        }


      private:
        std::ifstream m_in_file;          //!< File to be read
        const char    m_separator{','};   //!< Expected data separator
        //! Flag to signal start of reading the file, used for ignoring file header
        bool m_first{true};
        //! Regex expression to find numbers in the first couple characters of a line
        std::regex                       m_regex_expr;
        std::array<double, NumberValues> m_read_values{0.0};   //!< Values read from the file's most recent line


        //! Processes one provided line and fills the internal array.
        //!
        //! @param line_str String holding the entire line that was read from the file
        //! @throw std::runtime_error When the data in the line_str cannot be understood as double
        void processLine(std::string& line_str)
        {
            std::string       tmp_str;
            std::stringstream ss(line_str);
            for (size_t index = 0; index < NumberValues; index++)
            {
                if (!std::getline(ss, tmp_str, m_separator))
                {
                    throw std::runtime_error(
                        fmt::format("Insufficient number of values in the line. Expected {} values.", NumberValues)
                    );
                }
                try
                {
                    m_read_values[index] = stod(tmp_str);
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error("Invalid number format in CSV.");
                }
            }
        }

        //! Checks if the first two characters contain a number and whether the entire line contains the expected
        //! separator, as a rudimentary check whether the line might be a comment.
        //!
        //! @param line Line to be checked
        //! @return True if the line is a possible header, false otherwise
        bool possibleHeader(const std::string& line) const
        {
            // it is assumed to be a header if the first couple of characters are not a number
            return !std::regex_match(line, m_regex_expr);
        }
    };
}   // namespace fgc4::utils::test
