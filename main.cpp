#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <stdexcept>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "menu-admin.h"
#include "menu-pegawai.h"

using namespace std;

const int MAX_AKUN = 100;

const char *mainMenuItems[] = {
    "1. Login",
    "0. Keluar"
};
const int mainMenuSize = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);

// Fungsi untuk menghapus spasi di awal dan akhir string
string trim(const string& str) {
    try {
        std::size_t first = str.find_first_not_of(" \t\r\n");
        std::size_t last = str.find_last_not_of(" \t\r\n");
        if (first == string::npos || last == string::npos)
            return "";
        return str.substr(first, last - first + 1);
    } catch (const std::exception& e) {
        cerr << "Error trimming string: " << e.what() << endl;
        return str;
    }
}

#ifdef _WIN32
void clearOutput() {
    if (system("cls") != 0) {
        cerr << "Failed to clear terminal screen" << endl;
    }
}
#else
int _getch() {
    struct termios oldt, newt;
    int ch;
    try {
        if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
            throw std::runtime_error("Failed to get terminal attributes");
        }
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
            throw std::runtime_error("Failed to set terminal attributes");
        }
        ch = getchar();
        if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
            throw std::runtime_error("Failed to restore terminal attributes");
        }
        return ch;
    } catch (const std::exception& e) {
        cerr << "Error in _getch: " << e.what() << endl;
        return -1;
    }
}
void clearOutput() {
    if (system("clear") != 0) {
        cerr << "Failed to clear terminal screen" << endl;
    }
}
#endif

void displayMenu(int currentSelection) {
    try {
        clearOutput();
        cout << "--- Menu Utama ---\n";
        cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
        for (int i = 0; i < mainMenuSize; i++) {
            if (i == currentSelection)
                cout << " -> " << mainMenuItems[i] << "\n";
            else
                cout << "     " << mainMenuItems[i] << "\n";
        }
    } catch (const std::exception& e) {
        cerr << "Error displaying menu: " << e.what() << endl;
    }
}

int handleInput(int &currentSelection) {
    try {
        while (true) {
            displayMenu(currentSelection);
            int key = _getch();
            if (key == -1) {
                cerr << "Invalid input detected, please try again" << endl;
                continue;
            }
#ifdef _WIN32
            if (key == 224) {
                int arrow = _getch();
                if (arrow == 72) { // Up
                    currentSelection = (currentSelection - 1 + mainMenuSize) % mainMenuSize;
                } else if (arrow == 80) { // Down
                    currentSelection = (currentSelection + 1) % mainMenuSize;
                }
            } else if (key == 13) { // Enter
                return currentSelection;
            }
#else
            if (key == 27) {
                int second = _getch();
                if (second == -1) continue;
                if (second == 91) {
                    int arrow = _getch();
                    if (arrow == -1) continue;
                    if (arrow == 65) { // Up
                        currentSelection = (currentSelection - 1 + mainMenuSize) % mainMenuSize;
                    } else if (arrow == 66) { // Down
                        currentSelection = (currentSelection + 1) % mainMenuSize;
                    }
                }
            } else if (key == 10) { // Enter
                return currentSelection;
            }
#endif
        }
    } catch (const std::exception& e) {
        cerr << "Error handling input: " << e.what() << endl;
        return -1;
    }
}

// Fungsi untuk membaca akun dari CSV
int bacaAkunDariCSV(const string& namaFile, Akun daftarAkun[]) {
    try {
        ifstream file(namaFile);
        if (!file.is_open()) {
            throw std::runtime_error("Gagal membuka file: " + namaFile);
        }

        string line;
        int jumlahAkun = 0;

        while (getline(file, line) && jumlahAkun < MAX_AKUN) {
            stringstream ss(line);
            string username, password, role;

            if (!getline(ss, username, ',')) {
                cerr << "Warning: Invalid username format in line " << jumlahAkun + 1 << endl;
                continue;
            }
            if (!getline(ss, password, ',')) {
                cerr << "Warning: Invalid password format in line " << jumlahAkun + 1 << endl;
                continue;
            }
            if (!getline(ss, role)) {
                cerr << "Warning: Invalid role format in line " << jumlahAkun + 1 << endl;
                continue;
            }

            username = trim(username);
            password = trim(password);
            role = trim(role);

            if (username.empty() || password.empty() || role.empty()) {
                cerr << "Warning: Empty field detected in line " << jumlahAkun + 1 << endl;
                continue;
            }

            daftarAkun[jumlahAkun].username = username;
            daftarAkun[jumlahAkun].password = password;
            daftarAkun[jumlahAkun].role = role;
            jumlahAkun++;
        }
        file.close();
        return jumlahAkun;
    } catch (const std::exception& e) {
        cerr << "Error reading CSV file: " << e.what() << endl;
        return 0;
    }
}

bool login(Akun daftarAkun[], int jumlahAkun) {
    try {
        int percobaan = 0;
        while (percobaan < 3) {
            clearOutput();
            string inputNama, inputPassword;

            cout << "\nUsername: ";
            if (!getline(cin, inputNama)) {
                throw std::runtime_error("Failed to read username input");
            }
            cout << "Password: ";
            if (!getline(cin, inputPassword)) {
                throw std::runtime_error("Failed to read password input");
            }

            inputNama = trim(inputNama);
            inputPassword = trim(inputPassword);

            if (inputNama.empty() || inputPassword.empty()) {
                cerr << "Error: Username or password cannot be empty" << endl;
                cout << "Tekan enter untuk melanjutkan\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                percobaan++;
                continue;
            }

            for (int i = 0; i < jumlahAkun; i++) {
                if (daftarAkun[i].username == inputNama && daftarAkun[i].password == inputPassword) {
                    clearOutput();
                    if (daftarAkun[i].role == "admin")
                        menu_admin();
                    else if (daftarAkun[i].role == "pegawai")
                        menu_pegawai();
                    else {
                        cerr << "Error: Invalid role for user " << inputNama << endl;
                        return false;
                    }
                    return true;
                }
            }

            percobaan++;
            if (percobaan == 3) {
                cout << "Batas percobaan tercapai.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return false;
            } else {
                cout << "Username atau password salah, coba lagi.\n";
                cout << "Tekan enter untuk melanjutkan\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        return false;
    } catch (const std::exception& e) {
        cerr << "Error during login: " << e.what() << endl;
        cout << "Tekan enter untuk kembali\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
}

// Main Program
int main() {
    try {
        Akun daftarAkun[MAX_AKUN];
        int jumlahAkun = bacaAkunDariCSV("akun.csv", daftarAkun);
        if (jumlahAkun == 0 && !ifstream("akun.csv").good()) {
            throw std::runtime_error("No valid accounts loaded from CSV");
        }

        int currentSelection = 0;
        bool selesai = false;

        while (!selesai) {
            int pilihan = handleInput(currentSelection);
            if (pilihan == -1) {
                cerr << "Error: Invalid menu selection" << endl;
                cout << "Tekan enter untuk melanjutkan\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            clearOutput();
            switch (pilihan) {
                case 0: // Login
                    if (!login(daftarAkun, jumlahAkun)) {
                        selesai = true;
                    }
                    break;
                case 1: // Keluar
                    selesai = true;
                    break;
                default:
                    cerr << "Error: Pilihan tidak valid.\n";
                    cout << "Tekan enter untuk kembali\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
            }
        }

        clearOutput();
        cout << "Program selesai.\n";
        return 0;
    } catch (const std::exception& e) {
        cerr << "Fatal error in main: " << e.what() << endl;
        cout << "Tekan enter untuk keluar\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return 1;
    }
}