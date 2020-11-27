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
};

/**
 * Function used to build file's alphabet
 *
 * @param  content contents of input file
 * @return newly built alphabet that contains all
 * 		   symbols found in content and their frequencies	
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

string buildHuffmanTree(Alphabet *alphabet, const vector<unsigned char>& originalText) {
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

	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	string encodedString = "";
	
	for (auto it = originalText.begin(); it != originalText.end(); it++) {
		encodedString += huffmanCode[*it];
	}

	return encodedString;
}

void writeCodeToFile(const std::string& path, std::string& newCodes) {
	std::ofstream outFile(path, std::ios::out | std::ios::binary);
	
	for (unsigned int i = 0; i < newCodes.length(); i += 8) {
		unsigned char byte = 0;
		string str8 = "";
		
		if (i + 8 < newCodes.length()) {
			str8 = newCodes.substr(i, i + 8);
		} else {
			str8 = newCodes.substr(i, newCodes.length());
		}
		
		for (unsigned b = 0; b != 8; b++) {
			if (b < str8.length()) {
				byte |= (str8[b] & 1) << b;
			} else {
				byte |= 1 << b;
			}
		}
		
		outFile.put(byte);
	}
	
	outFile.close();
}

void compress(const std::vector<unsigned char>& content, const string& output_file) {
	vector<unsigned char> myContent = content;
	Alphabet *alphabet = getAlphabet(content);
	
	string newCodes = buildHuffmanTree(alphabet, myContent); // <-- modify this later
	
	writeCodeToFile(output_file, newCodes);
	
	return;
}

void readFromFile(const std::string path) {
	std::ifstream inFile(path, std::ios::in | std::ios::binary);
	
	
}

void decompress(const std::string& content) {
    // TODO
    
}