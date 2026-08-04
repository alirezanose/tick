# Guide: Bikin Timer TUI di C (ala timr-tui)

Gaya guide ini: tiap bagian jelasin **konsep + kenapa perlu**, kamu yang nulis implementasinya sendiri.

---

## 1. Kenapa ncurses?

`printf` biasa gak bisa: update di posisi tertentu tanpa scroll, baca input tanpa Enter, atau nge-refresh layar secara terkontrol. Itu semua job-nya sebuah **terminal library**. Di C, standar untuk itu adalah **ncurses**.

Fungsi inti yang wajib kamu ketahui (cari di `man`):
- `initscr()` — masuk mode ncurses
- `endwin()` — keluar mode ncurses (WAJIB dipanggil sebelum exit, atau terminal kamu rusak)
- `noecho()` — input gak ditampilin otomatis
- `curs_set(0)` — sembunyiin kursor
- `refresh()` — nge-flush apa yang digambar ke layar beneran

**Poin penting:** ncurses punya "virtual screen" di memori, kamu gambar ke situ dulu, baru `refresh()` nampilinnya. Ini beda dari `printf` yang langsung nulis.

---

## 2. State: apa yang perlu disimpan?

Sebuah timer minimal butuh nyimpen:
- sisa waktu (dalam detik, lebih gampang dari HH:MM:SS langsung)
- status: jalan / pause
- mode: countdown / stopwatch (opsional buat awal)

**Poin penting:** pisahin **state** (data timer) dari **rendering** (nampilin ke layar). Ini pattern umum di semua TUI/GUI: `update state → render(state)`. Jangan campur logic hitung detik dengan `mvprintw`.

---

## 3. Event loop: jantung dari TUI

TUI itu beda dari program C biasa yang baca input terus selesai. TUI harus:
1. cek input (tanpa nge-block kalau gak ada input)
2. update state (misal: udah lewat 1 detik? kurangin counter)
3. render ulang
4. ulangi

Fungsi kunci: `timeout(ms)` di ncurses — bikin `getch()` **non-blocking** dengan batas waktu tunggu. Cari tahu apa yang dikembalikan `getch()` kalau gak ada input dalam waktu itu.

**Poin penting:** loop ini biasanya jalan tiap 100-200ms, BUKAN tiap 1 detik. Kenapa? Supaya input berasa responsif. Tapi itu artinya kamu gak bisa asal "counter-- tiap iterasi" — itu bakal ngitung lebih cepat dari detik asli.

---

## 4. Ngukur waktu asli, bukan ngitung iterasi loop

Karena loop jalan lebih cepat dari 1 detik, kamu butuh cara ngukur waktu **beneran** berlalu. Cari tahu:
- `time(NULL)` dari `<time.h>` — resolusi 1 detik, paling gampang buat mulai
- `clock_gettime(CLOCK_MONOTONIC, ...)` — kalau butuh presisi sub-detik nanti

**Poin penting:** simpan timestamp terakhir kamu update counter. Tiap iterasi loop, bandingin waktu sekarang dengan timestamp itu — kalau udah beda ≥1 detik, baru update state dan simpan timestamp baru.

---

## 5. Format tampilan: detik → HH:MM:SS

Ini murni matematika: pakai `/` dan `%` terhadap 3600 dan 60. Gak ada library khusus, tinggal `sprintf` ke buffer string.

---

## 6. Input handling: mapping key ke aksi

Setelah `getch()` kasih kode key, kamu `switch` ke aksi:
- pause/resume
- reset
- quit
- (nanti) ganti mode / tab

**Poin penting:** pikirin state machine-nya — apa yang boleh terjadi kalau timer lagi paused vs running? Misal reset boleh kapan aja, tapi apa espace/quit harus konfirmasi dulu?

---

## 7. (Lanjutan) Fitur ala timr-tui kalau mau dikembangkan

| Fitur | Konsep C yang dipakai |
|---|---|
| Big ASCII digit (jam raksasa) | array 2D string, lookup per digit |
| Multiple timer/tab | array of struct + index aktif |
| Config file | file I/O + parsing string (`fscanf`/`strtok`) |
| Suara notifikasi | `system()` panggil player, atau `\a` bell |
| Simpan sesi terakhir | serialize struct ke file biner (`fwrite`/`fread`) |

---

## Urutan belajar yang disaranin

1. ncurses paling basic — tampilin teks statis, keluar pas tekan `q`
2. Stopwatch yang cuma nambah (belum ada pause/reset)
3. Tambah pause & reset
4. Rapikan ke format HH:MM:SS
5. (opsional) besarin tampilan / tambah fitur dari tabel di atas

---

## Tantangan ngulik

Implementasikan poin 1-2 dulu: ncurses nampilin angka detik yang nambah terus tiap detik asli (pakai `time()`), keluar pas tekan `q`. Belum perlu pause. Share kalau ada yang error atau kalau counter-nya jalan gak sesuai detik asli.
