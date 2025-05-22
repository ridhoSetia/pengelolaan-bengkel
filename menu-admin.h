#define byte windows_byte
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#undef byte
#else
#include <termios.h>
#include <unistd.h>
#endif

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

const char *adminMenuItems[] = {
    "1. Lihat Antrean",
    "2. Lihat Laporan",
    "3. Manajemen Akun Pegawai",
    "0. Keluar"
};
const int adminMenuSize = sizeof(adminMenuItems) / sizeof(adminMenuItems[0]);

const char *subMenuItems[] = {
    "1. Rekrut Pegawai",
    "2. Pecat Pegawai",
    "3. Daftar Pegawai",
    "0. Kembali"
};
const int subMenuSize = sizeof(subMenuItems) / sizeof(subMenuItems[0]);

#ifdef _WIN32
void clearTerminal() {
    system("cls");
}
#else
int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
void clearTerminal() {
    system("clear");
}
#endif

void displayMenu(const char *menuItems[], int menuSize, int currentSelection) {
    clearTerminal();
    cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
    for (int i = 0; i < menuSize; i++) {
        if (i == currentSelection)
            cout << " -> " << menuItems[i] << "\n";
        else
            cout << "     " << menuItems[i] << "\n";
    }
}

void handleInput(int &currentSelection, int menuSize) {
    while (true) {
        displayMenu(adminMenuItems, adminMenuSize, currentSelection);
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
        if (key == 27) {
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

void handleSubMenuInput(int &currentSelection, int menuSize) {
    while (true) {
        displayMenu(subMenuItems, subMenuSize, currentSelection);
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
        if (key == 27) {
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

void menu_admin() {
    int pilihan;
    int currentSelection = 0;
    do {
        handleInput(currentSelection, adminMenuSize);
        pilihan = currentSelection;
        clearTerminal();

        switch (pilihan) {
            case 0: // Lihat Antrean
                tampilkan_antrean();
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            case 1: // Lihat Laporan
                tampilkan_laporan();
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            case 2: { // Manajemen Akun Pegawai
                int subPilihan;
                int subCurrentSelection = 0;
                do {
                    handleSubMenuInput(subCurrentSelection, subMenuSize);
                    subPilihan = subCurrentSelection;
                    clearTerminal();

                    switch (subPilihan) {
                        case 0: // Rekrut Pegawai
                            rekrutPegawai();
                            cout << "Tekan enter untuk kembali\n";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            break;
                        case 1: // Pecat Pegawai
                            pecatPegawai();
                            cout << "Tekan enter untuk kembali\n";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            break;
                        case 2: // Daftar Pegawai
                            daftarPegawai();
                            cout << "Tekan enter untuk kembali\n";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            break;
                        case 3: // Kembali
                            break;
                        default:
                            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                            cout << "Tekan enter untuk kembali\n";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                } while (subPilihan != 3);
                break;
            }
            case 3: // Keluar
                cout << "Keluar dari menu admin.\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (pilihan != 3);
}

void tampilkan_antrean() {
    ifstream file("bengkel.csv");
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
            bengkel.lama_servis = 0;
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

    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
    cout << "| No |      Nama Motor      | Plat Nomor |    No HP     | Lama Servis | Status Servis | Tanggal Masuk |\n";
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";

    int rowNumber = 1;
    for (const auto& m : antrean) {
        cout << "| " << setw(3) << left << rowNumber;
        cout << "| " << setw(21) << left << m.namaMotor;
        cout << "| " << setw(11) << left << m.noPlat;
        cout << "| " << setw(13) << left << m.noHp;
        cout << "| " << setw(12) << left << (m.lama_servis ? to_string(m.lama_servis) + " jam" : "");
        cout << "| " << setw(14) << left << m.statusServis;
        cout << "| " << setw(14) << left << m.tglMasuk;
        cout << "|\n";
        rowNumber++;
    }
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
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
            bengkel.lama_servis = 0;
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

    // Header tabel
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
    cout << "| No |      Nama Motor      | Plat Nomor |    No HP     | Lama Servis | Status Servis | Tanggal Masuk |\n";
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";

    int rowNumber = 1;
    for (const auto& m : laporan) {
        cout << "| " << setw(3) << left << rowNumber;
        cout << "| " << setw(21) << left << m.namaMotor;
        cout << "| " << setw(11) << left << m.noPlat;
        cout << "| " << setw(13) << left << m.noHp;
        cout << "| " << setw(12) << left << (m.lama_servis ? to_string(m.lama_servis) + " jam" : "");
        cout << "| " << setw(14) << left << m.statusServis;
        cout << "| " << setw(14) << left << m.tglMasuk;
        cout << "|\n";
        rowNumber++;
    }
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
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

    // Header tabel
    cout << "+----+----------------------+---------------+\n";
    cout << "| No |      Username        |     Role      |\n";
    cout << "+----+----------------------+---------------+\n";

    int rowNumber = 1;
    for (const auto& a : akunList) {
        cout << "| " << setw(3) << left << rowNumber;
        cout << "| " << setw(21) << left << a.username;
        cout << "| " << setw(14) << left << a.role;
        cout << "|\n";
        rowNumber++;
    }
    cout << "+----+----------------------+---------------+\n";
}