import argparse
import requests
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--base-url', 
        default='0.0.0.0',
        help='The base url of the request. defaults to 0.0.0.0',
        type=str
    )
    parser.add_argument(
        '--port', 
        default=8080,
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
        action='store_true',
        help='Whether or not to add the card to the database',
    )
    return parser.parse_args()

def main(base_url, port, card_bytes, api_key, add):
    api_url = f'http://{base_url}:{port}/api/OfficeAccessCard/verify'
    data = {'api_key': api_key, 'card_bytes': card_bytes, 'add': add}
    logger.info(f'Request payload: {data}')
    response = requests.post(url=api_url, json=data)
    logger.info(f'Response received from {api_url}')
    logger.info(f'Response: {response.text}')

if __name__ == '__main__':
    args = get_args()
    logger.info(f'Arguments retrieved: {args}')
    main(args.base_url, args.port, args.card_bytes, args.api_key, args.add)
