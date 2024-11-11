//! @file
//! @brief Helper class with a CSV file reader used to load the test data.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <string>

#include "fmt/format.h"

namespace fgc4::utils::test
{
    template<size_t NumberValues>
    class ReadCSV
    {
      public:
        //! ReadCSV constructor
        //!
        //! @param path Path to the file to be read.
        //! @throws std::runtime_error if the file cannot be opened for any reason
        ReadCSV(std::filesystem::path path, const char& separator = ',')
            : m_in_file(path),
              m_separator{separator}
        {
            if (!m_in_file.is_open())
            {
                throw std::runtime_error(fmt::format("Failed to open file: {}.", std::string(path)));
            }
        }

        //! ReadCSV destructor.
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
            processLine(line_str);
            return m_read_values;
        }


      private:
        std::ifstream                    m_in_file;
        const char                       m_separator{','};
        std::array<double, NumberValues> m_read_values{0.0};

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
    };
}   // namespace fgc4::utils::test
