FROM python:3.8 

WORKDIR /app

RUN apt-get update && apt-get install -y git && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/purseclab/Pieces.git .

ADD "https://api.github.com/repos/purseclab/Pieces/commits?per_page=1" latest_commit
RUN curl -sLO "https://github.com/purseclab/Pieces/archive/main.zip" && unzip main.zip

WORKDIR "/app/Pieces-main/"
RUN pip install --no-cache-dir -r requirements.txt

RUN mkdir -p /home/arslan/projects/ardupilot/ardupilot/build/Pixhawk6C/bin/
COPY ./build_ctxt/ardu/arducopter.bc /home/arslan/projects/ardupilot/ardupilot/build/Pixhawk6C/bin/

WORKDIR "/app/Pieces-main/partitioner/"
CMD ["python3", "./run.py", "./rules/ardu.json"]
