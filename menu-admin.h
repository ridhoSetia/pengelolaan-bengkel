#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

struct DataMotor {
    string namaMotor;
    string noPlat;
    string noHp;
    string tglMasuk;
};

struct Akun {
    string username;
    string password;
    string role; // "admin" atau "pegawai"
};


void tampilkan_antrean();
void tampilkan_laporan();
void rekrutPegawai();
void pecatPegawai();
void daftarPegawai();

void menu_admin() {
    int pilihan;
    do {
        cout << "\nMenu Admin:\n1. Lihat Antrean\n2. Lihat Laporan\n3. Manajemen Akun Pegawai\n0. Keluar\nPilih: ";
        cin >> pilihan;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (pilihan == 1) {
            tampilkan_antrean();

        } else if (pilihan == 2) {
            tampilkan_laporan();

        } else if (pilihan == 3) {
            int subPilihan;
            do {
                cout << "\nManajemen Akun Pegawai:\n1. Rekrut Pegawai\n2. Pecat Pegawai\n3. Daftar Pegawai\n0. Keluar\nPilih: ";
                cin >> subPilihan;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (subPilihan == 1) {
                    rekrutPegawai();

                } else if (subPilihan == 2) {
                    pecatPegawai();

                } else if (subPilihan == 3) {
                    daftarPegawai();

                } else if (subPilihan != 0) {
                    cout << "Pilihan tidak valid.\n";
                }
            } while (subPilihan != 0);

        } else if (pilihan != 0) {
            cout << "Pilihan tidak valid.\n";
        }

    } while (pilihan != 0);
}

void tampilkan_antrean() {
    ifstream file("data.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file antrean atau belum ada data.\n";
        return;
    }

    vector<DataMotor> antrean;
    DataMotor motor;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, motor.namaMotor, ',');
        getline(ss, motor.noPlat, ',');
        getline(ss, motor.noHp, ',');
        getline(ss, motor.tglMasuk);
        antrean.push_back(motor);
    }
    file.close();

    if (antrean.empty()) {
        cout << "Antrean kosong.\n";
        return;
    }

    cout << "\nDaftar Antrean Motor:\n";
    cout << setw(20) << "Nama Motor" << setw(15) << "No. Plat" << setw(15) << "No. HP" << setw(15) << "Tgl Masuk" << endl;
    for (const auto& m : antrean) {
        cout << setw(20) << m.namaMotor << setw(15) << m.noPlat << setw(15) << m.noHp << setw(15) << m.tglMasuk << endl;
    }
}

void tampilkan_laporan() {
    ifstream file("laporan.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file laporan atau belum ada data.\n";
        return;
    }

    vector<DataMotor> laporan;
    DataMotor motor;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, motor.namaMotor, ',');
        getline(ss, motor.noPlat, ',');
        getline(ss, motor.noHp, ',');
        getline(ss, motor.tglMasuk);
        laporan.push_back(motor);
    }
    file.close();

    if (laporan.empty()) {
        cout << "Laporan kosong.\n";
        return;
    }

    cout << "\nLaporan Servis Motor:\n";
    cout << setw(20) << "Nama Motor" << setw(15) << "No. Plat" << setw(15) << "No. HP" << setw(15) << "Tgl Masuk" << endl;
    for (const auto& m : laporan) {
        cout << setw(20) << m.namaMotor << setw(15) << m.noPlat << setw(15) << m.noHp << setw(15) << m.tglMasuk << endl;
    }
}

void rekrutPegawai() {
    Akun akun;
    cout << "\nMasukkan username baru: ";
    getline(cin, akun.username);
    cout << "Masukkan password baru: ";
    getline(cin, akun.password);
    akun.role = "pegawai"; // Set default role sebagai pegawai

    // Cek apakah username sudah ada
    ifstream file("akun.csv");
    string line;
    bool usernameExists = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string existingUsername;
        getline(ss, existingUsername, ',');
        if (existingUsername == akun.username) {
            usernameExists = true;
            break;
        }
    }
    file.close();

    if (usernameExists) {
        cout << "Username sudah digunakan. Silakan coba username lain.\n";
        return;
    }

    // Simpan akun baru ke file
    ofstream outFile("akun.csv", ios::app);
    if (!outFile.is_open()) {
        cout << "Gagal membuka file akun.\n";
        return;
    }
    outFile << akun.username << "," << akun.password << "," << akun.role << "\n";
    outFile.close();
    cout << "Pegawai berhasil direkrut.\n";
}

void pecatPegawai() {
    string username;
    cout << "\nMasukkan username pegawai yang akan dipecat: ";
    getline(cin, username);

    // Baca semua akun dari file
    ifstream file("akun.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file akun atau belum ada data.\n";
        return;
    }

    vector<Akun> akunList;
    Akun akun;
    string line;
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, akun.username, ',');
        getline(ss, akun.password, ',');
        getline(ss, akun.role);
        if (akun.username == username && akun.role == "pegawai") {
            found = true; // Tandai bahwa akun ditemukan
        } else {
            akunList.push_back(akun); // Simpan akun lain
        }
    }
    file.close();

    if (!found) {
        cout << "Pegawai dengan username tersebut tidak ditemukan atau bukan pegawai.\n";
        return;
    }

    // Tulis kembali akun yang tidak dihapus ke file
    ofstream outFile("akun.csv");
    if (!outFile.is_open()) {
        cout << "Gagal membuka file akun.\n";
        return;
    }
    for (const auto& a : akunList) {
        outFile << a.username << "," << a.password << "," << a.role << "\n";
    }
    outFile.close();
    cout << "Pegawai berhasil dipecat.\n";
}

void daftarPegawai() {
    ifstream file("akun.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file akun atau belum ada data.\n";
        return;
    }

    vector<Akun> akunList;
    Akun akun;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, akun.username, ',');
        getline(ss, akun.password, ',');
        getline(ss, akun.role);
        if (akun.role == "pegawai") { // Hanya tampilkan akun dengan role pegawai
            akunList.push_back(akun);
        }
    }
    file.close();

    if (akunList.empty()) {
        cout << "Belum ada pegawai terdaftar.\n";
        return;
    }

    cout << "\nDaftar Pegawai:\n";
    cout << setw(20) << "Username" << setw(15) << "Role" << endl;
    for (const auto& a : akunList) {
        cout << setw(20) << a.username << setw(15) << a.role << endl;
    }
}