#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>
#include <vector>

using namespace std;

struct DataBengkel {
    string namaMotor;
    string noPlat;
    string noHp;
    int lama_servis;
    string statusServis;
    string tglMasuk;
};

struct Akun {
    string username;
    string password;
    string role; // "admin" or "pegawai"
};

void tampilkan_antrean();
void tampilkan_laporan();
void rekrutPegawai();
void pecatPegawai();
void daftarPegawai();

void menu_admin() {
    int pilihan;
    do {
        cout << "\n=== Menu Admin ===\n";
        cout << "1. Lihat Antrean\n";
        cout << "2. Lihat Laporan\n";
        cout << "3. Manajemen Akun Pegawai\n";
        cout << "0. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (pilihan) {
            case 1:
                tampilkan_antrean();
                break;
            case 2:
                tampilkan_laporan();
                break;
            case 3: {
                int subPilihan;
                do {
                    cout << "\n=== Manajemen Akun Pegawai ===\n";
                    cout << "1. Rekrut Pegawai\n";
                    cout << "2. Pecat Pegawai\n";
                    cout << "3. Daftar Pegawai\n";
                    cout << "0. Kembali\n";
                    cout << "Pilih: ";
                    cin >> subPilihan;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    switch (subPilihan) {
                        case 1:
                            rekrutPegawai();
                            break;
                        case 2:
                            pecatPegawai();
                            break;
                        case 3:
                            daftarPegawai();
                            break;
                        case 0:
                            break;
                        default:
                            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                    }
                } while (subPilihan != 0);
                break;
            }
            case 0:
                cout << "Keluar dari menu admin.\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    } while (pilihan != 0);
}

void tampilkan_antrean() {
    ifstream file("data.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file antrean atau belum ada data.\n";
        return;
    }

    vector<DataBengkel> antrean;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        DataBengkel bengkel;
        getline(ss, bengkel.namaMotor, ',');
        getline(ss, bengkel.noPlat, ',');
        getline(ss, bengkel.noHp, ',');
        string lama_servis_str;
        getline(ss, lama_servis_str, ',');
        try {
            bengkel.lama_servis = stoi(lama_servis_str);
        } catch (...) {
            bengkel.lama_servis = 0; // Default value if conversion fails
        }
        getline(ss, bengkel.statusServis, ',');
        getline(ss, bengkel.tglMasuk, ',');
        antrean.push_back(bengkel);
    }
    file.close();

    if (antrean.empty()) {
        cout << "Antrean kosong.\n";
        return;
    }

    cout << "\n=== Daftar Antrean Bengkel ===\n";
    cout << setw(20) << left << "Nama Motor" 
         << setw(15) << "No. Plat" 
         << setw(15) << "No. HP" 
         << setw(10) << "Lama Servis" 
         << setw(15) << "Status" 
         << setw(15) << "Tgl Masuk" << endl;
    cout << string(90, '-') << endl;
    for (const auto& m : antrean) {
        cout << setw(20) << left << m.namaMotor 
             << setw(15) << m.noPlat 
             << setw(15) << m.noHp 
             << setw(10) << m.lama_servis 
             << setw(15) << m.statusServis 
             << setw(15) << m.tglMasuk << endl;
    }
}

void tampilkan_laporan() {
    ifstream file("laporan.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file laporan atau belum ada data.\n";
        return;
    }

    vector<DataBengkel> laporan;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        DataBengkel bengkel;
        getline(ss, bengkel.namaMotor, ',');
        getline(ss, bengkel.noPlat, ',');
        getline(ss, bengkel.noHp, ',');
        string lama_servis_str;
        getline(ss, lama_servis_str, ',');
        try {
            bengkel.lama_servis = stoi(lama_servis_str);
        } catch (...) {
            bengkel.lama_servis = 0; // Default value if conversion fails
        }
        getline(ss, bengkel.statusServis, ',');
        getline(ss, bengkel.tglMasuk, ',');
        laporan.push_back(bengkel);
    }
    file.close();

    if (laporan.empty()) {
        cout << "Laporan kosong.\n";
        return;
    }

    cout << "\n=== Lapor Servis Bengkel ===\n";
    cout << setw(20) << left << "Nama Motor" 
         << setw(15) << "No. Plat" 
         << setw(15) << "No. HP" 
         << setw(10) << "Lama Servis" 
         << setw(15) << "Status" 
         << setw(15) << "Tgl Masuk" << endl;
    cout << string(90, '-') << endl;
    for (const auto& m : laporan) {
        cout << setw(20) << left << m.namaMotor 
             << setw(15) << m.noPlat 
             << setw(15) << m.noHp 
             << setw(10) << m.lama_servis 
             << setw(15) << m.statusServis 
             << setw(15) << m.tglMasuk << endl;
    }
}

void rekrutPegawai() {
    Akun akun;
    cout << "\nMasukkan username baru: ";
    getline(cin, akun.username);
    cout << "Masukkan password baru: ";
    getline(cin, akun.password);
    akun.role = "pegawai";

    // Check if username already exists
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

    // Save new account to file
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

    // Read all accounts from file
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
            found = true;
        } else {
            akunList.push_back(akun);
        }
    }
    file.close();

    if (!found) {
        cout << "Pegawai dengan username tersebut tidak ditemukan atau bukan pegawai.\n";
        return;
    }

    // Write back accounts excluding the deleted one
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
        if (akun.role == "pegawai") {
            akunList.push_back(akun);
        }
    }
    file.close();

    if (akunList.empty()) {
        cout << "Belum ada pegawai terdaftar.\n";
        return;
    }

    cout << "\n=== Daftar Pegawai ===\n";
    cout << setw(20) << left << "Username" << setw(15) << "Role" << endl;
    cout << string(35, '-') << endl;
    for (const auto& a : akunList) {
        cout << setw(20) << left << a.username << setw(15) << a.role << endl;
    }
}