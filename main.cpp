#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "menu-admin.h"
#include "menu-pegawai.h"

using namespace std;

const int MAX_AKUN = 100;

// Fungsi untuk menghapus spasi di awal dan akhir string
string trim(const string& str) {
    std::size_t first = str.find_first_not_of(" \t\r\n");
    std::size_t last = str.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

// Fungsi untuk membaca akun dari CSV (pakai stringstream dan string)
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

    return jumlahAkun;
}

// Fungsi login
bool login(Akun daftarAkun[], int jumlahAkun) {
    int percobaan = 0;

    while (percobaan < 3) {
        string inputNama, inputPassword;

        cout << "\nUsername: ";
        getline(cin, inputNama);
        cout << "Password: ";
        getline(cin, inputPassword);

        inputNama = trim(inputNama);
        inputPassword = trim(inputPassword);

        for (int i = 0; i < jumlahAkun; i++) {
            if (daftarAkun[i].username == inputNama && daftarAkun[i].password == inputPassword) {
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
            return false;
        } else {
            cout << "Username atau password salah, coba lagi.\n";
        }
    }

    return false;
}

// Main Program
// TO DO: fix boot loop jika nginput random di menu
int main() {
    Akun daftarAkun[MAX_AKUN];
    int jumlahAkun = bacaAkunDariCSV("akun.csv", daftarAkun);

    string pilihan;
    bool selesai = false;

    while (!selesai) {
        cout << "\n--- Menu Utama ---\n";
        cout << "1. Login\n2. Keluar\n";
        cout << "Pilihan: ";
        getline(cin, pilihan);
        pilihan = trim(pilihan);

        if (pilihan == "1") {
            cin.ignore(); //pindahkan kesini untuk menghindari bug
            bool berhasil = login(daftarAkun, jumlahAkun);
            if (!berhasil)
                selesai = true;
        } else if (pilihan == "2") {
            selesai = true;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }

    cout << "Program selesai.\n";
    return 0;
}