#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <cmath>
using namespace std;

// At the top of your file, add:
/*
 * SOURCES CITED:
 * 
 * 1. ASCII to Binary Conversion
 *    cplusplus.com/reference/bitset/bitset/
 *    Used for converting text to binary using bitset<8>
 * 
 * 2. Dynamic 2D Arrays
 *    learncpp.com - Section 11.5 Multidimensional Arrays
 *    Used for creating grid[][] with new/delete
 * 
 * 3. QR Code Structure and History
 *    "I used to hate QR codes. But they're actually genius" - Veritasium
 *    https://www.youtube.com/watch?v=w5ebcowAJD8
 *    Used for understanding QR position squares, timing patterns, 
 *    alignment patterns, byte mode encoding (0100), and padding patterns
 *
 * 4. How QR Codes Generally Are Made
 *    "How QR Codes Are Built" - James Explains
 *    https://www.youtube.com/watch?v=142TGhaTMtI
 * 
 * 5. File Output
 *    cplusplus.com/reference/fstream/ofstream/
 *    Used for saving QR codes to .txt files
 */

// QRCode class - manages encoding/decoding with proper QR structure
class QRCode {
private:
    char* binaryData;      // Dynamically allocated array for binary
    int dataLength;        // Length of binary data
    string signature;      // Unique signature (our creative touch)
    int gridSize;          // Size of QR grid (21, 25, 29, etc.)
    string displayStyle;   // Display style for QR code
    
public:
    // Constructor
    QRCode() {
        binaryData = nullptr;
        dataLength = 0;
        signature = "1010"; // 4-bit unique signature
        gridSize = 25;      // Version 2 QR (25x25)
        displayStyle = "block"; // Default style
    }
    
    // Destructor - deallocate memory
    ~QRCode() {
        if (binaryData != nullptr) {
            delete[] binaryData;
            binaryData = nullptr;
        }
    }
    
    // Set display style
    void setDisplayStyle(const string& style) {
        displayStyle = style;
    }
    
    // Encode text to QR with proper structure
    void encode(const string& text) {
        // Calculate size needed
        int modeBits = 4;      // 0100 for byte mode
        int countBits = 8;     // Character count
        int textBits = text.length() * 8;  // ASCII to binary
        int sigBits = signature.length();
        
        dataLength = sigBits + modeBits + countBits + textBits;
        
        // Add padding to fill grid
        int totalCapacity = gridSize * gridSize;
        int paddingNeeded = totalCapacity - dataLength;
        if (paddingNeeded > 0) {
            dataLength = totalCapacity;
        }
        
        // Allocate memory
        if (binaryData != nullptr) {
            delete[] binaryData;
        }
        binaryData = new char[dataLength];
        
        int i;
        for (i = 0; i < dataLength; i++) {
            binaryData[i] = '0';
        }
        
        // Add signature
        int index = 0;
        for (i = 0; i < sigBits; i++) {
            binaryData[index] = signature[i];
            index++;
        }
        
        // Add mode (0100 = byte)
        binaryData[index++] = '0';
        binaryData[index++] = '1';
        binaryData[index++] = '0';
        binaryData[index++] = '0';
        
        // Add character count
        bitset<8> countBits8(text.length());
        string countStr = countBits8.to_string();
        for (i = 0; i < 8; i++) {
            binaryData[index] = countStr[i];
            index++;
        }
        
        // Convert text to binary
        for (i = 0; i < (int)text.length(); i++) {
            bitset<8> bits(text[i]);
            string bitStr = bits.to_string();
            int j;
            for (j = 0; j < 8; j++) {
                binaryData[index] = bitStr[j];
                index++;
            }
        }
        
        string pad1 = "11101100";
        string pad2 = "00010001";
        bool usePad1 = true;
        while (index < dataLength) {
            string currentPad = usePad1 ? pad1 : pad2;
            for (i = 0; i < 8 && index < dataLength; i++) {
                binaryData[index] = currentPad[i];
                index++;
            }
            usePad1 = !usePad1;
        }
        
        cout << "Encoded " << text.length() << " characters (" 
             << dataLength << " bits)\n";
    }
    
    // Decode QR to text
    string decode() {
        if (binaryData == nullptr) {
            return "";
        }
        
        // Remove signature
        int index = signature.length();
        
        // Skip mode
        index += 4;
        
        // Read character count
        string countStr = "";
        int i;
        for (i = 0; i < 8; i++) {
            countStr += binaryData[index];
            index++;
        }
        bitset<8> countBits(countStr);
        int charCount = (int)countBits.to_ulong();
        
        // Convert binary to text
        string result = "";
        for (i = 0; i < charCount && index + 8 <= dataLength; i++) {
            string byte = "";
            int j;
            for (j = 0; j < 8; j++) {
                byte += binaryData[index];
                index++;
            }
            bitset<8> bits(byte);
            char c = (char)bits.to_ulong();
            result += c;
        }
        
        return result;
    }
    
    // Display QR code visually
    void display() {
        if (binaryData == nullptr) {
            cout << "No QR code to display.\n";
            return;
        }
        
        // Create grid
        char** grid = new char*[gridSize];
        int i, j;
        for (i = 0; i < gridSize; i++) {
            grid[i] = new char[gridSize];
            for (j = 0; j < gridSize; j++) {
                grid[i][j] = '0';
            }
        }
        
        // Position squares (3 corners, 7x7)
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 7; j++) {
                if (i == 0 || i == 6 || j == 0 || j == 6 || 
                    (i >= 2 && i <= 4 && j >= 2 && j <= 4)) {
                    grid[i][j] = '1';
                    grid[i][gridSize - 7 + j] = '1';
                    grid[gridSize - 7 + i][j] = '1';
                } else {
                    grid[i][j] = '0';
                    grid[i][gridSize - 7 + j] = '0';
                    grid[gridSize - 7 + i][j] = '0';
                }
            }
        }
        
        // Timing patterns
        for (i = 8; i < gridSize - 8; i++) {
            grid[6][i] = (i % 2 == 0) ? '1' : '0';
            grid[i][6] = (i % 2 == 0) ? '1' : '0';
        }
        
        // Alignment pattern (5x5)
        int alignRow = gridSize - 7;
        int alignCol = gridSize - 7;
        for (i = -2; i <= 2; i++) {
            for (j = -2; j <= 2; j++) {
                if (abs(i) == 2 || abs(j) == 2 || (i == 0 && j == 0)) {
                    grid[alignRow + i][alignCol + j] = '1';
                } else {
                    grid[alignRow + i][alignCol + j] = '0';
                }
            }
        }
        
        grid[gridSize - 8][8] = '1';
        
        // Fill data
        int dataIndex = 0;
        for (i = gridSize - 1; i >= 8 && dataIndex < dataLength; i--) {
            for (j = gridSize - 1; j >= 8 && dataIndex < dataLength; j--) {
                if (grid[i][j] == '0') {
                    grid[i][j] = (binaryData[dataIndex] == '1') ? '1' : '.';
                    dataIndex++;
                }
            }
        }
        
        // Display with selected style
        cout << "\n";
        for (i = 0; i < gridSize; i++) {
            for (j = 0; j < gridSize; j++) {
                if (grid[i][j] == '1') {
                    if (displayStyle == "star") {
                        cout << "**";
                    } else if (displayStyle == "plus") {
                        cout << "++";
                    } else if (displayStyle == "x") {
                        cout << "XX";
                    } else if (displayStyle == "circle") {
                        cout << "()";
                    } else if (displayStyle == "square") {
                        cout << "[]";
                    } else if (displayStyle == "at") {
                        cout << "@@";
                    } else if (displayStyle == "dollar") {
                        cout << "$$";
                    } else { // block (default)
                        cout << "██";
                    }
                } else if (grid[i][j] == '.') {
                    if (displayStyle == "star") {
                        cout << "..";
                    } else if (displayStyle == "plus") {
                        cout << "--";
                    } else if (displayStyle == "x") {
                        cout << "xx";
                    } else if (displayStyle == "circle") {
                        cout << "..";
                    } else if (displayStyle == "square") {
                        cout << "..";
                    } else if (displayStyle == "at") {
                        cout << "..";
                    } else if (displayStyle == "dollar") {
                        cout << "¢¢";
                    } else { // block (default)
                        cout << "▒▒";
                    }
                } else {
                    // White module
                    cout << "  ";
                }
            }
            cout << "\n";
        }
        cout << "\n";
        
        for (i = 0; i < gridSize; i++) {
            delete[] grid[i];
        }
        delete[] grid;
    }
    
    // Get binary string
    string getBinaryString() {
        string result = "";
        int i;
        for (i = 0; i < dataLength; i++) {
            result += binaryData[i];
        }
        return result;
    }
    
    // Load binary string
    void loadBinaryString(const string& binary) {
        if (binaryData != nullptr) {
            delete[] binaryData;
        }
        
        dataLength = binary.length();
        binaryData = new char[dataLength];
        
        int i;
        for (i = 0; i < dataLength; i++) {
            binaryData[i] = binary[i];
        }
        
        gridSize = (int)ceil(sqrt(dataLength));
    }
    
    int getLength() { return dataLength; }
    string getSignature() { return signature; }
    int getGridSize() { return gridSize; }
};

// Function 1: Calculate checksum
int calculateChecksum(const string& data) {
    int sum = 0;
    int i;
    for (i = 0; i < (int)data.size(); i++) {
        sum += (int)data[i];
    }
    return sum % 256;
}

// Function 2: Save to file
void saveToFile(QRCode& qr) {
    string filename;
    cout << "Filename: ";
    getline(cin, filename);
    
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << "Binary: " << qr.getBinaryString() << "\n";
        outFile << "Length: " << qr.getLength() << " bits\n";
        outFile << "Grid: " << qr.getGridSize() << "x" << qr.getGridSize() << "\n";
        outFile << "Signature: " << qr.getSignature() << "\n";
        outFile.close();
        cout << "Saved to " << filename << "\n";
    } else {
        cout << "Error: Could not create file\n";
    }
}

// Function 3: Get QR input
string getQRInput() {
    cout << "Binary (1s and 0s): ";
    string input;
    getline(cin, input);
    
    string cleaned = "";
    int i;
    for (i = 0; i < (int)input.size(); i++) {
        if (input[i] == '0' || input[i] == '1') {
            cleaned += input[i];
        }
    }
    
    return cleaned;
}

// Function 4: Explain signature
void explainSignature() {
    cout << "\nSignature: 1010 (4 bits)\n";
    cout << "Position: Start of data\n";
    cout << "Purpose: Unique identifier for this generator\n";
    cout << "Removed automatically during decode\n\n";
}

// Function 5: Verify checksum
bool verifyChecksum(int original, int qrCheck) {
    return original == qrCheck;
}

// Function 6: Display checksum
void displayChecksumInfo(int msgSum, int qrSum) {
    cout << "Checksum: message=" << msgSum << " qr=" << qrSum;
    cout << " [" << (verifyChecksum(msgSum, qrSum) ? "MATCH" : "FAIL") << "]\n";
}

// Function 8: Get menu choice
string getMenuChoice() {
    string choice;
    cout << "> ";
    getline(cin, choice);
    return choice;
}

// Function 9: Ask continue
bool askContinue() {
    cout << "\nContinue? (y/n): ";
    string choice;
    getline(cin, choice);
    
    if (choice.size() > 0) {
        if (choice[0] == 'n' || choice[0] == 'N') {
            return false;
        } else if (choice[0] != 'y' && choice[0] != 'Y') {
            return false;
        }
    }
    return true;
}

// Function 10: Show style options
void showStyleOptions() {
    cout << "\nDisplay Styles:\n";
    cout << "block  - ██ ▒▒ (default)\n";
    cout << "star   - ** ..\n";
    cout << "plus   - ++ --\n";
    cout << "x      - XX xx\n";
    cout << "circle - () ..\n";
    cout << "square - [] ..\n";
    cout << "at     - @@ ..\n";
    cout << "dollar - $$ ¢¢\n\n";
}

int main() {
    QRCode* currentQR = nullptr;
    string lastMessage = "";
    
    cout << "QR Code Generator v2.1\n\n";
    
    while (true) {
        cout << "1. Encode  2. Decode  3. Display  4. Save  5. Signature  6. Style  exit\n";
        
        string choice = getMenuChoice();
        
        if (choice == "exit" || choice == "Exit") {
            cout << "Goodbye\n";
            break;
        }
        
        // ENCODE
        if (choice == "1") {
            cout << "Message: ";
            string message;
            getline(cin, message);
            
            if (message.empty()) {
                cout << "Error: Empty message\n";
                continue;
            }
            
            if (currentQR != nullptr) {
                delete currentQR;
            }
            currentQR = new QRCode();
            
            currentQR->encode(message);
            lastMessage = message;
            
            currentQR->display();
            
            int msgSum = calculateChecksum(message);
            string binaryStr = currentQR->getBinaryString();
            int qrSum = calculateChecksum(binaryStr);
            displayChecksumInfo(msgSum, qrSum);
        }
        
        // DECODE
        else if (choice == "2") {
            if (currentQR == nullptr || currentQR->getLength() == 0) {
                cout << "No QR code in memory\n";
            } else {
                string decoded = currentQR->decode();
                cout << "\nDecoded: \"" << decoded << "\"\n";
                
                int decodedSum = calculateChecksum(decoded);
                cout << "Checksum: " << decodedSum << "\n";
            }
        }
        
        // DISPLAY
        else if (choice == "3") {
            if (currentQR == nullptr || currentQR->getLength() == 0) {
                cout << "No QR code in memory\n";
            } else {
                currentQR->display();
            }
        }
        
        // SAVE
        else if (choice == "4") {
            if (currentQR == nullptr || currentQR->getLength() == 0) {
                cout << "No QR code to save\n";
            } else {
                saveToFile(*currentQR);
            }
        }
        
        // SIGNATURE
        else if (choice == "5") {
            explainSignature();
        }
        
        // STYLE
        else if (choice == "6") {
            showStyleOptions();
            cout << "Choose style: ";
            string style;
            getline(cin, style);
            
            if (currentQR != nullptr) {
                currentQR->setDisplayStyle(style);
                cout << "Style set to: " << style << "\n";
                currentQR->display();
            } else {
                cout << "No QR code in memory (encode a message first)\n";
            }
        }
        
        else {
            cout << "Invalid choice\n";
        }
        
        if (!askContinue()) {
            cout << "Goodbye\n";
            break;
        }
    }
    
    if (currentQR != nullptr) {
        delete currentQR;
        currentQR = nullptr;
    }
    
    return 0;
}
