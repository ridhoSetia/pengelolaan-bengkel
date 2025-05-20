#include <iostream>
#include <fstream>
#include <sstream>
#include "menu-admin.h"

using namespace std;

bool login(string& role) {
    string username, password, line, file_username, file_password, file_role;
    int attempts = 0;
    const int max_attempts = 3;

    while (attempts < max_attempts) {
        cout << "\nMasukkan username: ";
        cin >> username;
        cout << "Masukkan password: ";
        cin >> password;

        ifstream file("akun.csv");
        bool login_success = false;

        if (!file.is_open()) {
            cout << "Error: Tidak dapat membuka file akun.csv\n";
            return false;
        }

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, file_username, ',');
            getline(ss, file_password, ',');
            getline(ss, file_role, ',');

            if (username == file_username && password == file_password) {
                role = file_role;
                login_success = true;
                break;
            }
        }
        file.close();

        if (login_success) {
            cout << "Login berhasil sebagai " << role << endl;
            return true;
        } else {
            attempts++;
            cout << "Username atau password salah. Sisa percobaan: " << (max_attempts - attempts) << endl;
        }
    }

    cout << "Login gagal. Terlalu banyak percobaan.\n";
    return false;
}


int main() {
    int pilih;
    string role;
    while (true) {
        cout << "1. Login\n0. Keluar\nPilih: ";
        cin >> pilih;

        if (pilih == 1) {
            if (login(role)) {
                if (role == "admin") {
                    menu_admin();
                } else {
                    // menu_pegawai();
                }
            }
        } else if (pilih == 0) {
            cout << "Program selesai.\n";
            break;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }
    return 0;
}