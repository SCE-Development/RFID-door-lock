import argparse
import json

from fastapi import FastAPI, Response
from fastapi.middleware.cors import CORSMiddleware
import logging
import uvicorn


parser = argparse.ArgumentParser()
parser.add_argument("--host", required=True)
parser.add_argument("--port", type=int, required=True)
parser.add_argument("--database-path", required=True)
args = parser.parse_args()

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

database = {}
with open(args.database_path) as f:
  database = json.load(f)


@app.get("/verify")
async def get_all_urls(data: str = None):
    if data in database:
      return Response(status_code=200)
    return Response(status_code=404)


if __name__ == "__main__":
    logging.info(f"running on {args.host}, listening on port {args.port}")
    uvicorn.run("server:app", host=args.host, port=args.port, reload=True)
