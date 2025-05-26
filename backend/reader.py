import sys
import argparse
import requests

def parse_request():
    parser = argparse.ArgumentParser(prog='reader.py')
    parser.add_argument('--base-url', type=str, help='The base url of the request')
    parser.add_argument('--card-bytes', type=int, help='The ID of the card read')
    parser.add_argument('--add', action='store_true', help='Whether or not to add the card to the database')
    request = parser.parse_args()
    print(request)
    send_request(request.base_url, request.card_bytes, request.add)

async def send_request(base_url: str, card_bytes: int, add: bool):
    port_number = 8000 # dummy port number for now -> replace with real port
    api_url = f'http://{base_url}:{port_number}/api/OfficeAccessCard/verify'
    response = await requests.post(url=api_url, data={'card_bytes': card_bytes, 'add': add})
    print(f'Request made to {api_url}')
    print(f'Response: {response}')

if __name__ == '__main__':
    parse_request()
