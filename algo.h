#ifndef ALGO_H
#define ALGO_H

#include <vector>

/**
 * Compresses the data it receives.
 *
 * @content: The original content of a file in bytes
 * @returns: The content of the compressed file
 */
void compress(const std::vector<unsigned char>& content, const std::string& output_file);

/**
 * Decompresses the data it receives.
 *
 * @content: The compressed file in bytes
 * @returns: The content of the original file
 */
void decompress(const std::string& input_file, const std::string& output_file);

// You may add your own (de)compression function, with more parameters.

#endif