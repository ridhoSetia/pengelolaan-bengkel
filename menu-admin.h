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
#include <algorithm>

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

// Deklarasi fungsi
void tampilkan_antrean();
void tampilkan_laporan();
void rekrutPegawai();
void pecatPegawai();
void daftarPegawai();
void handleInput(int &currentSelection, const char *menuItems[], int menuSize);
void displayMenu(const char *menuItems[], int menuSize, int currentSelection);
void clearTerminal();

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

const char *sortMenuItems[] = {
    "1. A-Z (ascending)",
    "2. Z-A (descending)"
};
const int sortMenuSize = sizeof(sortMenuItems) / sizeof(sortMenuItems[0]);

// Fungsi utilitas baru untuk menangani error
void handle_error(const string& pesan, const exception& e) {
    cout << pesan << ": " << e.what() << "\n";
    cout << "Tekan enter untuk kembali\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

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

// Fungsi Buat Tampilan Pada Terminal
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

void handleInput(int &currentSelection, const char *menuItems[], int menuSize) {
    while (true) {
        try {
            displayMenu(menuItems, menuSize, currentSelection);
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
            handle_error("Error handling input", e);
            break;
        }
    }
}

// Fungsi utilitas untuk membaca data CSV
vector<DataBengkel> baca_data_csv(const string& namaFile) {
    vector<DataBengkel> data;
    try {
        ifstream file(namaFile);
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file " + namaFile + ". Pastikan file ada di direktori yang sama dengan program.");
        }

        string line;
        bool hasData = false;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) continue;
            stringstream ss(line);
            DataBengkel bengkel;
            getline(ss, bengkel.namaMotor, ',');
            if (bengkel.namaMotor.empty()) bengkel.namaMotor = "N/A";
            getline(ss, bengkel.noPlat, ',');
            if (bengkel.noPlat.empty()) bengkel.noPlat = "N/A";
            getline(ss, bengkel.noHp, ',');
            if (bengkel.noHp.empty()) bengkel.noHp = "N/A";
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
            if (bengkel.statusServis.empty()) bengkel.statusServis = "N/A";
            getline(ss, bengkel.tglMasuk, ',');
            if (bengkel.tglMasuk.empty()) bengkel.tglMasuk = "N/A";
            data.push_back(bengkel);
            hasData = true;
        }
        file.close();
        if (!hasData) {
            cout << "Data kosong.\n";
        }
    } catch (const exception& e) {
        handle_error("Error membaca data", e);
    }
    return data;
}

vector<Akun> baca_akun_csv(const string& namaFile) {
    vector<Akun> akunList;
    try {
        ifstream file(namaFile);
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file " + namaFile + ".");
        }
        string line;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) continue;
            stringstream ss(line);
            Akun akun;
            if (!getline(ss, akun.username, ',')) continue;
            if (!getline(ss, akun.password, ',')) continue;
            if (!getline(ss, akun.role)) continue;
            akunList.push_back(akun);
        }
        file.close();
    } catch (const exception& e) {
        handle_error("Error membaca file akun", e);
    }
    return akunList;
}

// Fungsi utilitas untuk menulis akun ke CSV
void tulis_akun_csv(const string& namaFile, const vector<Akun>& akunList) {
    try {
        ofstream outFile(namaFile);
        if (!outFile.is_open()) {
            throw runtime_error("Gagal membuka file " + namaFile + " untuk menulis.");
        }
        for (const auto& a : akunList) {
            outFile << a.username << "," << a.password << "," << a.role << "\n";
        }
        outFile.close();
    } catch (const exception& e) {
        handle_error("Error menulis file akun", e);
    }
}

// Fungsi utilitas untuk menampilkan tabel
void tampilkan_data_csv(const string& namaFile, const string& judul, int lebarLamaServis) {
    vector<DataBengkel> data = baca_data_csv(namaFile);
    if (data.empty()) {
        cout << judul << " kosong.\n";
        return;
    }

    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
    cout << "| No |      Nama Motor      | Plat Nomor |    No HP     | Lama Servis | Status Servis | Tanggal Masuk |\n";
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";

    int rowNumber = 1;
    for (const auto& m : data) {
        cout << "| " << setw(3) << left << rowNumber;
        cout << "| " << setw(21) << left << (m.namaMotor.empty() ? "N/A" : m.namaMotor);
        cout << "| " << setw(11) << left << (m.noPlat.empty() ? "N/A" : m.noPlat);
        cout << "| " << setw(13) << left << (m.noHp.empty() ? "N/A" : m.noHp);
        cout << "| " << setw(lebarLamaServis) << left << (m.lama_servis ? to_string(m.lama_servis) + " jam" : "N/A");
        cout << "| " << setw(14) << left << (m.statusServis.empty() ? "N/A" : m.statusServis);
        cout << "| " << setw(14) << left << (m.tglMasuk.empty() ? "N/A" : m.tglMasuk);
        cout << "|\n";
        rowNumber++;
    }
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+\n";
}

// Crud Admin
void menu_admin() {
    int pilihan;
    int currentSelection = 0;
    do {
        try {
            handleInput(currentSelection, adminMenuItems, adminMenuSize);
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
                        handleInput(subCurrentSelection, subMenuItems, subMenuSize);
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
            handle_error("Error in menu_admin", e);
        }
    } while (pilihan != 3);
}

void tampilkan_antrean() {
    tampilkan_data_csv("bengkel.csv", "Antrean", 12);
}

void tampilkan_laporan() {
    tampilkan_data_csv("laporan.csv", "Laporan", 12);
}

void rekrutPegawai() {
    try {
        Akun akun;
        cout << "\nMasukkan username baru: ";
        if (!getline(cin, akun.username) || akun.username.empty() || akun.username.find(',') != string::npos) {
            throw runtime_error("Username tidak boleh kosong atau mengandung koma.");
        }
        cout << "Masukkan password baru: ";
        if (!getline(cin, akun.password) || akun.password.empty() || akun.password.find(',') != string::npos) {
            throw runtime_error("Password tidak boleh kosong atau mengandung koma.");
        }
        akun.role = "pegawai";

        vector<Akun> akunList = baca_akun_csv("akun.csv");
        for (const auto& a : akunList) {
            if (a.username == akun.username) {
                throw runtime_error("Username sudah digunakan. Silakan coba username lain.");
            }
        }
        akunList.push_back(akun);
        tulis_akun_csv("akun.csv", akunList);
        cout << "Pegawai berhasil direkrut.\n";
    } catch (const exception& e) {
        handle_error("Error merekrut pegawai", e);
    }
}

void pecatPegawai() {
    try {
        string keyword;
        cout << "\nMasukkan kata kunci untuk mencari username pegawai: ";
        if (!getline(cin, keyword)) {
            throw runtime_error("Gagal membaca kata kunci.");
        }

        vector<Akun> akunList = baca_akun_csv("akun.csv");
        vector<Akun> filteredList;
        bool hasData = false;
        for (const auto& akun : akunList) {
            if (akun.role == "pegawai") {
                string usernameLower = akun.username;
                string keywordLower = keyword;
                transform(usernameLower.begin(), usernameLower.end(), usernameLower.begin(), ::tolower);
                transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);
                if (keyword.empty() || usernameLower.find(keywordLower) != string::npos) {
                    filteredList.push_back(akun);
                    hasData = true;
                }
            }
        }

        if (!hasData) {
            cout << "Tidak ada pegawai yang cocok dengan kata kunci '" << keyword << "'.\n";
            return;
        }

        cout << "\nHasil pencarian:\n";
        cout << "+----+----------------------+---------------+\n";
        cout << "| No |      Username        |     Role      |\n";
        cout << "+----+----------------------+---------------+\n";
        int rowNumber = 1;
        for (const auto& a : filteredList) {
            cout << "| " << setw(3) << left << rowNumber;
            cout << "| " << setw(21) << left << (a.username.empty() ? "N/A" : a.username);
            cout << "| " << setw(14) << left << (a.role.empty() ? "N/A" : a.role);
            cout << "|\n";
            rowNumber++;
        }
        cout << "+----+----------------------+---------------+\n";

        cout << "Masukkan nomor pegawai yang akan dipecat (1-" << filteredList.size() << "): ";
        string input;
        if (!getline(cin, input)) {
            throw runtime_error("Gagal membaca nomor pilihan.");
        }
        int choice;
        try {
            choice = stoi(input);
        } catch (const invalid_argument&) {
            throw runtime_error("Input harus berupa angka.");
        }
        if (choice < 1 || choice > static_cast<int>(filteredList.size())) {
            throw runtime_error("Nomor pilihan tidak valid.");
        }

        string usernameToDelete = filteredList[choice - 1].username;
        cout << "Apakah Anda yakin ingin memecat pegawai dengan username '" << usernameToDelete << "'? (y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (tolower(confirm) != 'y') {
            cout << "Pemecatan dibatalkan.\n";
            return;
        }

        akunList.erase(
            remove_if(akunList.begin(), akunList.end(),
                [&usernameToDelete](const Akun& a) { return a.username == usernameToDelete && a.role == "pegawai"; }),
            akunList.end()
        );

        tulis_akun_csv("akun.csv", akunList);
        cout << "Pegawai dengan username '" << usernameToDelete << "' berhasil dipecat.\n";
    } catch (const exception& e) {
        handle_error("Error memecat pegawai", e);
    }
}

void daftarPegawai() {
    try {
        vector<Akun> akunList = baca_akun_csv("akun.csv");
        vector<Akun> pegawaiList;
        bool hasData = false;
        for (const auto& akun : akunList) {
            if (akun.role == "pegawai") {
                pegawaiList.push_back(akun);
                hasData = true;
            }
        }

        if (!hasData) {
            cout << "Belum ada pegawai terdaftar.\n";
            return;
        }

        cout << "\nPilih urutan pengurutan:\n";
        int sortSelection = 0;
        handleInput(sortSelection, sortMenuItems, sortMenuSize);
        int sortChoice = sortSelection + 1;

        if (sortChoice == 1) {
            sort(pegawaiList.begin(), pegawaiList.end(),
                 [](const Akun& a, const Akun& b) {
                     string usernameA = a.username;
                     string usernameB = b.username;
                     transform(usernameA.begin(), usernameA.end(), usernameA.begin(), ::tolower);
                     transform(usernameB.begin(), usernameB.end(), usernameB.begin(), ::tolower);
                     return usernameA < usernameB;
                 });
        } else if (sortChoice == 2) {
            sort(pegawaiList.begin(), pegawaiList.end(),
                 [](const Akun& a, const Akun& b) {
                     string usernameA = a.username;
                     string usernameB = b.username;
                     transform(usernameA.begin(), usernameA.end(), usernameA.begin(), ::tolower);
                     transform(usernameB.begin(), usernameB.end(), usernameB.begin(), ::tolower);
                     return usernameA > usernameB;
                 });
        }

        clearTerminal();
        cout << "\nDaftar Pegawai:\n";
        cout << "+----+----------------------+---------------+\n";
        cout << "| No |      Username        |     Role      |\n";
        cout << "+----+----------------------+---------------+\n";

        int rowNumber = 1;
        for (const auto& a : pegawaiList) {
            cout << "| " << setw(3) << left << rowNumber;
            cout << "| " << setw(21) << left << (a.username.empty() ? "N/A" : a.username);
            cout << "| " << setw(14) << left << (a.role.empty() ? "N/A" : a.role);
            cout << "|\n";
            rowNumber++;
        }
        cout << "+----+----------------------+---------------+\n";
    } catch (const exception& e) {
        handle_error("Error menampilkan daftar pegawai", e);
    }
}