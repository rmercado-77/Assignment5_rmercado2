#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <exception>
#include "pixel.h"

// Calculate and display the average of each color.
void average_colors(std::vector<Pixel> &pixel_list)
{
    // Avoid dividing by zero.
    if (pixel_list.empty())
    {
        std::cout << "No pixels to average.\n";
        return;
    }

    double total_r = 0.0;
    double total_g = 0.0;
    double total_b = 0.0;

    // Read each pixel without copying or changing it.
    for (const Pixel &p : pixel_list)
    {
        total_r += p.r;
        total_g += p.g;
        total_b += p.b;
    }

    // Divide each color total by the number of pixels.
    std::cout << "Average red: "
              << total_r / pixel_list.size() << '\n';

    std::cout << "Average green: "
              << total_g / pixel_list.size() << '\n';

    std::cout << "Average blue: "
              << total_b / pixel_list.size() << '\n';
}

// Flip each pixel across the image's vertical range.
void flip_vertically(std::vector<Pixel> &pixel_list)
{
    // The image has 256 rows, numbered 0 through 255.
    for (Pixel &p : pixel_list)
    {
        p.y = 255 - p.y;
    }
}

// Order pixels by x first, then y.
bool coordinate_order(const Pixel &a, const Pixel &b)
{
    if (a.x != b.x)
    {
        return a.x < b.x;
    }

    return a.y < b.y;
}

int main(int argc, char* argv[])
{
    // Check that an input filename was provided.
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file>\n";
        return 1;
    }

    std::string filename = argv[1];

    // Open the input file.
    std::ifstream input_file(filename);

    if (!input_file.is_open())
    {
        std::cerr << "Error: Could not open " << filename << '\n';
        return 1;
    }

    // Store all pixels read from the file.
    std::vector<Pixel> pixel_list;
    std::string line;
    int line_number = 0;

    // getline() reads one full line because each line contains one pixel.
    while (std::getline(input_file, line))
    {
        line_number++;

        // find_first_not_of() checks for text beyond whitespace.
        if (line.find_first_not_of(" \t\r") == std::string::npos)
        {
            continue;
        }

        Pixel p;
        std::string values[5];
        std::size_t start = 0;

        // Use find() and substr() to split each line at its four commas. This separates x, y, r, g, and b without another parser.
        for (int i = 0; i < 4; i++)
        {
            std::size_t comma = line.find(',', start);

            // npos means the required comma was not found.
            if (comma == std::string::npos)
            {
                std::cerr << "Error: Missing comma on line "
                          << line_number << '\n';
                return 1;
            }

            values[i] = line.substr(start, comma - start);
            start = comma + 1;
        }

        // Extract the final value after the last comma.
        values[4] = line.substr(start);

        // Reject records with more than five fields.
        if (values[4].find(',') != std::string::npos)
        {
            std::cerr << "Error: Extra field on line "
                      << line_number << '\n';
            return 1;
        }

        try
        {
            // The fields are text, so stoi() converts coordinates to integers and stof() converts RGB values to floats.
            p.x = std::stoi(values[0]);
            p.y = std::stoi(values[1]);
            p.r = std::stof(values[2]);
            p.g = std::stof(values[3]);
            p.b = std::stof(values[4]);
        }
        catch (const std::exception &)
        {
            // Catch conversion errors and report the problem line.
            std::cerr << "Error: Invalid number on line "
                      << line_number << '\n';
            return 1;
        }

        // Check the coordinate and color ranges.
        if (p.x < 0 || p.x >= 512 ||
            p.y < 0 || p.y >= 256 ||
            !(p.r >= 0.0f && p.r <= 1.0f) ||
            !(p.g >= 0.0f && p.g <= 1.0f) ||
            !(p.b >= 0.0f && p.b <= 1.0f))
        {
            std::cerr << "Error: Value out of range on line "
                      << line_number << '\n';
            return 1;
        }

        // push_back() adds each completed pixel to the end of the vector.
        pixel_list.push_back(p);
    }

    // bad() checks for a serious input read error.
    if (input_file.bad())
    {
        std::cerr << "Error: Failed while reading the input file.\n";
        return 1;
    }

    input_file.close();

    std::cout << "Pixels loaded: " << pixel_list.size() << '\n';

    if (pixel_list.empty())
    {
        std::cerr << "Error: No pixels were loaded.\n";
        return 1;
    }

    // Display averages before flipping the image.
    average_colors(pixel_list);

    // Change y coordinates while keeping x and RGB unchanged.
    flip_vertically(pixel_list);

    // sort() uses coordinate_order() to compare x first, then y. This restores coordinate order to match the output example.
    std::sort(pixel_list.begin(), pixel_list.end(), coordinate_order);

    // Open the output file.
    std::ofstream output_file("flipped.dat");

    if (!output_file.is_open())
    {
        std::cerr << "Error: Could not create flipped.dat.\n";
        return 1;
    }

    // setprecision() controls how many digits are written. max_digits10 preserves the stored float values when read back.
    output_file << std::setprecision(
        std::numeric_limits<float>::max_digits10);

    // Save each pixel in the original comma-separated format.
    for (const Pixel &p : pixel_list)
    {
        output_file << p.x << ','
                    << p.y << ','
                    << p.r << ','
                    << p.g << ','
                    << p.b << '\n';
    }

    output_file.close();

    // Check that writing and closing the file succeeded.
    if (!output_file)
    {
        std::cerr << "Error: Failed while writing flipped.dat.\n";
        return 1;
    }

    std::cout << "Flipped image saved to flipped.dat.\n";

    return 0;
}
