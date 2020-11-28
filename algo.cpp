#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <fstream>

#include "algo.h"

using namespace std;

void writeCodesToFile(const std::string& output_file, const std::vector<unsigned int>& newCodes) {
	ofstream outputFile(output_file, std::ios::out | std::ios::binary);
	
	for (unsigned int i = 0; i < newCodes.size(); i++) {
		outputFile.write((char*)(&(newCodes[i])), sizeof(unsigned int));
	}
	
	outputFile.close();
}

std::vector<unsigned int> readCodesFromFile(const std::string& path) {
	ifstream inputFile(path, std::ios::in | std::ios::binary);
	std::vector<unsigned int> codes;
	
	unsigned int number;
	
	while (!inputFile.eof()) {
		inputFile.read((char *)&number, sizeof(unsigned int));
		codes.push_back(number);
	}
	
	codes.pop_back();
	
	return codes;
}

void compress(const std::vector<unsigned char>& content, const std::string& output_file){ 
    unordered_map<string, unsigned int> table; 
    for (unsigned int i = 0; i < 256; i++) { 
        string ch = ""; 
        ch += char(i); 
        table[ch] = i; 
    } 
  
    string prefix = "";
	string c = ""; 
    prefix += content.front(); 
    unsigned int codeIter = 256; 
    vector<unsigned int> output_code; 
	
	for(unsigned int i = 0; i < content.size(); i++) {
		if(i < content.size() - 1) {
			c += content[i + 1];
		}
		
		if(table.find(prefix + c) != table.end()) {
			prefix += c;
		} else {
			output_code.push_back(table[prefix]);
			
			table[prefix + c] = codeIter;
			codeIter++;
			
			prefix = c;
		}
		
		c = "";
	}
	
    output_code.push_back(table[prefix]);
	
	writeCodesToFile(output_file, output_code);	
}


std::string decode(const std::string& path) {
	std::vector<unsigned int> codes = readCodesFromFile(path);
	
	std::string originalText;
	
    unordered_map<unsigned int, string> table; 
    for (unsigned int i = 0; i < 256; i++) { 
        string ch = ""; 
        ch += char(i); 
        table[i] = ch; 
    } 
	 
	unsigned int old = codes.front();
	unsigned int n; 
	
    string s = table[old]; 
	string c2 = ""; 
    c2 += s[0]; 
    originalText += s; 
    unsigned int count = 256;
	cout << codes.size() << endl;
    for (unsigned int i = 0; i < codes.size() - 1; i++) { 
        n = codes[i + 1]; 
		
        if (table.find(n) == table.end()) { 
            s = table[old]; 
            s = s + c2; 
        } else { 
            s = table[n]; 
        } 
		
        originalText += s; 
        c2 = ""; 
        c2 += s[0]; 
        table[count] = table[old] + c2; 
        count++; 
	
		
        old = n; 
    }
    
	return originalText;
}

void decompress(const std::string& input_file, const std::string& output_file){	
	string decompressedText = decode(input_file);
	
	ofstream file(output_file, std::ios::out);
	
	file << decompressedText;
	
	file.close();
}