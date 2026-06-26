#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<queue>

struct Node {
    char data; // to store the byte value
    unsigned freq; // to store the frequency of the byte 
    Node *left , * right; // pointers to left and right child in huffman tree
    // constructor to initialize the node 
    Node (char data , unsigned freq) {
        left = right = nullptr;
        this->data = data;
        this->freq = freq;
    }
};

// need a custom comparator for pq 
struct compare {
    bool operator()(Node *l , Node *r) {
        return (l->freq  > r->freq);
    }
};

void generateCodes(struct Node* root, std::string str , std::vector<std::string>& huffmanCode) {
    if(!root )return; // null
    // leaf node in huffman tree
    if(!root->left and !root->right){
        huffmanCode[static_cast<unsigned char>(root->data)] = str;
    }
    // traverse left 
    // traverse right
    // basically dfs 
    generateCodes(root->left,str+'0',huffmanCode); // append '0' if going left 
    generateCodes(root->right,str+'1',huffmanCode); // append '1' if going right
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main(){
    // opening the target file 
    std::ifstream inFile("input.txt",std::ios::binary);
    if(!inFile) {
        std::cerr << "Cannot open the file>.\n";
        return 1;
    }
    // frequency map to count occurences of every byte 
    unsigned int freqMap[256] ={0};
     
    char ch;
    //reads one character at a time from the file and stores it in char variable ch. The loop continues until the end of the file is reached.
    // it reads raw bytes be it spaces or newlines
    while(inFile.get(ch)) {
       freqMap[static_cast<unsigned char>(ch)]++;
    }
    inFile.close();
    // time to initialize the min - heap ( priority queue )
    std::priority_queue<Node*,std::vector<Node*>,compare> minHeap;

    // pushing all the character to map 
    for(int i = 0;i<256;i++) {
        if(freqMap[i] > 0) {
            minHeap.push(new Node(static_cast<char>(i),freqMap[i]));
        }
    }
    // TIME to make HUFFMAN TREE 
    if(minHeap.size() == 1) {
        Node* singleNode = minHeap.top();
        minHeap.pop();
        Node* dummyRoot = new Node('$', singleNode->freq);
        dummyRoot->left = singleNode;
        minHeap.push(dummyRoot);
    }
    while(minHeap.size()!=1) {
        // pop two most rarest node (characters which appears very less)
        Node *left = minHeap.top();
        minHeap.pop();
        Node *right = minHeap.top();
        minHeap.pop();
        // now creating a internal node which will have sum of freq of both left and right node
        Node *top = new Node('$' ,left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
     /*
     each time I popped two nodes and added one one 
     so at the end i will have one node left which is root node of 
     Huffman tree , whose freq is equal to total freq of characters in file 
     */  
     
    Node * root = minHeap.top();
    // now I will map every char to a unique binary representation string
    std::vector<std::string> huffmanCode(256, "");
    generateCodes(root,"",huffmanCode); 
    // now opening the file again 
    std::ifstream inFile2("input.txt",std::ios::binary);
    std::ofstream outFile("compressed.bin",std::ios::binary);
    if(!inFile2 || !outFile) {
        std::cerr <<"file streams failed.\n";
        return 1;
    }
    unsigned char memoryblock = 0; // 8-bit memory block 
    int bitcount = 0;
    char ch2 ;
    while (inFile2.get(ch2)) {
        std::string code = huffmanCode[static_cast<unsigned char>(ch2)];
        for(char bit : code) {
            memoryblock <<= 1; // left shifting to make more space
            if(bit=='1') {
                memoryblock |= 1;
            }
            bitcount++;
            if(bitcount==8) {
                outFile.write(reinterpret_cast<const char*>(&memoryblock),1);
                memoryblock = 0;
                bitcount = 0;
            }
        }
    }
    if(bitcount>0) {
        memoryblock = memoryblock << (8-bitcount);
        outFile.write(reinterpret_cast<const char*>(&memoryblock),1);
    }
    inFile2.close();
    outFile.close();
    // DECOMPRESSING THE BINARY FILE 
    std::ifstream inBin("compressed.bin" , std::ios::binary);
    std::ofstream outText("decompressed.txt",std::ios::binary);
    if(!inBin || !outText) {
        std::cerr << "Cannot open files for decompression\n";
        return 1;
    }
    Node* cur = root;
    char byteRead;
    int decodedChars = 0;
    int totalChar = root->freq;
    while((inBin.get(byteRead)) and decodedChars < totalChar) {
        for(int i = 7;i>=0;i--) {
            int bit  = (byteRead >> i) & 1;
            if(bit==0) {
                cur  = cur->left;
            }else {
                cur  = cur->right;
            }
            // leaf node 
            if(!cur->left and !cur->right) {
                outText.put(cur->data);
                decodedChars++;
                cur = root;
                if(decodedChars==totalChar) break;
            }
        }
    }
    inBin.close();
    outText.close();
    freeTree(root);

}