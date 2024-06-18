FROM python:3.8 

WORKDIR /app

RUN apt-get update && apt-get install -y git vim git-lfs google-perftools libunwind8 

RUN git clone https://github.com/purseclab/Pieces.git .

ADD "https://api.github.com/repos/purseclab/Pieces/commits?per_page=1" latest_commit
RUN curl -sLO "https://github.com/purseclab/Pieces/archive/main.zip" && unzip main.zip

WORKDIR "/app/Pieces-main/"
RUN pip install --no-cache-dir -r requirements.txt
RUN git lfs fetch --all

RUN mkdir -p /home/arslan/projects/ardupilot/ardupilot/build/Pixhawk6C/bin/
COPY ./build_ctxt/ardu/arducopter.bc /home/arslan/projects/ardupilot/ardupilot/build/Pixhawk6C/bin/
COPY ./build_ctxt/klee /app/compiler/bin/
RUN mkdir -p /app/compiler/lib/
COPY ./temp/* /lib/x86_64-linux-gnu/
RUN  mkdir -p /home/arslan/projects/LBC/SVF//Debug-build/bin/
COPY ./build_ctxt/svf/* /home/arslan/projects/LBC/SVF//Debug-build/bin/
RUN mkdir -p /home/arslan/projects/KLEE/klee/build2/runtime/lib/
COPY ./build_ctxt/klee/* /app/compiler/bin/
COPY ./build_ctxt/runtime/* /home/arslan/projects/KLEE/klee/build2/runtime/lib/
RUN mkdir -p  /app/partitioner/out/
COPY ./partitioner/out/* /app/partitioner/out/
WORKDIR "/app/"
RUN git pull
CMD ["python3", "./run.py", "./rules/ardu.json"]
