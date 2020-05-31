#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


bool plain_text_key_valid(std::string, std::string);
bool sort_vec_by_0(std::vector<char>, std::vector<char>);
bool sort_vec_by_1(std::vector<char>, std::vector<char>);
int convertToDecimal(std::array<int, 6>);
std::array<int, 6> convertToBinary(int);
std::string col_trans(std::string, std::string, bool);
std::string deciper_col_trans(std::string, std::string);
std::string decryptOneTimePad(std::string, std::string, std::vector<std::vector<char>>);
std::string get_message_from_plain_text(std::string);
std::string key_from_poly_square(std::string);
std::string keyToNumbers(std::string, std::vector<std::vector<char>>);
std::string OneTimePad(std::string, std::string, std::vector<std::vector<char>>, bool);
std::string symmetric_one_time_pad(std::string, std::string, bool);

std::vector<std::vector<char>> polybius_square();
void print_vector(std::vector<std::vector<char>>);
void usage();


int main(int argc, char* argv[]) {
	std::vector <std::string> args;
	std::string destination;
	bool debug = false;
	for (int i = 1; i < argc; ++i) { // Remember argv[0] is the path to the program, we want from argv[1] onwards
		args.push_back(argv[i]);
	}
	std::string plain_text;
	std::string composite_key;
	for (int i = 0; i < args.size(); i++) {
		if (args[i] == "-p") {
			plain_text = args[i + 1];
		}
		if (args[i] == "-k") {
			composite_key = args[i + 1];
		}
		if (args[i] == "-d") {
			debug = true;
		}
	}
	if (argc < 6) { // No arguments, get them manually
		std::string operation;
		std::vector<std::string> valid_values = { "encrypt", "decrypt" };
		while (std::find(valid_values.begin(), valid_values.end(), operation) == valid_values.end()) {
			std::cout << "Enter desired operation (encrypt/decrypt): ";
			std::cin >> operation;
		}
		args.push_back(operation);
		if (operation == "encrypt") {
			std::cout << "Enter your plaintext: ";
		} else {
			std::cout << "Enter your encrypted message: ";
		}
		std::cin.ignore();
		std::getline(std::cin, plain_text);
		std::cout << "Enter your composite key: ";
		std::cin >> composite_key;
	}
	if (!plain_text_key_valid(plain_text, composite_key)) {
		usage();
		return 0;
	}
	auto square = polybius_square();
	std::string trans_key = composite_key.substr(0, composite_key.size() - 2);
	if (debug) {
		std::cout << "Polybius key: " << trans_key << std::endl;
	}
	auto col_key = key_from_poly_square(trans_key);
	if (debug) {
		std::cout << "Columnar Transposition Key: " << col_key << std::endl;
	}
	if (args[0] == "encrypt") { // If encrypt we're doing encryption
		std::cout << "Encrypted Message: " << OneTimePad(col_trans(plain_text, col_key, debug), composite_key, square, debug) << std::endl;
	}
	else if (args[0] == "decrypt") {
		std::cout << "Decrypted Message: " << deciper_col_trans(decryptOneTimePad(plain_text, composite_key, square), col_key) << std::endl;
	}
	else {
		usage();
	}
	return 0;
}

std::vector<std::vector<char>> polybius_square() {
	return {
		{'E', 'Y', 'O', 'P', 'D', '9'},
		{'2', 'H', 'Q', 'X', '1', 'I'},
		{'R', '3', 'A', 'J', '8', 'S'},
		{'F', '0', 'N', '4', 'G', '5'},
		{'Z', 'B', 'U', 'V', 'C', 'T'},
		{'M', '7', 'K', 'W', '6', 'L'},
	};
}

std::string symmetric_one_time_pad(std::string message, std::string key, bool debug = false) {
	// Get the last two digits for one time pad and convert it to binary
	std::string lastTwoChars = key.substr(key.size() - 2, 2);
	if (debug) {
		std::cout << "One-time pad key: " << lastTwoChars << std::endl;
	}
	std::stringstream lastTwoDigits(lastTwoChars);
	int oneTimeKey = 0;
	lastTwoDigits >> oneTimeKey;
	std::array<int, 6> binOneTimeKey = convertToBinary(oneTimeKey);

	// Convert decryptCipher1 to binary
	std::vector<std::array<int, 6>> binEncryption;
	for (int i = 0; i < message.size(); i += 2) {
		std::string twoChars = message.substr(i, 2);
		std::stringstream ssTwoChars(twoChars);
		int num = 0;
		ssTwoChars >> num;
		std::array<int, 6> binNum = convertToBinary(num);
		binEncryption.push_back(binNum);
	}

	if (debug) {
		std::string debug_msg;
		for (auto n : binEncryption) {
			debug_msg += std::to_string(convertToDecimal(n));
		}
		std::cout << "Polybius transformation: " << debug_msg << std::endl;
	}

	// apply XOR on each of the bits.
	for (int i = 0; i < binEncryption.size(); i++) {
		for (int j = 0; j < 6; j++) {
			binEncryption[i][j] = binEncryption[i][j] ^ binOneTimeKey[j];
		}
	}

	// convert each number back to decimal
	std::string return_message = "";
	for (int i = 0; i < binEncryption.size(); i++) {
		int dec = convertToDecimal(binEncryption[i]);
		if (dec < 10) {
			return_message += std::to_string(0);
		}
		return_message += std::to_string(dec);
	}
	return return_message;
}

std::string OneTimePad(std::string cipher1, std::string compositeKey, std::vector<std::vector<char>> square, bool debug = false) {
	std::string decryptCipher1 = keyToNumbers(cipher1, square);
	return symmetric_one_time_pad(decryptCipher1, compositeKey, debug);
}

std::string decryptOneTimePad(std::string encryptedMessage, std::string compositeKey, std::vector<std::vector<char>> square) {
	return key_from_poly_square(symmetric_one_time_pad(encryptedMessage, compositeKey));
}

std::array<int, 6> convertToBinary(int num) {
	std::array<int, 6> bin = { 0,0,0,0,0,0 };
	int digit = 0;
	//check which digit we need to start at
	if (num < 32) {
		int temp = 32;
		while (temp > num) {
			digit++;
			temp /= 2;
		}
	}
	// convert to each digit to binary
	for (int i = bin.size() - 1; i >= digit; i--) {
		bin[i] = num % 2;
		num = num / 2;
	}

	return bin;
}

int convertToDecimal(std::array<int, 6> num) {
	int value = 32;
	int decimal = 0;
	for (int v = 0; v < num.size(); v++) {
		if (num[v] == 1) {
			decimal += value;
		}
		value = value / 2;
	}
	return decimal;
}

// this will take the given key and use the p square 
// to turn it into its number equavalent (each letter or number 
// has a 2-digit number associated with it)
std::string keyToNumbers(std::string key, std::vector<std::vector<char>> square) {

	std::string newKey = "";

	//probably not the best way... triple for loops.. sorry
	for (int k = 0; k < key.size(); k++) {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				if (square[i][j] == key[k]) {
					newKey += std::to_string(j);
					newKey += std::to_string(i);
				}
			}

		}
	}
	return newKey;
}

std::string col_trans(std::string plain_text, std::string key, bool debug = false) {

	std::string return_string = "";
	auto message = get_message_from_plain_text(plain_text);
	std::vector<std::vector<char>> cipher_vec;

	for (int i = 0; i < key.size(); i++) {
		cipher_vec.push_back({ key[i] });// Adds Key to the cipher vector for sorting later
	}

	for (int i = 0; i < message.size(); i++) {
		cipher_vec[i % key.size()].push_back(message[i]);
	}

	std::sort(cipher_vec.begin(), cipher_vec.end(), sort_vec_by_0); // sort cipher vector by first character in vector (sort by key)

	for (auto vec : cipher_vec) {
		for (int i = 1; i < vec.size(); i++) {
			return_string.push_back(vec[i]);
		}
	}

	if (debug) {
		std::cout << "Columnar Transposition cipher text: " << return_string << std::endl;
	}

	return return_string;
}

std::string deciper_col_trans(std::string encrypted_text, std::string key) {

	std::string deciphered_text = "";
	int num_blocks = encrypted_text.size() / key.size();
	int missing_chars = encrypted_text.size() % key.size();
	std::vector<std::vector<char>> decipher_vec; // 2d vector

	for (int i = 0; i < key.size(); i++) {
		decipher_vec.push_back({ key[i], (char)i }); // push the key onto the vector
	}

	// Sort vectors by element 0 to scramble key (so we put encrypted message back in correct order)
	std::sort(decipher_vec.begin(), decipher_vec.end(), sort_vec_by_0);

	for (int i = 0,k = 0; i < key.size(); i++) {
		for (int j = 0; j <= num_blocks; j++) {
			// We're in the last row, and we need to check if the index is less than number of missing chars
			if (j == num_blocks && decipher_vec[i][1] >= missing_chars) {
				continue;
			}
			decipher_vec[i].push_back(encrypted_text[k++]); // Push back num_blocks (calculated number of rows) characters to column vector i
		}
	}

	std::sort(decipher_vec.begin(), decipher_vec.end(), sort_vec_by_1);	// Sort by element 1 to reorder message

	for (int i = 0; i <= num_blocks; i++) { // For each row in vector
		for (int j = 0; j < decipher_vec.size(); j++) { // For each column in vector
			// If we're working on the last row and we're working on a column index that is greater than or equal to the number of missing characters
			if (i == num_blocks && decipher_vec[j][1] >= missing_chars) {
				// Skip the characters (they are "padding")
				continue;
			}
			// Work across the rows and skip the top two rows as they are key and index data
			deciphered_text.push_back(decipher_vec[j][i+2]);
		}
	}

	return deciphered_text;
}

std::string key_from_poly_square(std::string key) {
	auto poly_square = polybius_square();
	std::vector<std::vector<int>> key_pairs;
	std::string return_string = "";
	for (int i = 0; i < key.size(); i += 2) {
		std::vector<int> tmp = { (int)key[i] - 48, (int)key[i + 1] - 48 };
		key_pairs.push_back(tmp);
	}
	for (auto row : key_pairs) {
		return_string += poly_square[row[1]][row[0]];
	}
	return return_string;
}

std::string get_message_from_plain_text(std::string plain_text) {
	plain_text.erase(std::remove_if(plain_text.begin(), plain_text.end(), isspace), plain_text.end()); // Remove all spaces from plain text
	std::transform(plain_text.begin(), plain_text.end(), plain_text.begin(), ::toupper); // Convert plain text to uppercase
	return plain_text;
}

// ######################UTILS#########################

bool sort_vec_by_0(std::vector<char> i, std::vector<char> j) {
	return i[0] < j[0];
}

bool sort_vec_by_1(std::vector<char> i, std::vector<char> j) {
	return i[1] < j[1];
}

void print_vector(std::vector<std::vector<char>> vector) {
	int total_elements = 0;
	for (auto vec : vector) {
		total_elements += vec.size();
	}
	std::cout << "Debug Info" << std::endl;
	std::cout << std::endl;
	std::cout << total_elements << std::endl;
	for (int i = 0; i <= (total_elements/vector.size()); i++) {
		for (auto vec : vector) {
			std::cout << " " << vec[i] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// ######################INPUT/OUTPUT/USER MESSAGES#########################

void usage() {
	std::cout << "Usage" << std::endl
		<< "main encrypt -p \"hello there my friends\" -k \"1422555515\"" << std::endl
		<< "main decrypt -p \"15571513390427131224560410605956150515\" -k \"1422555515\"" << std::endl;
}

bool plain_text_key_valid(std::string plain_text, std::string key) {
	if (plain_text.size() < 1 || key.size() < 1) {
		std::cout << "Must include a plaintext/key with -p/-k" << std::endl;
		return false;
	}
	if (key.size() % 2 != 0) {
		std::cout << "Composite key should be a string of digits with length divisible by 2!" << std::endl;
		return false;
	}
	return true;
}