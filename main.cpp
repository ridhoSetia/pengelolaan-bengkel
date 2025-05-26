#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

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

// Menu items for the main menu
const char *mainMenuItems[] = {
    "1. Login",
    "0. Keluar"
};
const int mainMenuSize = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);

// Fungsi untuk menghapus spasi di awal dan akhir string
string trim(const string& str) {
    std::size_t first = str.find_first_not_of(" \t\r\n");
    std::size_t last = str.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

// Cross-platform terminal clearing
#ifdef _WIN32
void clearOutput() {
    system("cls");
}
#else
int _getch() {
    struct termios oldt, newt;
    int ch;
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        std::cerr << "Failed to get terminal attributes\n";
        return -1;
    }
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        std::cerr << "Failed to set terminal attributes\n";
        return -1;
    }
    ch = getchar();
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
        std::cerr << "Failed to restore terminal attributes\n";
        return -1;
    }
    return ch;
}
void clearOutput() {
    system("clear");
}
#endif

// Display menu with arrow key selection
void displayMenu(int currentSelection) {
    clearOutput();
    cout << "--- Menu Utama ---\n";
    cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
    for (int i = 0; i < mainMenuSize; i++) {
        if (i == currentSelection)
            cout << " -> " << mainMenuItems[i] << "\n";
        else
            cout << "     " << mainMenuItems[i] << "\n";
    }
}

// Handle arrow key input for menu navigation
int handleInput(int &currentSelection) {
    while (true) {
        displayMenu(currentSelection);
        int key = _getch();
        if (key == -1) continue; // Skip on error
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
            if (second == 91) {
                int arrow = _getch();
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
}

// Fungsi untuk membaca akun dari CSV
int bacaAkunDariCSV(const string& namaFile, Akun daftarAkun[]) {
    ifstream file(namaFile);
    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << namaFile << endl;
        return 0;
    }

    string line;
    int jumlahAkun = 0;

    while (getline(file, line) && jumlahAkun < MAX_AKUN) {
        stringstream ss(line);
        string username, password, role;

        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, role);

        username = trim(username);
        password = trim(password);
        role = trim(role);

        daftarAkun[jumlahAkun].username = username;
        daftarAkun[jumlahAkun].password = password;
        daftarAkun[jumlahAkun].role = role;
        jumlahAkun++;
    }
    file.close();
    return jumlahAkun;
}

// Fungsi login
bool login(Akun daftarAkun[], int jumlahAkun) {
    int percobaan = 0;

    while (percobaan < 3) {
        clearOutput();
        string inputNama, inputPassword;

        cout << "\nUsername: ";
        getline(cin, inputNama);
        cout << "Password: ";
        getline(cin, inputPassword);

        inputNama = trim(inputNama);
        inputPassword = trim(inputPassword);

        for (int i = 0; i < jumlahAkun; i++) {
            if (daftarAkun[i].username == inputNama && daftarAkun[i].password == inputPassword) {
                clearOutput();
                if (daftarAkun[i].role == "admin")
                    menu_admin();
                else
                    menu_pegawai();
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
}

// Main Program
int main() {
    Akun daftarAkun[MAX_AKUN];
    int jumlahAkun = bacaAkunDariCSV("akun.csv", daftarAkun);
    int currentSelection = 0;
    bool selesai = false;

    while (!selesai) {
        int pilihan = handleInput(currentSelection);
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
                cout << "Pilihan tidak valid.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
        }
    }

    clearOutput();
    cout << "Program selesai.\n";
    return 0;
}