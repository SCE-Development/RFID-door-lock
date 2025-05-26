import argparse
import requests
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--base-url', 
        default='0.0.0.0',
        required=True,
        help='The baes url of the request',
        type=str
    )
    parser.add_argument(
        '--port', 
        required=True,
        type=int,
        help='The port number of the API request'
    )
    parser.add_argument(
        '--card-bytes',
        required=True,
        help='The ID of the card read',
        type=int
    )
    parser.add_argument(
        '--api-key',
        required=True,
        type=str,
        help='API key for secure access'
    )
    parser.add_argument(
        '--add',
        required=False,
        action='store_true',
        help='Whether or not to add the card to the database',
    )
    return parser.parse_args()

def main(base_url, port, card_bytes, api_key, add):
    api_url = f'http://{base_url}:{port}/api/OfficeAccessCard/verify'
    data = {'api_key': api_key, 'card_bytes': card_bytes, 'add': add}
    logger.debug(f'Request payload: {data}')
    response = requests.post(url=api_url, json=data)
    logger.debug(f'Response received from {api_url}')
    logger.debug(f'Response: {response.text}')

if __name__ == '__main__':
    logger.debug('Program has begun')
    args = get_args()
    logger.debug(f'Arguments retrieved: {args}')
    main(args.base_url, args.port, args.card_bytes, args.api_key, args.add)
