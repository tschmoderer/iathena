# FROM python:3.10.1-bullseye

FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive
ENV TZ Europe/Paris

# INSTALL PYTHON ANC COMPILATOR
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils && apt-get upgrade -y 
RUN apt-get install -y python3-pip cmake
RUN pip3 install virtualenv 

WORKDIR /libot

RUN mkdir src
RUN mkdir build

COPY src/* ./src/

WORKDIR /libot/build
RUN cmake ../src
RUN cmake --build .

WORKDIR /libot/lichess-bot

RUN virtualenv .venv -p python3
ENV PATH="./.venv/bin:$PATH"

COPY lichess-bot/requirements.txt .
RUN pip install -r requirements.txt

RUN mkdir engines 

COPY lichess-bot/*.py ./
COPY lichess-bot/config.yml .
RUN cp ../build/iathena ./engines/

# CLEAN
RUN rm -r ../build
RUN rm -r ../src

CMD ["python", "lichess-bot.py"]