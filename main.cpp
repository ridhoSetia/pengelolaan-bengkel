#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

const int MAX_ROWS = 10;
const int MAX_COLS = 4;

// void write_csv(string filename, string nama_motor, string no_plat, string no_hp, string tgl_masuk){
//     ofstream data(filename, ios::app);

//     if (data.is_open())
//     {
//         // Send the column name to the stream
//         data << nama_motor;
//         data << ", ";
//         data << no_plat;
//         data << ", ";
//         data << no_hp;   
//         data << ", ";
//         data << tgl_masuk << "\n";   
//         // Close the file
//         data.close();
//     } 
//     else 
//     {
//         cerr << "Gagal membuka file." << endl;
//     }
// }

// Fungsi membaca CSV ke array 2D
int read_csv(const string& filename, string data[MAX_ROWS][MAX_COLS]) {
    ifstream file(filename);
    string line;
    int row = 0;

    if (!file.is_open()) {
        cerr << "Gagal membuka file CSV." << endl;
        return -1;
    }

    while (getline(file, line) && row < MAX_ROWS) {
        stringstream ss(line);
        string cell;
        int col = 0;

        while (getline(ss, cell, ',') && col < MAX_COLS) {
            data[row][col] = cell;
            col++;
        }

        row++;
    }

    file.close();
    return row; // Mengembalikan jumlah baris yang berhasil dibaca
}

void update_csv_cell(const string& filename, int targetRow, int targetCol, const string& newValue) {
    ifstream inFile(filename);
    ofstream tempFile("temp.csv");

    string line;
    int currentRow = 0;

    if (!inFile.is_open() || !tempFile.is_open()) {
        cerr << "Gagal membuka file." << endl;
        return;
    }

    while (getline(inFile, line)) {
        stringstream ss(line);
        string cell;
        string newLine;
        int currentCol = 0;

        // Jika ini baris target
        if (currentRow == targetRow) {
            while (getline(ss, cell, ',')) {
                if (currentCol == targetCol) {
                    newLine += newValue;  // Update data
                } else {
                    newLine += cell;
                }

                if (ss.peek() != EOF) newLine += ","; // Tambahkan koma jika bukan kolom terakhir
                currentCol++;
            }
        } else {
            newLine = line; // Salin baris lain tanpa perubahan
        }

        tempFile << newLine << endl;
        currentRow++;
    }

    inFile.close();
    tempFile.close();

    remove(filename.c_str());
    rename("temp.csv", filename.c_str());
}

void delete_csv_row(const string& filename, int rowToDelete) {
    ifstream inFile(filename);
    ofstream tempFile("temp.csv");
    string line;
    int currentRow = 0;

    if (!inFile.is_open() || !tempFile.is_open()) {
        cerr << "Gagal membuka file." << endl;
        return;
    }

    // Salin semua baris kecuali baris ke-n
    while (getline(inFile, line)) {
        if (currentRow != rowToDelete) {
            tempFile << line << endl;
        }
        currentRow++;
    }

    inFile.close();
    tempFile.close();

    // Hapus file asli dan ganti dengan file baru
    remove(filename.c_str());
    rename("temp.csv", filename.c_str());
}

const char* menuItems[] = {
    "Menu 1",
    "Menu 2",
    "Menu 3",
    "Keluar"
};

const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);
int currentSelection = 0;

#ifdef _WIN32
void clearScreen() {
    system("cls");
}
#else
// getch alternatif untuk Linux
int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);            // simpan mode lama
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);          // non-canonical, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);   // terapkan mode baru
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);   // kembalikan mode lama
    return ch;
}

void clearScreen() {
    system("clear");
}
#endif

void displayMenu() {
    clearScreen();
    cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
    for (int i = 0; i < menuSize; i++) {
        // cout << i << endl;
        // cout << currentSelection << endl;
        if (i == currentSelection)
            cout << " -> " << menuItems[i] << "\n";
        else
            cout << "     " << menuItems[i] << "\n";
    }
}

void handleInput() {
    while (true) {
        displayMenu();
        int key = _getch();

#ifdef _WIN32
        if (key == 224) {
            int arrow = _getch();
            if (arrow == 72) { // Up
                currentSelection = (currentSelection - 1 + menuSize) % menuSize;
            } else if (arrow == 80) { // Down
                currentSelection = (currentSelection + 1) % menuSize;
            }
        } else if (key == 13) { // Enter
            break;
        }
#else
        if (key == 27) { // Escape
            int second = _getch();
            if (second == 91) {
                int arrow = _getch();
                if (arrow == 65) { // Up
                    currentSelection = (currentSelection - 1 + menuSize) % menuSize;
                } else if (arrow == 66) { // Down
                    currentSelection = (currentSelection + 1) % menuSize;
                }
            }
        } else if (key == 10) { // Enter
            break;
        }
#endif
    }
}

int main()
{    
    int inputRow;
    cout << "Pilih antrean yang ingin diupdate: ";
    cin >> inputRow;

    string nama_motor, no_plat, no_hp, tgl_masuk;
    cout << "Update nama motor: ";
    cin.ignore();
    getline(cin, nama_motor);
    cout << "Update nomor plat motor: ";
    getline(cin, no_plat);
    cout << "Update nomor hp: ";
    getline(cin, no_hp);
    cout << "Update tanggal masuk ke bengkel: ";
    getline(cin, tgl_masuk);
    update_csv_cell("data.csv", inputRow-1, 0, nama_motor);
    update_csv_cell("data.csv", inputRow-1, 1, no_plat);
    update_csv_cell("data.csv", inputRow-1, 2, no_hp);
    update_csv_cell("data.csv", inputRow-1, 3, tgl_masuk);

    handleInput();
    clearScreen();
    cout << "Kamu memilih: " << menuItems[currentSelection] << "\n";

    string csvData[MAX_ROWS][MAX_COLS];
    int totalRows = read_csv("data.csv", csvData);

    if (totalRows != -1) {
        for (int i = 0; i < totalRows; i++) {
            for (int j = 0; j < MAX_COLS && !csvData[i][j].empty(); j++) {
                cout << csvData[i][j] << " ";
            }
            cout << endl;
        }
    }

    int hapus;
    cout << "Masukkan motor yang ingin dihapus: ";
    cin >> hapus;

    delete_csv_row("data.csv", hapus+1);

    // Send data to the stream
    // string nama_motor, no_plat, no_hp, tgl_masuk;
    // cout << "Masukkan nama motor: ";
    // cin.ignore();
    // getline(cin, nama_motor);
    // cout << "Masukkan nomor plat motor: ";
    // getline(cin, no_plat);
    // cout << "Masukkan nomor hp: ";
    // getline(cin, no_hp);
    // cout << "Masukkan tanggal masuk ke bengkel: ";
    // getline(cin, tgl_masuk);
    
    // write_csv("data.csv", nama_motor, no_plat, no_hp, tgl_masuk);
    
    return 0;
}