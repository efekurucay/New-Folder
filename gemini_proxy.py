# gemini_proxy.py - A simple TCP to Gemini API proxy
# Listens for plain text requests and forwards them to the Google Gemini API.

import socket
import http.client
import json
import os
import ssl

HOST = '0.0.0.0'  # Listen on all available network interfaces
PORT = 8081       # The port Minix will connect to

# --- Gemini API Configuration ---
API_KEY = os.environ.get("GEMINI_API_KEY")
API_HOST = "generativelanguage.googleapis.com"
API_PATH = "/v1beta/models/gemini-2.5-flash-lite:generateContent"

def get_gemini_response(prompt):
    """Sends a prompt to the Gemini API and returns the text response."""
    if not API_KEY:
        return "ERROR: GEMINI_API_KEY environment variable is not set on the proxy server."

    headers = {'Content-Type': 'application/json'}
    body = {
        "contents": [{
            "parts": [{"text": prompt}]
        }]
    }

    try:
        # Use SSL for a secure HTTPS connection
        context = ssl.create_default_context()
        conn = http.client.HTTPSConnection(API_HOST, context=context)

        conn.request("POST", f"{API_PATH}?key={API_KEY}", json.dumps(body), headers)
        response = conn.getresponse()
        response_data = response.read()

        if response.status != 200:
            return f"ERROR: API request failed with status {response.status}: {response_data.decode()}"

        data = json.loads(response_data)
        text_response = data['candidates'][0]['content']['parts'][0]['text']
        return text_response

    except Exception as e:
        return f"ERROR: An exception occurred on the proxy server: {e}"
    finally:
        if 'conn' in locals():
            conn.close()

def main():
    """Main server loop."""
    print(f"Time Gateway Proxy starting on {HOST}:{PORT}")
    if not API_KEY:
        print("\nWARNING: GEMINI_API_KEY environment variable is not set.")
        print("The proxy will run but will not be able to contact the Gemini API.\n")


    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print("Waiting for a connection from a Minix machine...")

        while True:
            conn, addr = s.accept()
            with conn:
                print(f"Connected by {addr}")
                try:
                    # Read the incoming question (expects a single line ending in \n)
                    data = conn.recv(1024)
                    if not data:
                        continue

                    prompt = data.decode('utf-8').strip()
                    print(f"Received prompt: {prompt}")

                    # Get response from Gemini
                    print("Asking Gemini...")
                    gemini_text = get_gemini_response(prompt)
                    print("Received response. Sending back to Minix.")

                    # Send the plain text response back to the Minix client
                    conn.sendall(gemini_text.encode('utf-8'))

                except Exception as e:
                    print(f"An error occurred: {e}")
                finally:
                    print(f"Connection with {addr} closed.")

if __name__ == '__main__':
    main()