#define byte windows_byte
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
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

const int MAX_ROWS = 10;
const int MAX_COLS = 4;

struct PaketServis
{
    int id;
    string namaPaket;
    int lamaServis;
};

void hapusDataMotor(const string &filename, int rowToDelete);

void tambahMotor(string filename, string nama_motor, string no_plat, string no_hp, int paket_servis, string tgl_masuk)
{
    ofstream data(filename, ios::app);

    PaketServis paket[] = {
        {1, "Paket servis ringan", 10},
        {2, "Paket servis lengkap", 6},
        {3, "Paket servis berat", 3}};

    cout << "Masukkan nama motor: ";
    getline(cin, nama_motor);
    cout << "Masukkan nomor plat motor: ";
    getline(cin, no_plat);
    cout << "Masukkan nomor hp: ";
    getline(cin, no_hp);

    cout << "Paket Servis Bengkel Mas Tahir\n";
    for (int i = 0; i < 3; ++i)
    {
        cout << paket[i].id << ". " << paket[i].namaPaket << endl;
    }

    int pilihPaket;
    cout << "Pilih paket (1-3): ";
    cin >> pilihPaket;

    if (pilihPaket >= 1 && pilihPaket <= 3)
    {
        paket_servis = paket[pilihPaket - 1].lamaServis;
    }
    else
    {
        cout << "Pilihan tidak valid!" << endl;
    }

    if (data.is_open())
    {
        // Send the column name to the stream
        data << nama_motor;
        data << ",";
        data << no_plat;
        data << ",";
        data << no_hp;
        data << ",";
        data << paket_servis;
        data << ",";
        data << tgl_masuk << "\n";
        // Close the file
        data.close();
    }
    else
    {
        cerr << "Gagal membuka file." << endl;
    }
}

void tabelAntrean(const string &filename, int lengkap1, const string &statusServisUtama, const string &statusServisLain)
{
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cerr << "Gagal membuka file: " << filename << endl;
        return;
    }

    string line;
    int rowNumber = 1;

    // Header tabel
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+" << endl;
    cout << "| No |      Nama Motor      | Plat Nomor |    No HP     | Lama Servis | Status Servis | Tanggal Masuk |" << endl;
    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+" << endl;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        string namaMotor, platNomor, noHp, lamaServis, tglMasuk;

        // Ambil kolom dari CSV
        getline(ss, namaMotor, ',');
        getline(ss, platNomor, ',');
        getline(ss, noHp, ',');
        getline(ss, lamaServis, ',');
        getline(ss, tglMasuk, ',');

        cout << "| " << setw(3) << left << rowNumber;
        cout << "| " << setw(21) << left << namaMotor;
        cout << "| " << setw(11) << left << platNomor;
        cout << "| " << setw(13) << left << noHp;

        if (!lamaServis.empty() && lamaServis != "0")
        {
            cout << "| " << setw(12) << (lamaServis + " jam");
        }
        else
        {
            cout << "| " << setw(13) << " ";
        }

        // Pilih status servis sesuai baris
        if (rowNumber == 1 && statusServisUtama != "tunggu konfirmasi")
        {
            cout << "| " << setw(14) << statusServisUtama;
        }
        else
        {
            cout << "| " << setw(14) << statusServisLain;
        }

        cout << "| " << setw(14) << tglMasuk;
        cout << "|" << endl;

        rowNumber++;
    }

    cout << "+----+----------------------+------------+--------------+-------------+---------------+---------------+" << endl;

    inFile.close();
}

bool isCSVKosong(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
        return true; // Jika file tidak bisa dibuka, anggap kosong

    string line;
    while (getline(file, line))
    {
        // Cek apakah ada baris yang tidak kosong atau tidak hanya whitespace
        if (!line.empty() && line.find_first_not_of(" \t\n\r") != string::npos)
        {
            return false; // Ada isi
        }
    }

    return true; // Tidak ada baris berarti file kosong
}

void servisMotor(const string &filename)
{
    string mulai;
    do
    {
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "Gagal membuka file: " << filename << endl;
            return;
        }

        string line;
        getline(inFile, line); // hanya baca antrean pertama
        inFile.close();

        system("clear || cls");

        string status = "tunggu konfirmasi";
        tabelAntrean("bengkel.csv", 1, status, "Menunggu");

        cout << "Lakukan Servis (y/t)? : ";
        cin >> mulai;

        if (mulai == "y")
        {
            stringstream ss(line);
            string namaMotor;
            getline(ss, namaMotor, ',');

            system("clear || cls");
            cout << "Proses servis motor " << namaMotor << " sedang dilakukan" << endl;

            status = "Dikerjakan";
            tabelAntrean("bengkel.csv", 1, status, "Menunggu");

            // sleep(10);

            system("clear || cls");
            cout << "Motor telah selesai diservis" << endl;

            status = "Selesai";
            tabelAntrean("bengkel.csv", 1, status, "Menunggu");

            cout << "Sedang update antrean" << endl;
            // sleep(5);
            
            system("clear || cls");

            hapusDataMotor("bengkel.csv", 0); // hapus antrean pertama

            status = "";
            tabelAntrean("bengkel.csv", 1, status, "Menunggu");

            if (isCSVKosong("bengkel.csv"))
            {
                system("clear || cls");
                cout << "Servis dihentikan, karena tidak ada antrean motor lagi" << endl;
                break;
            }
        }
        else
        {
            cout << "\nProses servis motor dihentikan" << endl;
            break;
        }

    } while (mulai == "y" && !isCSVKosong("bengkel.csv"));
}

void updateDataMotor(const string &filename)
{
    int inputRow;
    DataBengkel motor;

    cout << "Pilih antrean yang ingin diupdate: ";
    cin >> inputRow;
    cin.ignore(); // membersihkan newline dari buffer

    cout << "Update nama motor: ";
    getline(cin, motor.namaMotor);
    cout << "Update nomor plat motor: ";
    getline(cin, motor.noPlat);
    cout << "Update nomor hp: ";
    getline(cin, motor.noHp);
    cout << "Update tanggal masuk ke bengkel: ";
    getline(cin, motor.tglMasuk);

    ifstream inFile(filename);
    ofstream tempFile("temp.csv");

    string line;
    int currentRow = 0;

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Gagal membuka file." << endl;
        return;
    }

    while (getline(inFile, line))
    {
        stringstream ss(line);
        string cell;
        string newLine;
        int currentCol = 0;

        if (currentRow == inputRow - 1)
        {
            while (getline(ss, cell, ','))
            {
                if (currentCol == 0)
                    newLine += motor.namaMotor;
                else if (currentCol == 1)
                    newLine += motor.noPlat;
                else if (currentCol == 2)
                    newLine += motor.noHp;
                else if (currentCol == 3)
                    newLine += motor.tglMasuk;
                else
                    newLine += cell;

                if (ss.peek() != EOF)
                    newLine += ",";
                currentCol++;
            }
        }
        else
        {
            newLine = line;
        }

        tempFile << newLine << endl;
        currentRow++;
    }

    inFile.close();
    tempFile.close();

    remove(filename.c_str());
    rename("temp.csv", filename.c_str());
}

void hapusDataMotor(const string &filename, int rowToDelete)
{
    ifstream inFile(filename);
    ofstream tempFile("temp.csv");
    string line;
    int currentRow = 0;

    if (!inFile.is_open() || !tempFile.is_open())
    {
        cerr << "Gagal membuka file." << endl;
        return;
    }

    // Salin semua baris kecuali baris ke-n
    while (getline(inFile, line))
    {
        if (currentRow != rowToDelete)
        {
            tempFile << line << endl;
        }
        currentRow++;
    }
    cout << rowToDelete << endl;

    inFile.close();
    tempFile.close();

    // Hapus file asli dan ganti dengan file baru
    remove(filename.c_str());
    rename("temp.csv", filename.c_str());
}

const char *menuItems[] = {
    "1. Tambah Motor",
    "2. Lihat Antrean",
    "3. Servis Motor",
    "4. Update Data Motor",
    "5. Hapus Motor",
    "0. Keluar"};

const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);
int currentSelection = 0;

#ifdef _WIN32
void clearScreen()
{
    system("cls");
}
#else
// getch alternatif untuk Linux
int _getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt); // simpan mode lama
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // non-canonical, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // terapkan mode baru
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // kembalikan mode lama
    return ch;
}

void clearScreen()
{
    system("clear");
}
#endif

void displayMenu()
{
    clearScreen();
    cout << "Gunakan panah atas/bawah, tekan Enter untuk pilih:\n\n";
    for (int i = 0; i < menuSize; i++)
    {
        // cout << i << endl;
        // cout << currentSelection << endl;
        if (i == currentSelection)
            cout << " -> " << menuItems[i] << "\n";
        else
            cout << "     " << menuItems[i] << "\n";
    }
}

void handleInput()
{
    while (true)
    {
        displayMenu();
        int key = _getch();

#ifdef _WIN32
        if (key == 224)
        {
            int arrow = _getch();
            if (arrow == 72)
            { // Up
                currentSelection = (currentSelection - 1 + menuSize) % menuSize;
            }
            else if (arrow == 80)
            { // Down
                currentSelection = (currentSelection + 1) % menuSize;
            }
        }
        else if (key == 13)
        { // Enter
            break;
        }
#else
        if (key == 27)
        { // Escape
            int second = _getch();
            if (second == 91)
            {
                int arrow = _getch();
                if (arrow == 65)
                { // Up
                    currentSelection = (currentSelection - 1 + menuSize) % menuSize;
                }
                else if (arrow == 66)
                { // Down
                    currentSelection = (currentSelection + 1) % menuSize;
                }
            }
        }
        else if (key == 10)
        { // Enter
            break;
        }
#endif
    }
}

string tanggalMasuk()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char buffer[11]; // Format: "YYYY-MM-DD" + null terminator
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

bool AdaDalamTeks(const string &teks, char yangDicari)
{
    return teks.find(yangDicari) != string::npos;
}

int menu_pegawai()
{
    DataBengkel motor;

    while (true)
    {
        handleInput();
        clearScreen();
        if (AdaDalamTeks(menuItems[currentSelection], '1'))
        {
            tambahMotor("bengkel.csv", motor.namaMotor, motor.noPlat, motor.noHp, motor.lama_servis, tanggalMasuk());
            handleInput();
            clearScreen();
        }
        else if (AdaDalamTeks(menuItems[currentSelection], '2'))
        {
            tabelAntrean("bengkel.csv", 1, "tunggu konfirmasi", "Menunggu");
            cout << "Tekan enter untuk keluar" << endl;
            cin.ignore();
        }
        else if (AdaDalamTeks(menuItems[currentSelection], '3'))
        {
            servisMotor("bengkel.csv");
            handleInput();
            clearScreen();
        }
        else if (AdaDalamTeks(menuItems[currentSelection], '4'))
        {
            updateDataMotor("bengkel.csv");
            handleInput();
            clearScreen();
        }
        else if (AdaDalamTeks(menuItems[currentSelection], '5'))
        {
            int hapus;
            cout << "Masukkan motor yang ingin dihapus: ";
            cin >> hapus;
            cout << hapus << endl;
        
            hapusDataMotor("bengkel.csv", hapus - 1);
            handleInput();
            clearScreen();
        }
        else 
        {
            break;
        }
    }
    return 0;
}
