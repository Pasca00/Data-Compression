#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <fstream>

#include "algo.h"

using namespace std;

class Element {
	private:
		unsigned char symbol;
		unsigned int frequency;
		
	public:
		Element(unsigned char symbol) {
			this->symbol = symbol;
			this->frequency = 1;
		}
	
		Element(unsigned char symbol, unsigned int frequency) {
			this->symbol = symbol;
			this->frequency = frequency;
		}
		
		void updateFrequeny() {
			frequency++;
		}
		
		unsigned char getSymbol() {
			return symbol;
		}
		
		unsigned int getFrequency() {
			return frequency;
		}
};

class Alphabet {
	private:
		vector<Element*> *symbols;
		unsigned int size;
		
	public:
		Alphabet() {
			symbols = new vector<Element*>();
			size = 0;
		}
		
		bool contains(unsigned char key) {
			for (vector<Element*>::iterator it = symbols->begin(); it != symbols->end(); it++) {
				if((*it)->getSymbol() == key) {
					return true;
				}
			}
			
			return false;
		}
		
		void addSymbol(unsigned char symbol) {
			Element *newSymbol = new Element(symbol);
			symbols->push_back(newSymbol);
			
			size++;
		}
		
		void addSymbolAndFreq(unsigned char symbol, unsigned int frequency) {
			Element *newSymbol = new Element(symbol, frequency);
			symbols->push_back(newSymbol);
			
			size++;
		}
		
		void updateSymbol(unsigned char key) {
			for (vector<Element*>::iterator it = symbols->begin(); it != symbols->end(); it++) {
				if((*it)->getSymbol() == key) {
					(*it)->updateFrequeny();
					return;
				}
			}
		}
		
		vector<Element*> * getSymbols() {
			return symbols;
		}
		
		unsigned int getSize() {
			return size;
		}
};

/**
 * Function used to build uncompressed file's alphabet
 *
 * @param  content all bytes from input file
 * @return newly built alphabet that contains all
 * 		   unique symbols found in content and their frequencies	
 */
Alphabet *getAlphabet(const vector<unsigned char>& content) {
	vector<unsigned char> myContent = content;
	Alphabet *newAlphabet = new Alphabet();
	
	for (std::vector<unsigned char>::iterator it = myContent.begin(); it != myContent.end(); it++) {
		if (newAlphabet->contains(*it)) {
			newAlphabet->updateSymbol(*it);
		} else {
			newAlphabet->addSymbol(*it);
		}
	}
	
	return newAlphabet;
}

typedef struct minHeapNode {
	char symbol;
	unsigned int frequency;
	
	struct minHeapNode *l;	
	struct minHeapNode *r;	
} minHeapNode;

typedef struct minHeap{
	unsigned int size;
	unsigned int capacity;
	
	minHeapNode **array;
} minHeap;

minHeapNode *newNode(char symbol, unsigned int frequency, minHeapNode *l, minHeapNode* r) {
	minHeapNode *newNode = 	new minHeapNode();
	
	newNode->symbol = symbol;
	newNode->frequency = frequency;

	newNode->l = l;
	newNode->r = r;
	
	return newNode;
}

void encode(minHeapNode *root, string code, unordered_map<char, string> &huffmanCodes) {
	if (root == NULL) {
		return;
	}
	
	if (!root->l && !root->r) {
		huffmanCodes[root->symbol] = code;
	}
	
	encode(root->l, code + "0", huffmanCodes);
	encode(root->r, code + "1", huffmanCodes);
}

struct comp {
	bool operator()(minHeapNode* l, minHeapNode* r) {
		return l->frequency > r->frequency;
	}
};

minHeapNode* buildHuffmanTree(Alphabet *alphabet) {
	priority_queue<minHeapNode*, vector<minHeapNode*>, comp> pq;

	for (std::vector<Element*>::iterator it = (*alphabet->getSymbols()).begin(); it != (*alphabet->getSymbols()).end(); it++) {
		minHeapNode* addToQueue = newNode((*it)->getSymbol(), (*it)->getFrequency(), NULL, NULL);
		pq.push(addToQueue);
	}

	while (pq.size() != 1) {
		minHeapNode *left = pq.top(); pq.pop();
		minHeapNode *right = pq.top();	pq.pop();

		int sum = left->frequency + right->frequency;
		pq.push(newNode('\0', sum, left, right));
	}

	minHeapNode* root = pq.top();

	return root;
}

string getEncodedText(minHeapNode *root, const vector<unsigned char>& originalText) {
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	string encodedString = "";
	
	for (auto it = originalText.begin(); it != originalText.end(); it++) {
		encodedString += huffmanCode[*it];
	}

	return encodedString;
}

string decode(minHeapNode* root, std::string encodedString) {
    string originalText = "";
	
	minHeapNode *curr = root;
	
	for (unsigned int i = 0; i <= encodedString.size(); i++) {
		if (encodedString[i] == '0') {
			curr = curr->l;
		} else {
			curr = curr->r;
		}
		
		if (curr->l == NULL && curr->r == NULL) {
			originalText += curr->symbol;
			curr = root;
		}
	}
	
	return originalText;
}

string buildTreeAndDecode(Alphabet *alphabet, const std::string encodedString) {
	priority_queue<minHeapNode*, vector<minHeapNode*>, comp> pq;

	for (std::vector<Element*>::iterator it = (*alphabet->getSymbols()).begin(); it != (*alphabet->getSymbols()).end(); it++) {
		minHeapNode* addToQueue = newNode((*it)->getSymbol(), (*it)->getFrequency(), NULL, NULL);
		pq.push(addToQueue);
	}

	while (pq.size() != 1) {
		minHeapNode *left = pq.top(); pq.pop();
		minHeapNode *right = pq.top();	pq.pop();

		int sum = left->frequency + right->frequency;
		pq.push(newNode('\0', sum, left, right));
	}

	minHeapNode* root = pq.top();
	
	return decode(root, encodedString);
}

void writeCodeToFile(const std::string& path, std::string& newCodes, Alphabet *alphabet) {
	std::ofstream outFile(path, std::ios::out | std::ios::binary);
	
	// Write orignal number of symbols to compressed
	// file
	unsigned int alphabetSize = alphabet->getSize();
	outFile.write((char*)(&alphabetSize), sizeof(unsigned int));
	
	// Write all symbols from original file and 
	// their respective frequencies to the compressed
	// file so original Huffman tree can be rebuilt
	for(auto it = (*alphabet->getSymbols()).begin(); it != (*alphabet->getSymbols()).end(); it++) {
		unsigned char symbol = (*it)->getSymbol();
		unsigned int frequency = (*it)->getFrequency();
		
		outFile.write((char*)(&symbol), sizeof(unsigned char));
		outFile.write((char*)(&frequency), sizeof(unsigned int));
	}
	
	// Write the encoded string's length so that 
	// decompression knows when to stop
	unsigned long codeLength = newCodes.length();
	outFile.write((char*)(&codeLength), sizeof(unsigned long));
	
	// Write new codes
	unsigned char byte = 0;
	int bitCount = 0;
	for (unsigned int i = 0; i < newCodes.length(); i++) {
		if (bitCount == 8 || i == newCodes.length() - 1) {
			outFile.write((char*)(&byte), sizeof(unsigned char));
			byte = 0;
			bitCount = 0;
		}
		
		byte |= (newCodes[i] == '1') << bitCount;
		bitCount++;
	}
	
	outFile.close();
}

void compress(const std::vector<unsigned char>& content, const string& output_file) {
	vector<unsigned char> myContent = content;
	Alphabet *alphabet = getAlphabet(content);
	
	minHeapNode* root = buildHuffmanTree(alphabet);
	string newCodes = getEncodedText(root, myContent);
	
	writeCodeToFile(output_file, newCodes, alphabet);
}

string readFromFile(const std::string path) {
	std::ifstream inFile(path, std::ios::in | std::ios::binary);
	
	// Number of unique characters in original file
	unsigned int alphabetSize = 0;
	inFile.read((char*)(&alphabetSize), sizeof(unsigned int));
	
	Alphabet *originalAlphabet = new Alphabet();

	// Read all characters and their respective
	// frequencies and build the original file's
	// alphabet
	for (unsigned int i = 0; i < alphabetSize; i++) {
		unsigned char symbol;
		unsigned int frequency;
		
		inFile.read((char*)(&symbol), sizeof(unsigned char));
		inFile.read((char*)(&frequency), sizeof(unsigned int));
		
		originalAlphabet->addSymbolAndFreq(symbol, frequency);
	}

	// Lenght of compressed file in bits so
	// that leftover bits are ignored
	unsigned long compressedCodeLength;
	inFile.read((char*)(&compressedCodeLength), sizeof(unsigned long));
	
	// Read each byte from file and create
	// the bit string that will be used
	// to traverse the tree and decompress
	// the file
	string codes = "";
	unsigned long codeLength = 0;
	bool done = false;
	while (!inFile.eof() && !done) {
		unsigned char byte;
		inFile.read((char*)(&byte), sizeof(unsigned char));
		
		for (unsigned int i = 0; i < 8; i++) {
			if (byte & (1 << i)) {
				codes += '1';
			} else {
				codes += '0';
			}
			
			codeLength++;
			
			if (codeLength == compressedCodeLength) {
				done = true;
				break;
			}
		}
	}
	
	inFile.close();
	string originalText = buildTreeAndDecode(originalAlphabet, codes);

	return originalText;
}

void decompress(const std::string& input_file, const std::string& output_file) {
    string originalText = readFromFile(input_file);
	
	ofstream decompressedFile(output_file, std::ios::out | std::ios::binary);
	
	for (unsigned char const& c : originalText) {
		decompressedFile.write((char*)(&c), sizeof(unsigned char));
	}
	
	decompressedFile.close();
}