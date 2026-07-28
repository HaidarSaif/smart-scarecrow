# 🦅 Smart Scarecrow

## 📖 Deskripsi Proyek
Inovasi sistem pertanian modern berbasis *Internet of Things* (IoT) dan *Computer Vision* yang berhasil meraih pendanaan nasional pada Program Kreativitas Mahasiswa bidang Karsa Cipta (PKM-KC). Alat ini dirancang untuk mengoptimalkan kualitas lahan sekaligus memproteksi tanaman dari hama perusak secara otomatis dan ramah lingkungan.

Sistem mengintegrasikan Raspberry Pi dan kamera untuk mendeteksi hama burung, kemudian memancarkan gelombang ultrasonik untuk mengusir hama tersebut. Dilengkapi sistem pemantauan pH dan kelembapan, alat ini dapat melakukan injeksi cairan dolomit penstabil pH dan penyiraman air secara otomatis. Seluruh operasional ditenagai oleh energi mandiri (PLTS).

**Fitur Utama:**
*   **Deteksi Hama Visual:** Menggunakan algoritma YOLOv5 untuk mendeteksi burung secara akurat.
*   **Pengusir Ultrasonik:** Memancarkan suara frekuensi tinggi khusus untuk hama tanpa mengganggu manusia.
*   **Auto-pH Balancing:** Membaca tingkat keasaman tanah dan menyemprotkan cairan dolomit secara otonom.
*   **Sistem Penyiraman:** Menyiram lahan secara otomatis berdasarkan sensor kelembapan tanah.
*   **IoT & Mobile App:** Pemantauan dan kontrol parameter terintegrasi melalui aplikasi *mobile*.

## 🚀 Teknologi yang Digunakan
*   **Hardware:** Raspberry Pi, ESP32, Kamera AI, Sensor pH Tanah, Sensor Kelembapan, Mini PLTS.
*   **Software:** Python, C++, YOLOv5 (Computer Vision), Mobile App Framework.

## 📂 Struktur Direktori
*   `/computer_vision` - Berisi *script* inferensi YOLOv5 (Python) dan logika deteksi pada Raspberry Pi.
*   `/microcontroller` - Berisi *source code* (C++) untuk ESP32 pengatur aktuator penyiraman dan ultrasonik.
*   `/mobile_app` - Berisi file aplikasi mobile yang diekspor dari MIT APP Inventor.
*   `/documentation` - Berisi foto dokumentasi dan laporan akhir proyek.
<img width="1332" height="800" alt="smartscarecrow" src="https://github.com/user-attachments/assets/0f6e968c-2a0c-470b-944c-f5f3540c8267" />
