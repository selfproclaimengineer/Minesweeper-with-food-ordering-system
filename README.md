# Minesweeper with Food Ordering System
Nevertheless, Why can't we have mie ayam while playing minesweeper?

Program ini adalah tugas rancang untuk praktikum dasar pemrograman C. Fitur utama yang harus ada :
- Menu utama: play minesweeper, order makanan, exit warnet
- Sistem Minesweeper berbasis file teks untuk level 10x10 dan 15x15
- Sistem order makanan dengan notifikasi pesanan siap

## File utama
- `level_easy.txt`: file untuk level easy
- `level_hard.txt`: file untuk level hard
- `main.cpp`: kode program C tapi compilenya tetap `.cpp`

## Kompilasi di Windows
Gunakan GCC (misalnya MinGW) atau compiler C lain:

```bash

Atau jika ingin tetap menggunakan file `main.cpp`:

```bash
g++ main.cpp -o warnet.exe
```

Jika ingin memaksa compiler membaca `main.cpp` sebagai C style:

```bash
gcc -x c main.cpp -o warnet.exe
```

## Menjalankan program
Jalankan file ekseskusi, lalu ikuti menu di layar.

## Format file level Minesweeper
File level berisi `0` dan `1` saja.
- `0` artinya kosong
- `1` artinya bom
- Setiap baris harus memiliki jumlah angka sesuai dimensi level
- Contoh untuk 10x10: 10 baris, setiap baris 10 angka
- Contoh untuk 15x15: 15 baris, setiap baris 15 angka

Contoh isi file (10x10):
```
0 0 0 1 0 0 0 0 1 0
0 1 0 0 0 1 0 0 0 0
...
```

Program akan membaca file sesuai level dan menghitung jumlah flag otomatis sesuai jumlah bom.
