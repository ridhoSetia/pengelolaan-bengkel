#define byte windows_byte
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <ctime>

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
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        throw runtime_error("Failed to get terminal attributes");
    }
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        throw runtime_error("Failed to set terminal attributes");
    }
    ch = getchar();
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0) {
        throw runtime_error("Failed to restore terminal attributes");
    }
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
        try {
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
        } catch (const exception& e) {
            cout << "Error handling input: " << e.what() << "\n";
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
}

void handleSubMenuInput(int &currentSelection, int menuSize) {
    while (true) {
        try {
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
        } catch (const exception& e) {
            cout << "Error handling submenu input: " << e.what() << "\n";
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
}

void menu_admin() {
    int pilihan;
    int currentSelection = 0;
    do {
        try {
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
        } catch (const exception& e) {
            cout << "Error in menu_admin: " << e.what() << "\n";
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (pilihan != 3);
}

void tampilkan_antrean() {
    try {
        ifstream file("bengkel.csv");
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file bengkel.csv. Pastikan file ada di direktori yang sama dengan program.");
        }

        vector<DataBengkel> antrean;
        string line;
        bool hasData = false;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            DataBengkel bengkel;
            getline(ss, bengkel.namaMotor, ',');
            if (bengkel.namaMotor.empty()) {
                bengkel.namaMotor = "N/A";
            }
            getline(ss, bengkel.noPlat, ',');
            if (bengkel.noPlat.empty()) {
                bengkel.noPlat = "N/A";
            }
            getline(ss, bengkel.noHp, ',');
            if (bengkel.noHp.empty()) {
                bengkel.noHp = "N/A";
            }
            string lama_servis_str;
            getline(ss, lama_servis_str, ',');
            try {
                if (!lama_servis_str.empty()) {
                    bengkel.lama_servis = stoi(lama_servis_str);
                    if (bengkel.lama_servis < 0) {
                        throw out_of_range("Lama servis tidak boleh negatif.");
                    }
                } else {
                    bengkel.lama_servis = 0;
                }
            } catch (const invalid_argument&) {
                bengkel.lama_servis = 0;
            } catch (const out_of_range&) {
                bengkel.lama_servis = 0;
            }
            getline(ss, bengkel.statusServis, ',');
            if (bengkel.statusServis.empty()) {
                bengkel.statusServis = "N/A";
            }
            getline(ss, bengkel.tglMasuk, ',');
            if (bengkel.tglMasuk.empty()) {
                bengkel.tglMasuk = "N/A";
            }
            antrean.push_back(bengkel);
            hasData = true;
        }
        file.close();

        if (!hasData) {
            cout << "Antrean kosong.\n";
            return;
        }

        cout << "+----+----------------------+------------+--------------+---------------+-----------------+---------------+\n";
        cout << "| No |      Nama Motor      | Plat Nomor |    No HP     |  Lama Servis  |  Status Servis  | Tanggal Masuk |\n";
        cout << "+----+----------------------+------------+--------------+---------------+-----------------+---------------+\n";

        int rowNumber = 1;
        for (const auto& m : antrean) {
            cout << "| " << setw(3) << left << rowNumber;
            cout << "| " << setw(21) << left << (m.namaMotor.empty() ? "N/A" : m.namaMotor);
            cout << "| " << setw(11) << left << (m.noPlat.empty() ? "N/A" : m.noPlat);
            cout << "| " << setw(13) << left << (m.noHp.empty() ? "N/A" : m.noHp);
            cout << "| " << setw(14) << left << (m.lama_servis ? to_string(m.lama_servis) + " jam" : "N/A");
            cout << "| " << setw(16) << left << (m.statusServis.empty() ? "N/A" : m.statusServis);
            cout << "| " << setw(14) << left << (m.tglMasuk.empty() ? "N/A" : m.tglMasuk);
            cout << "|\n";
            rowNumber++;
        }
        cout << "+----+----------------------+------------+--------------+---------------+-----------------+---------------+\n";
    } catch (const exception& e) {
        cout << "Error membaca antrean: " << e.what() << "\n";
    }
}

void tampilkan_laporan() {
    try {
        ifstream file("laporan.csv");
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file laporan.csv. Pastikan file ada di direktori yang sama dengan program.");
        }

        vector<DataBengkel> laporan;
        string line;
        bool hasData = false;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            DataBengkel bengkel;
            getline(ss, bengkel.namaMotor, ',');
            if (bengkel.namaMotor.empty()) {
                bengkel.namaMotor = "N/A";
            }
            getline(ss, bengkel.noPlat, ',');
            if (bengkel.noPlat.empty()) {
                bengkel.noPlat = "N/A";
            }
            getline(ss, bengkel.noHp, ',');
            if (bengkel.noHp.empty()) {
                bengkel.noHp = "N/A";
            }
            string lama_servis_str;
            getline(ss, lama_servis_str, ',');
            try {
                if (!lama_servis_str.empty()) {
                    bengkel.lama_servis = stoi(lama_servis_str);
                    if (bengkel.lama_servis < 0) {
                        throw out_of_range("Lama servis tidak boleh negatif.");
                    }
                } else {
                    bengkel.lama_servis = 0;
                }
            } catch (const invalid_argument&) {
                bengkel.lama_servis = 0;
            } catch (const out_of_range&) {
                bengkel.lama_servis = 0;
            }
            getline(ss, bengkel.statusServis, ',');
            if (bengkel.statusServis.empty()) {
                bengkel.statusServis = "N/A";
            }
            getline(ss, bengkel.tglMasuk, ',');
            if (bengkel.tglMasuk.empty()) {
                bengkel.tglMasuk = "N/A";
            }
            laporan.push_back(bengkel);
            hasData = true;
        }
        file.close();

        if (!hasData) {
            cout << "Laporan kosong.\n";
            return;
        }

        cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
        cout << "| No |      Nama Motor      | Plat Nomor |    No HP     | Lama Servis | Status Servis | Tanggal Masuk |\n";
        cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";

        int rowNumber = 1;
        for (const auto& m : laporan) {
            cout << "| " << setw(3) << left << rowNumber;
            cout << "| " << setw(21) << left << (m.namaMotor.empty() ? "N/A" : m.namaMotor);
            cout << "| " << setw(11) << left << (m.noPlat.empty() ? "N/A" : m.noPlat);
            cout << "| " << setw(13) << left << (m.noHp.empty() ? "N/A" : m.noHp);
            cout << "| " << setw(12) << left << (m.lama_servis ? to_string(m.lama_servis) + " jam" : "N/A");
            cout << "| " << setw(14) << left << (m.statusServis.empty() ? "N/A" : m.statusServis);
            cout << "| " << setw(14) << left << (m.tglMasuk.empty() ? "N/A" : m.tglMasuk);
            cout << "|\n";
            rowNumber++;
        }
        cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
    } catch (const exception& e) {
        cout << "Error membaca laporan: " << e.what() << "\n";
    }
}

void rekrutPegawai() {
    try {
        Akun akun;
        cout << "\nMasukkan username baru: ";
        if (!getline(cin, akun.username)) {
            throw runtime_error("Gagal membaca username.");
        }
        if (akun.username.empty() || akun.username.find(',') != string::npos) {
            throw runtime_error("Username tidak boleh kosong atau mengandung koma.");
        }
        cout << "Masukkan password baru: ";
        if (!getline(cin, akun.password)) {
            throw runtime_error("Gagal membaca password.");
        }
        if (akun.password.empty() || akun.password.find(',') != string::npos) {
            throw runtime_error("Password tidak boleh kosong atau mengandung koma.");
        }
        akun.role = "pegawai";

        ifstream file("akun.csv");
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file akun.csv untuk membaca.");
        }

        string line;
        bool usernameExists = false;
        while (getline(file, line)) {
            stringstream ss(line);
            string existingUsername;
            if (!getline(ss, existingUsername, ',')) continue;
            if (existingUsername == akun.username) {
                usernameExists = true;
                break;
            }
        }
        file.close();

        if (usernameExists) {
            throw runtime_error("Username sudah digunakan. Silakan coba username lain.");
        }

        ofstream outFile("akun.csv", ios::app);
        if (!outFile.is_open()) {
            throw runtime_error("Gagal membuka file akun.csv untuk menulis.");
        }
        outFile << akun.username << "," << akun.password << "," << akun.role << "\n";
        outFile.close();
        cout << "Pegawai berhasil direkrut.\n";
    } catch (const exception& e) {
        cout << "Error merekrut pegawai: " << e.what() << "\n";
    }
}

void pecatPegawai() {
    try {
        string username;
        cout << "\nMasukkan username pegawai yang akan dipecat: ";
        if (!getline(cin, username)) {
            throw runtime_error("Gagal membaca username.");
        }
        if (username.empty() || username.find(',') != string::npos) {
            throw runtime_error("Username tidak boleh kosong atau mengandung koma.");
        }

        ifstream file("akun.csv");
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file akun.csv atau belum ada data.");
        }

        vector<Akun> akunList;
        Akun akun;
        string line;
        bool found = false;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            if (!getline(ss, akun.username, ',')) continue;
            if (!getline(ss, akun.password, ',')) continue;
            if (!getline(ss, akun.role)) continue;
            if (akun.username == username && akun.role == "pegawai") {
                found = true;
            } else {
                akunList.push_back(akun);
            }
        }
        file.close();

        if (!found) {
            throw runtime_error("Pegawai dengan username tersebut tidak ditemukan atau bukan pegawai.");
        }

        ofstream outFile("akun.csv");
        if (!outFile.is_open()) {
            throw runtime_error("Gagal membuka file akun.csv untuk menulis.");
        }
        for (const auto& a : akunList) {
            outFile << a.username << "," << a.password << "," << a.role << "\n";
        }
        outFile.close();
        cout << "Pegawai berhasil dipecat.\n";
    } catch (const exception& e) {
        cout << "Error memecat pegawai: " << e.what() << "\n";
    }
}

void daftarPegawai() {
    try {
        ifstream file("akun.csv");
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file akun.csv atau belum ada data.");
        }

        vector<Akun> akunList;
        Akun akun;
        string line;
        bool hasData = false;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            if (!getline(ss, akun.username, ',')) {
                continue;
            }
            if (!getline(ss, akun.password, ',')) {
                continue;
            }
            if (!getline(ss, akun.role)) {
                continue;
            }
            if (akun.role == "pegawai") {
                akunList.push_back(akun);
                hasData = true;
            }
        }
        file.close();

        if (!hasData) {
            cout << "Belum ada pegawai terdaftar.\n";
            return;
        }

        cout << "+----+----------------------+---------------+\n";
        cout << "| No |      Username        |     Role      |\n";
        cout << "+----+----------------------+---------------+\n";

        int rowNumber = 1;
        for (const auto& a : akunList) {
            cout << "| " << setw(3) << left << rowNumber;
            cout << "| " << setw(21) << left << (a.username.empty() ? "N/A" : a.username);
            cout << "| " << setw(14) << left << (a.role.empty() ? "N/A" : a.role);
            cout << "|\n";
            rowNumber++;
        }
        cout << "+----+----------------------+---------------+\n";
    } catch (const exception& e) {
        cout << "Error menampilkan daftar pegawai: " << e.what() << "\n";
    }
}