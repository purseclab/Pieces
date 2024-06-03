FROM python:3.8 

WORKDIR /app

RUN apt-get update && apt-get install -y git && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/purseclab/Pieces.git .

RUN pip install --no-cache-dir -r requirements.txt


CMD ["python3", "./run.py", "./rules/ardu.json"]
