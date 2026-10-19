FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        gdb \
        valgrind \
        python3 \
        python3-pip \
        python3-venv \
        graphviz \
        curl \
        git \
        wget \
        && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /workspace