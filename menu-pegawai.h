#define byte windows_byte
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <limits>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#undef byte
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

struct PaketServis {
    int id;
    string namaPaket;
    int lamaServis;
};

void hapusDataMotor(const string &filename, int rowToDelete);
string tanggalMasuk();

const char *menuItems[] = {
    "1. Tambah Motor",
    "2. Lihat Antrean",
    "3. Servis Motor",
    "4. Hapus Motor",
    "0. Keluar"
};
const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

int currentSelection = 0;

#ifdef _WIN32
void clearScreen() {
    system("cls");
}
#else
extern int _getch();

void clearScreen() {
    system("clear");
}
#endif

bool AdaDalamTeks(const string &teks, char yangDicari) {
    return teks.find(yangDicari) != string::npos;
}

string tanggalMasuk() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

void displayMenu() {
    clearScreen();
    cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
    for (int i = 0; i < menuSize; i++) {
        if (i == currentSelection)
            cout << " -> " << menuItems[i] << "\n";
        else
            cout << "     " << menuItems[i] << "\n";
    }
}

void handleInput() {
    while (true) {
        displayMenu();
        int key = _getch();

#ifdef _WIN32
        if (key == 224) {
            int arrow = _getch();
            if (arrow == 72) {
                currentSelection = (currentSelection - 1 + menuSize) % menuSize;
            } else if (arrow == 80) {
                currentSelection = (currentSelection + 1) % menuSize;
            }
        } else if (key == 13) {
            break;
        }
#else
        if (key == 27) {
            int second = _getch();
            if (second == 91) {
                int arrow = _getch();
                if (arrow == 65) {
                    currentSelection = (currentSelection - 1 + menuSize) % menuSize;
                } else if (arrow == 66) {
                    currentSelection = (currentSelection + 1) % menuSize;
                }
            }
        } else if (key == 10) {
            break;
        }
#endif
    }
}

bool isCSVKosong(const string &filename) {
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            return true;
        }
        string line;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (!line.empty()) {
                return false;
            }
        }
        return true;
    } catch (const exception &e) {
        cout << "Error checking CSV: " << e.what() << "\n";
        return true;
    }
}

void tambahMotor(const string &filename) {
    try {
        ofstream data(filename, ios::app);
        if (!data.is_open()) {
            throw runtime_error("Gagal membuka file: " + filename);
        }

        PaketServis paket[] = {
            {1, "Paket servis ringan", 3},
            {2, "Paket servis lengkap", 10},
            {3, "Paket servis berat", 6}
        };

        DataBengkel motor;
        cout << "Masukkan nama motor: ";
        if (!getline(cin, motor.namaMotor)) {
            throw runtime_error("Gagal membaca nama motor.");
        }
        if (motor.namaMotor.empty() || motor.namaMotor.find(',') != string::npos) {
            throw runtime_error("Nama motor tidak boleh kosong atau mengandung koma.");
        }
        cout << "Masukkan nomor plat motor: ";
        if (!getline(cin, motor.noPlat)) {
            throw runtime_error("Gagal membaca nomor plat.");
        }
        if (motor.noPlat.empty() || motor.noPlat.find(',') != string::npos) {
            throw runtime_error("Nomor plat tidak boleh kosong atau mengandung koma.");
        }
        cout << "Masukkan nomor hp: ";
        if (!getline(cin, motor.noHp)) {
            throw runtime_error("Gagal membaca nomor HP.");
        }
        if (motor.noHp.empty() || motor.noHp.find(',') != string::npos) {
            throw runtime_error("Nomor HP tidak boleh kosong atau mengandung koma.");
        }
        for (char c : motor.noHp) {
            if (!isdigit(c)) {
                throw runtime_error("Nomor HP hanya boleh terdiri dari angka.");
            }
        }

        cout << "Paket Servis Bengkel Mas Tahir\n";
        for (int i = 0; i < 3; ++i) {
            cout << paket[i].id << ". " << paket[i].namaPaket << endl;
        }

        string input;
        int pilihPaket;
        cout << "Pilih paket (1-3): ";
        getline(cin, input);

        if (input.empty()) {
            throw runtime_error("Input tidak boleh kosong.");
        }

        for (char c : input) {
            if (!isdigit(c)) {
                throw runtime_error("Input harus berupa angka.");
            }
        }

        pilihPaket = stoi(input);
        if (pilihPaket >= 1 && pilihPaket <= 3) {
            motor.lama_servis = paket[pilihPaket - 1].lamaServis;
            motor.statusServis = "Menunggu";
        } else {
            throw runtime_error("Pilihan paket tidak valid!");
        }

        motor.tglMasuk = tanggalMasuk();

        data << motor.namaMotor << ","
             << motor.noPlat << ","
             << motor.noHp << ","
             << motor.lama_servis << ","
             << motor.statusServis << ","
             << motor.tglMasuk << "\n";

        data.close();
        cout << "Data motor berhasil ditambahkan.\n";
    } catch (const exception &e) {
        cout << "Error menambah motor: " << e.what() << "\n";
    }
}

void tabelAntrean(const string &filename) {
    try {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file: " + filename);
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
            } catch (const invalid_argument &) {
                bengkel.lama_servis = 0;
            } catch (const out_of_range &) {
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
        for (const auto &m : antrean) {
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
    } catch (const exception &e) {
        cout << "Error membaca antrean: " << e.what() << "\n";
    }
}

void servisMotor(const string &filename) {
    try {
        if (isCSVKosong(filename)) {
            clearScreen();
            cout << "Antrean kosong.\n";
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        string mulai;
        do {
            clearScreen();
            tabelAntrean(filename);

            string input;
            int nomorAntrean;
            cout << "Masukkan nomor antrean motor yang akan diservis (0 untuk kembali): ";
            getline(cin, input);

            if (input.empty()) {
                cout << "Input tidak boleh kosong.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            bool isValidNumber = true;
            for (char c : input) {
                if (!isdigit(c)) {
                    isValidNumber = false;
                    break;
                }
            }

            if (!isValidNumber) {
                cout << "Input harus berupa angka.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            nomorAntrean = stoi(input);
            if (nomorAntrean == 0) {
                clearScreen();
                cout << "Proses servis motor dihentikan.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            ifstream inFile(filename);
            if (!inFile.is_open()) {
                throw runtime_error("Gagal membuka file: " + filename);
            }

            vector<DataBengkel> antrean;
            string line;
            while (getline(inFile, line)) {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);
                if (line.empty()) {
                    continue;
                }
                stringstream ss(line);
                DataBengkel bengkel;
                getline(ss, bengkel.namaMotor, ',');
                getline(ss, bengkel.noPlat, ',');
                getline(ss, bengkel.noHp, ',');
                string lama_servis_str;
                getline(ss, lama_servis_str, ',');
                try {
                    bengkel.lama_servis = lama_servis_str.empty() ? 0 : stoi(lama_servis_str);
                } catch (...) {
                    bengkel.lama_servis = 0;
                }
                getline(ss, bengkel.statusServis, ',');
                getline(ss, bengkel.tglMasuk, ',');
                antrean.push_back(bengkel);
            }
            inFile.close();

            if (nomorAntrean < 1 || nomorAntrean > antrean.size()) {
                clearScreen();
                cout << "Nomor antrean tidak valid.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            DataBengkel &bengkel = antrean[nomorAntrean - 1];

            clearScreen();
            cout << "Proses servis motor " << bengkel.namaMotor << " sedang dilakukan\n";
            bengkel.statusServis = "Dikerjakan";

            ofstream tempFile("temp.csv");
            if (!tempFile.is_open()) {
                throw runtime_error("Gagal membuka file temp.csv.");
            }
            for (const auto &b : antrean) {
                tempFile << b.namaMotor << "," << b.noPlat << "," << b.noHp << ","
                         << b.lama_servis << "," << b.statusServis << "," << b.tglMasuk << "\n";
            }
            tempFile.close();
            remove(filename.c_str());
            rename("temp.csv", filename.c_str());

            this_thread::sleep_for(chrono::seconds(2)); // Simulate servicing time

            clearScreen();
            cout << "Motor " << bengkel.namaMotor << " telah selesai diservis\n";
            bengkel.statusServis = "Selesai";

            ofstream laporanFile("laporan.csv", ios::app);
            if (!laporanFile.is_open()) {
                throw runtime_error("Gagal membuka file laporan.csv.");
            }
            laporanFile << bengkel.namaMotor << "," << bengkel.noPlat << "," << bengkel.noHp << ","
                        << bengkel.lama_servis << "," << bengkel.statusServis << "," << bengkel.tglMasuk << "\n";
            laporanFile.close();

            antrean.erase(antrean.begin() + (nomorAntrean - 1));

            tempFile.open("temp.csv");
            if (!tempFile.is_open()) {
                throw runtime_error("Gagal membuka file temp.csv.");
            }
            for (const auto &b : antrean) {
                tempFile << b.namaMotor << "," << b.noPlat << "," << b.noHp << ","
                         << b.lama_servis << "," << b.statusServis << "," << b.tglMasuk << "\n";
            }
            tempFile.close();
            remove(filename.c_str());
            rename("temp.csv", filename.c_str());

            clearScreen();
            cout << "Antrean telah diperbarui.\n";
            tabelAntrean(filename);

            if (isCSVKosong(filename)) {
                cout << "Tidak ada antrean motor lagi.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            cout << "Lanjutkan servis motor lain (y/t)? : ";
            getline(cin, mulai);
            if (mulai != "y" && mulai != "Y") {
                break;
            }
        } while (!isCSVKosong(filename));
    } catch (const exception &e) {
        cout << "Error selama servis motor: " << e.what() << "\n";
        cout << "Tekan enter untuk kembali\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void hapusDataMotor(const string &filename, int rowToDelete) {
    try {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw runtime_error("Gagal membuka file: " + filename);
        }

        vector<DataBengkel> antrean;
        string line;
        while (getline(inFile, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            DataBengkel bengkel;
            getline(ss, bengkel.namaMotor, ',');
            getline(ss, bengkel.noPlat, ',');
            getline(ss, bengkel.noHp, ',');
            string lama_servis_str;
            getline(ss, lama_servis_str, ',');
            try {
                bengkel.lama_servis = lama_servis_str.empty() ? 0 : stoi(lama_servis_str);
            } catch (...) {
                bengkel.lama_servis = 0;
            }
            getline(ss, bengkel.statusServis, ',');
            getline(ss, bengkel.tglMasuk, ',');
            antrean.push_back(bengkel);
        }
        inFile.close();

        if (rowToDelete < 0 || rowToDelete >= antrean.size()) {
            throw runtime_error("Nomor antrean tidak valid.");
        }

        antrean.erase(antrean.begin() + rowToDelete);

        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Gagal membuka file: " + filename);
        }
        for (const auto &b : antrean) {
            outFile << b.namaMotor << "," << b.noPlat << "," << b.noHp << ","
                    << b.lama_servis << "," << b.statusServis << "," << b.tglMasuk << "\n";
        }
        outFile.close();
        cout << "Data motor berhasil dihapus.\n";
    } catch (const exception &e) {
        cout << "Error menghapus data motor: " << e.what() << "\n";
    }
}

int menu_pegawai() {
    while (true) {
        handleInput();
        clearScreen();
        if (AdaDalamTeks(menuItems[currentSelection], '1')) {
            tambahMotor("bengkel.csv");
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (AdaDalamTeks(menuItems[currentSelection], '2')) {
            tabelAntrean("bengkel.csv");
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (AdaDalamTeks(menuItems[currentSelection], '3')) {
            servisMotor("bengkel.csv");
        } else if (AdaDalamTeks(menuItems[currentSelection], '4')) {
            if (isCSVKosong("bengkel.csv")) {
                cout << "Antrean kosong.\n";
                cout << "Tekan enter untuk kembali\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            tabelAntrean("bengkel.csv");
            string input;
            int hapus;
            cout << "Masukkan nomor data yang akan dihapus (0 untuk kembali): ";
            getline(cin, input);

            if (input.empty()) {
                cout << "Input tidak boleh kosong.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            bool isValidNumber = true;
            for (char c : input) {
                if (!isdigit(c)) {
                    isValidNumber = false;
                    break;
                }
            }

            if (!isValidNumber) {
                cout << "Input harus berupa angka.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            hapus = stoi(input);
            if (hapus == 0) {
                continue;
            }

            ifstream inFile("bengkel.csv");
            vector<DataBengkel> antrean;
            string line;
            while (getline(inFile, line)) {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);
                if (line.empty()) {
                    continue;
                }
                stringstream ss(line);
                DataBengkel bengkel;
                getline(ss, bengkel.namaMotor, ',');
                getline(ss, bengkel.noPlat, ',');
                getline(ss, bengkel.noHp, ',');
                string lama_servis_str;
                getline(ss, lama_servis_str, ',');
                try {
                    bengkel.lama_servis = lama_servis_str.empty() ? 0 : stoi(lama_servis_str);
                } catch (...) {
                    bengkel.lama_servis = 0;
                }
                getline(ss, bengkel.statusServis, ',');
                getline(ss, bengkel.tglMasuk, ',');
                antrean.push_back(bengkel);
            }
            inFile.close();

            if (hapus < 1 || hapus > antrean.size()) {
                cout << "Nomor antrean tidak valid.\n";
                this_thread::sleep_for(chrono::seconds(2));
                continue;
            }

            hapusDataMotor("bengkel.csv", hapus - 1);
            cout << "Tekan enter untuk kembali\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    }
    return 0;
}