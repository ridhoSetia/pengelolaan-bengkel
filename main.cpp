#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

const int MAX_AKUN = 100;

struct Akun {
    string nama;
    string password;
    bool isAdmin;
};

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
        string nama, password, isAdminStr;

        getline(ss, nama, ',');
        getline(ss, password, ',');
        getline(ss, isAdminStr);

        bool isAdmin = (isAdminStr == "true");
        daftarAkun[jumlahAkun].nama = nama;
        daftarAkun[jumlahAkun].password = password;
        daftarAkun[jumlahAkun].isAdmin = isAdmin;
        jumlahAkun++;
    }

    return jumlahAkun;
}

// Menu Admin
void menuAdmin() {
    cout << "\n--- Menu Admin ---\n";
    cout << "Anda sedang berada di menu admin.\n";
}

// Menu Pegawai
void menuPegawai() {
    cout << "\n--- Menu Pegawai ---\n";
    cout << "Anda sedang berada di menu pegawai.\n";
}

// Fungsi login
bool login(Akun daftarAkun[], int jumlahAkun) {
    int percobaan = 0;

    while (percobaan < 3) {
        string inputNama, inputPassword;

        cout << "\nUsername: ";
        cin >> inputNama;
        cout << "Password: ";
        cin >> inputPassword;


        for (int i = 0; i < jumlahAkun; i++) {
            if (daftarAkun[i].nama == inputNama && daftarAkun[i].password == inputPassword) {
                if (daftarAkun[i].isAdmin)
                    menuAdmin();
                else
                    menuPegawai();
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
int main() {
    Akun daftarAkun[MAX_AKUN];
    int jumlahAkun = bacaAkunDariCSV("akun.csv", daftarAkun);

    int pilihan;
    bool selesai = false;

    while (!selesai) {
        cout << "\n--- Menu Utama ---\n";
        cout << "1. Login\n2. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan;

        if (pilihan == 1) {
            bool berhasil = login(daftarAkun, jumlahAkun);
            if (!berhasil)
                selesai = true;
        } else if (pilihan == 2) {
            selesai = true;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }

    cout << "Program selesai.\n";
    return 0;
}