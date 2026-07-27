import socket
import requests
import json

 #URL dasar Realtime Database Anda
database_url = 'https://scarecrow-pimnas2024-default-rtdb.firebaseio.com/'

# Kode Auth Firebase Anda
auth_key = 'DEt91GPe97hFTdFsDRlYHPt2v3yoZqDeVEOP0nnu'

# Fungsi untuk mengirim data ke Realtime Database
def send_data_to_firebase(reference, data):
    url = f'{database_url}{reference}.json?auth={auth_key}'
    response = requests.put(url, data=json.dumps(data))
    if response.status_code == 200:
        print(f"Data sent to {reference}: {data}")
    else:
        print(f"Failed to send data: {response.status_code}, {response.text}")

def read_data_from_firebase(reference):
    url = f'{database_url}{reference}.json?auth={auth_key}'
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        return data
    else:
        print(f"Failed to read data from Firebase: {response.status_code}")
        return None

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('0.0.0.0', 8083))
    server_socket.listen(1)
    print("Server started and listening for connections...")

    while True:
        client_socket, addr = server_socket.accept()
        print(f"Connection from {addr} has been established.")

        while True:
            data = client_socket.recv(1024).decode()
            if not data:
                break
            print(f"Received from ESP32: {data}")

            # Kirim data ke Firebase
            send_data_to_firebase('A23DR562GHHG6DSG/ESP32Data',data + "&" + "0")

            # Baca data dari Firebase
            response = read_data_from_firebase('A23DR562GHHG6DSG/phoneData')
            response = response + "&" + "0"
            client_socket.send(response.encode())

        client_socket.close()

if __name__ == "__main__":
    start_server()
